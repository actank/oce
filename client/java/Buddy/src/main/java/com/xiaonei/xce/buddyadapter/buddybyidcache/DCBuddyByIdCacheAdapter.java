package com.xiaonei.xce.buddyadapter.buddybyidcache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.LinkedBlockingDeque;

import xce.distcache.DistCacheAdapter;
import xce.distcache.buddycache.Buddy.FriendData;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.log.Oce;

/**
 * @author yuyang
 * 
 */
public class DCBuddyByIdCacheAdapter extends Thread {

    private DistCacheAdapter _adapter;

    private LinkedBlockingDeque<Long> _queue = new LinkedBlockingDeque<Long>(1000);

    private DCBuddyByIdCacheReloaderAdapter _reloader;

    public DCBuddyByIdCacheAdapter() {
        start();
    }

    private DistCacheAdapter getAdapter() {
        if (_adapter == null) {
            synchronized (this) {
                if (_adapter == null) {
                    _adapter = DistCacheAdapter.connect("BuddyByIdCache");
                    return _adapter;
                }
            }
        }
        return _adapter;
    }

    private DCBuddyByIdCacheReloaderAdapter getReloader() {
        if (_reloader == null) {
            synchronized (this) {
                if (_reloader == null) {
                    _reloader = new DCBuddyByIdCacheReloaderAdapter();
                    return _reloader;
                }
            }
        }
        return _reloader;
    }

    public void run() {
        while (true) {
            try {
                List<Long> ids = new ArrayList<Long>();
                _queue.drainTo(ids, 100);
                int[] id = new int[ids.size()];
                for (int i = 0; i < ids.size(); i++) {
                    id[i] = ids.get(i).intValue();
                }
                getReloader().reload(id);
                Thread.sleep(1000);
            } catch (Exception e) {}
        }
    }

    /**
     * @param userId
     * @return
     */
    public int getFriendCount(int userId) {
        int count = 0;
        byte[] res = null;
        try {
            res = getAdapter().get(userId);
        } catch (ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCount " + userId
                            + " Ice.ConnectTimeoutException");
            e.printStackTrace();
        } catch (TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCount " + userId
                            + " Ice.TimeoutException");
            e.printStackTrace();
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error(this.getClass().getName() + ".getBuddyFriendCount " + userId, e);
            e.printStackTrace();
        }
        if (res.equals(null)) {
            _queue.offer((long) userId);
            return count;
        }

        try {
            FriendData data = FriendData.parseFrom(res);
            count = data.getSize();

        } catch (InvalidProtocolBufferException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return count;
    }

    /**
     * @param userIds
     * @return
     */
    public Map<Integer, Integer> getFriendCounts(List<Integer> userIds) {
        List<Long> ids = new ArrayList<Long>();
        for (Integer i : userIds) {
            ids.add(i.longValue());
        }
        Map<Integer, Integer> res = new HashMap<Integer, Integer>();

        try {
            Map<Long, byte[]> results = getAdapter().get(ids);
            for (Entry<Long, byte[]> obj : results.entrySet()) {
                res.put(obj.getKey().intValue(), FriendData.parseFrom(obj.getValue()).getSize());
            }
        } catch (ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCounts " + userIds.size()
                            + " Ice.ConnectTimeoutException");
            e.printStackTrace();
        } catch (TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCounts " + userIds.size()
                            + " Ice.TimeoutException");
            e.printStackTrace();
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCounts " + userIds.size(), e);
            e.printStackTrace();
        }

        for (Integer id : userIds) {
            if (!res.containsKey(id)) {
                _queue.offer(id.longValue());
            }
        }

        return res;
    }

    /**
     * @param userId
     * @param begin
     * @param limit
     * @return
     */
    public List<Integer> getFriendListAsc(int userId, int begin, int limit) {
        List<Integer> res = new ArrayList<Integer>();
        byte[] result = null;
        try {
            result = getAdapter().get(userId);
        } catch (ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCount " + userId
                            + " Ice.ConnectTimeoutException");
            e.printStackTrace();
        } catch (TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCount " + userId
                            + " Ice.TimeoutException");
            e.printStackTrace();
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error(this.getClass().getName() + ".getBuddyFriendCount " + userId, e);
            e.printStackTrace();
        }
        if (result.equals(null)) {
            _queue.offer((long) userId);
            return res;
        }

        try {
            FriendData data = FriendData.parseFrom(result);
            res = data.getFriendList();
        } catch (InvalidProtocolBufferException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return res;
    }

    /**
     * @param userIds
     * @return
     */
    public Map<Integer, List<Integer>> getFriendListsAsc(List<Integer> userIds) {
        Map<Integer, List<Integer>> res = new HashMap<Integer, List<Integer>>();
        List<Long> ids = new ArrayList<Long>();
        for (Integer i : userIds) {
            ids.add(i.longValue());
        }

        try {
            Map<Long, byte[]> results = getAdapter().get(ids);
            for (Entry<Long, byte[]> obj : results.entrySet()) {
                res.put(obj.getKey().intValue(), FriendData.parseFrom(obj.getValue())
                        .getFriendList());
            }
        } catch (ConnectTimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCounts " + userIds.size()
                            + " Ice.ConnectTimeoutException");
            e.printStackTrace();
        } catch (TimeoutException e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCounts " + userIds.size()
                            + " Ice.TimeoutException");
            e.printStackTrace();
        } catch (java.lang.Throwable e) {
            Oce.getLogger().error(
                    this.getClass().getName() + ".getBuddyFriendCounts " + userIds.size(), e);
            e.printStackTrace();
        }

        for (Integer id : userIds) {
            if (!res.containsKey(id)) {
                _queue.offer(id.longValue());
            }
        }

        return res;
    }

    public static void main(String[] args) {
        // TODO Auto-generated method stub
        DCBuddyByIdCacheAdapter adapter = new DCBuddyByIdCacheAdapter();
        int count = adapter.getFriendCount(221444915);
        System.out.println(count);
        List<Integer> data = adapter.getFriendListAsc(221444915, 0, -1);
        System.out.println(data.size());
    }

}
