package com.xiaonei.xce.socialgraph;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;

import com.google.protobuf.InvalidProtocolBufferException;
import com.sun.mail.iap.Argument;
import com.xiaonei.xce.socialgraph.bean.ClusterIndexCount;
import com.xiaonei.xce.socialgraph.proto.FriendClusters;
import com.xiaonei.xce.socialgraph.proto.FriendClusters.FriendCluster;
import com.xiaonei.xce.socialgraph.proto.FriendClusters.Person;

import xce.distcache.DistCacheAdapter;

public class SocialgraphAdapterOld implements Observer {

    private static SocialgraphAdapterOld instance = null;

    private ConcurrentHashMap<String, DistCacheAdapter> distCacheAdapterMap;

    private ConcurrentHashMap<String, Integer> bizNameTypeMap;

    private static final String ZNODE_PATH = "/BIZ";

    public static SocialgraphAdapterOld getInstance() {
        if (instance == null) {
            synchronized (SocialgraphAdapterOld.class) {
                if (instance == null) {
                    try {
                        instance = new SocialgraphAdapterOld();
                    } catch (Exception e) {
                        e.printStackTrace();
                        instance = null;
                    }
                }
            }
        }
        return instance;
    }

    private SocialgraphAdapterOld() throws IOException {
        /*
        ZookeeperClient.getInstance().attach(this);
        distCacheAdapterMap = new ConcurrentHashMap<String, DistCacheAdapter>();
        String znodeValue = ZookeeperClient.getInstance().getValue(ZNODE_PATH);
        if (znodeValue == null) {
            throw new IOException("cannot get data from zookeeper");
        }
        System.out.println(znodeValue);
        String[] units = znodeValue.split("#");
        for (String unit : units) {
            int pos = unit.indexOf(",");
            String bizName = unit.substring(0, pos);
            int bizType = Integer.parseInt(unit.substring(pos + 1, unit.length()));
            System.out.println("SocialgraphAdapter() bizName:" + bizName + " bizType:" + bizType);
            bizNameTypeMap.put(bizName, bizType);
        }
        */

        distCacheAdapterMap = new ConcurrentHashMap<String, DistCacheAdapter>();
        // 现在 只为FFCU FFUC做,为了guide的需求
        List<String> bizList = new ArrayList<String>();
        bizList.add("FFUC");
        bizList.add("FFCU");
        for (String biz : bizList) {
            for (int i = 0; i < 10; ++i) {
                try {
                    DistCacheAdapter adapter = DistCacheAdapter.connect(biz);
                    if (adapter == null) {
                        System.err.println("DistCacheAdapter.connect() return null  biz:" + biz);
                        TimeUnit.MILLISECONDS.sleep(20);
                        continue;
                    } else {
                        System.out.println("init biz:" + biz);
                        distCacheAdapterMap.put(biz, adapter);
                        break;
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        }
    }

    @Override
    public void update(String key, String value) {
        if (key.equals(ZNODE_PATH)) {
            // TODO: update
        }
    }

    private List<Integer> getFriendClusterIndexList(final int userId) {
        List<Integer> ret = new ArrayList<Integer>();
        final String bizUCName = "FFUC";
        final int bizUCType = 16;
        if (distCacheAdapterMap.containsKey(bizUCName)) {
            long l = bizUCType;
            l = (l << 32) + userId;
            byte[] data = distCacheAdapterMap.get(bizUCName).get(l);
            if (data != null) {
                Person person = null;
                try {
                    person = Person.parseFrom(data);
                } catch (InvalidProtocolBufferException e) {
                    e.printStackTrace();
                }
                if (person != null) {
                    for (int i = 0; i < person.getClusterindexseqCount(); ++i) {
                        ret.add(person.getClusterindexseq(i));
                    }
                }

            }
        }
        return ret;
    }

    /**
     * 取一个用户的好友簇数据.数据顺序是:按簇分数
     * 
     * @param userId
     * @param begin 必须>=0
     * @param limit 必须>0
     * @return
     */
    public List<Integer> getFriendClusterPeople(final int userId, final int begin, final int limit) {
        return new ArrayList<Integer>();
        /*
        if (begin < 0 || limit <= 0) {
            return new ArrayList<Integer>();
        }
        Set<Integer> peopleSet = new HashSet<Integer>();
        int count = 0;
        List<Integer> friendClusterIndexList = getFriendClusterIndexList(userId);

        for (Integer friendClusterIndex : friendClusterIndexList) {
            final String bizName = "FFCU";
            final int bizType = 64;
            long l = bizType;
            l = (l << 32) + friendClusterIndex;
            if (!distCacheAdapterMap.containsKey(bizName)) {
                break;
            } else {
                byte[] data = distCacheAdapterMap.get(bizName).get(l);
                if (data == null) {
                    continue;
                }
                try {
                    FriendCluster friendCluster = FriendCluster.parseFrom(data);
                    if (friendCluster != null) {
                        for (int i = 0; i < friendCluster.getUseridseqCount() && count < limit; ++i) {
                            peopleSet.add(friendCluster.getUseridseq(i));
                            ++count;
                        }
                    }
                    if (count >= limit) {
                        break;
                    }
                } catch (InvalidProtocolBufferException e) {
                    e.printStackTrace();
                }
            }
        }

        return new ArrayList<Integer>(peopleSet);
        */
    }

    /**
     * 取一帮用户的簇数据,数据顺序是:先按照簇出现次数,再按簇分数
     * 
     * @param userIdList
     * @param begin
     * @param limit
     * @return
     */
    public List<Integer> getFriendClusterPeople(List<Integer> userIdList, final int begin,
            final int limit) {
        return new ArrayList<Integer>();
        /*
        if (begin < 0 || limit <= 0 || userIdList == null || userIdList.isEmpty()) {
            return new ArrayList<Integer>();
        }

        Map<Integer, ClusterIndexCount> clusterIndexCountMap = new HashMap<Integer, ClusterIndexCount>();
        for (Integer userId : userIdList) {
            List<Integer> friendClusterIndexList = getFriendClusterIndexList(userId);
            for (Integer clusterIndex : friendClusterIndexList) {
                if (clusterIndexCountMap.containsKey(clusterIndex)) {
                    clusterIndexCountMap.get(clusterIndex).increase();
                } else {
                    clusterIndexCountMap.put(clusterIndex, new ClusterIndexCount(clusterIndex));
                }
            }
        }
        Collection<ClusterIndexCount> coll = clusterIndexCountMap.values();
        List<ClusterIndexCount> list = new ArrayList<ClusterIndexCount>(coll);
        Collections.sort(list);
        Set<Integer> peopleSet = new HashSet<Integer>();
        int count = 0;
        for (ClusterIndexCount clusterIndexCount : list) {
            final String bizName = "FFCU";
            final int bizType = 64;
            long l = bizType;
            l = (l << 32) + clusterIndexCount.getClusterIndex();
            if (!distCacheAdapterMap.containsKey(bizName)) {
                break;
            } else {
                byte[] data = distCacheAdapterMap.get(bizName).get(l);
                if (data == null) {
                    continue;
                }
                try {
                    FriendCluster friendCluster = FriendCluster.parseFrom(data);
                    if (friendCluster != null) {
                        for (int i = 0; i < friendCluster.getUseridseqCount() && count < limit; ++i) {
                            peopleSet.add(friendCluster.getUseridseq(i));
                            ++count;
                        }
                    }
                    if (count >= limit) {
                        break;
                    }
                } catch (InvalidProtocolBufferException e) {
                    e.printStackTrace();
                }
            }
        }
        return new ArrayList<Integer>(peopleSet);
        */
    }

    public static void main(String[] args) {
        int userId = 726996514;
        List<Integer> list = SocialgraphAdapterOld.getInstance()
                .getFriendClusterPeople(userId, 0, 100);
        System.out.println(list);
        System.exit(0);
    }
}
