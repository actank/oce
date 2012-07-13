package com.xiaonei.xce.buddyadapter.commonfriendcache;

import java.util.ArrayList;
import java.util.List;

import xce.buddy.CommonFriendCacheManagerPrx;
import xce.buddy.CommonFriendCacheManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

import com.xiaonei.xce.log.Oce;

/**
 * CommonFriendCache
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */
public class CommonFriendCacheAdapter extends ReplicatedClusterAdapter {

    private static final String endpoints = "ControllerCommonFriendCache";

    private static int interval = 120;

    private static CommonFriendCacheAdapter instance = null;

    private static final int TIME_OUT = 5000;

    /**
     * @return
     */
    public static CommonFriendCacheAdapter getInstance() {
        if (instance == null) {
            synchronized (CommonFriendCacheAdapter.class) {
                if (instance == null) {
                    try {
                        instance = new CommonFriendCacheAdapter();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            }
        }
        return instance;
    }

    /**
	 * 
	 */
    private CommonFriendCacheAdapter() {
        super(endpoints, interval);
    }

    /**
     * @param id
     * @return
     */
    private CommonFriendCacheManagerPrx getCommonFriendCacheManagerPrxTwoWay(int id) {
        Ice.ObjectPrx prx0 = getValidProxy(id, TIME_OUT);
        if (prx0 != null) {
            CommonFriendCacheManagerPrx prx = CommonFriendCacheManagerPrxHelper.uncheckedCast(prx0
                    .ice_timeout(TIME_OUT).ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

    /**
     * @param id
     * @return
     */
    private CommonFriendCacheManagerPrx getCommonFriendCacheManagerPrxOneWay(int id) {
        Ice.ObjectPrx prx0 = getValidProxy(id, TIME_OUT);
        if (prx0 != null) {
            CommonFriendCacheManagerPrx prx = CommonFriendCacheManagerPrxHelper.uncheckedCast(prx0
                    .ice_timeout(TIME_OUT).ice_oneway());
            return prx;
        } else {
            return null;
        }
    }

    /**
     * 验证参数
     * 
     * @param uid
     * @param pid
     * @param begin
     * @param limit
     * @return
     */
    private boolean checkParam(int uid, com.xiaonei.xce.globalid.GlobalId gid, int begin, int limit) {
        if (uid < 0 || begin < 0 || limit < -1) {
            return false;
        }
        return true;
    }

    /**
     * 取userId好友中是GlobalId好友的人 请注意begin,limit的使用
     * 
     * @param userId
     * @param pageId
     * @param begin >=0
     * @param limit <br/>
     *        如果limit == -1,取区间为[begin, end) <br/>
     *        如果 limit >=0 取[begin, min{end,begin+limit})
     * @return
     */
    public List<Integer> getCommonFriend(int userId, com.xiaonei.xce.globalid.GlobalId gId,
            int begin, int limit) {
        if (!checkParam(userId, gId, begin, limit)) {
            return new ArrayList<Integer>();
        }

        MyUtil.GlobalId gid = new MyUtil.GlobalId(gId.getType(), gId.getId());
        List<Integer> commonFansList = new ArrayList<Integer>();
        CommonFriendCacheManagerPrx proxy = null;
        try {
            proxy = getCommonFriendCacheManagerPrxTwoWay(userId);
            int[] array = proxy.GetCommon(userId, gid, begin, limit);
            if (array != null && array.length != 0) {
                for (int i : array)
                    commonFansList.add(i);
            }
        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".getCommonFans " + userId
                            + " Ice.ConnectTimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".getCommonFans " + userId + " Ice.TimeoutException "
                            + proxy.ice_toString());
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return commonFansList;

    }

    /**
     * 取与gId有关系的userId个数
     * 
     * @param gId
     * 
     * @return
     */
    public int getCount(com.xiaonei.xce.globalid.GlobalId gId) {

        int count = 0;
        CommonFriendCacheManagerPrx proxy = null;
        MyUtil.GlobalId gid = new MyUtil.GlobalId(gId.getType(), gId.getId());

        try {
            proxy = getCommonFriendCacheManagerPrxTwoWay(0);
            count = proxy.getCount(gid);

        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".getCount " + gid.toString()
                            + " Ice.ConnectTimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".getCount " + gid.toString() + " Ice.TimeoutException "
                            + proxy.ice_toString());
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return count;
    }

    /**
     * 判断userId与gId是否有关系
     * 
     * @param userId
     * @param gId
     * 
     * @return
     */
    public Boolean hasRelation(int userId, com.xiaonei.xce.globalid.GlobalId gId) {

        boolean has = false;
        CommonFriendCacheManagerPrx proxy = null;
        MyUtil.GlobalId gid = new MyUtil.GlobalId(gId.getType(), gId.getId());

        try {
            proxy = getCommonFriendCacheManagerPrxTwoWay(0);
            has = proxy.hasRelation(gid, userId);

        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".hasRelation " + gid.toString()
                            + " Ice.ConnectTimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".hasRelation " + gid.toString()
                            + " Ice.TimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return has;
    }

    public void addRelation(int userId, com.xiaonei.xce.globalid.GlobalId gId) {

        CommonFriendCacheManagerPrx proxy = null;
        MyUtil.GlobalId gid = new MyUtil.GlobalId(gId.getType(), gId.getId());

        try {
            proxy = getCommonFriendCacheManagerPrxTwoWay(0);
            proxy.add(gid, userId);

        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".addRelation " + gid.toString() + " " + userId
                            + " Ice.ConnectTimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".addRelation " + gid.toString() + " " + userId
                            + " Ice.TimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void removeRelation(int userId, com.xiaonei.xce.globalid.GlobalId gId) {

        CommonFriendCacheManagerPrx proxy = null;
        MyUtil.GlobalId gid = new MyUtil.GlobalId(gId.getType(), gId.getId());

        try {
            proxy = getCommonFriendCacheManagerPrxTwoWay(0);
            proxy.remove(gid, userId);

        } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".removeRelation " + gid.toString() + " " + userId
                            + " Ice.ConnectTimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    getClass().getName() + ".removeRelation " + gid.toString() + " " + userId
                            + " Ice.TimeoutException " + proxy.ice_toString());
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {

        com.xiaonei.xce.globalid.GlobalId x = com.xiaonei.xce.globalid.GlobalId.fromGame(9102001);

        System.out.println(CommonFriendCacheAdapter.getInstance().getCount(x));
    }
}
