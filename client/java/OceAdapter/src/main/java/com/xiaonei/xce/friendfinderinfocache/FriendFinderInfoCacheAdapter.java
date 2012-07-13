package com.xiaonei.xce.friendfinderinfocache;

import java.util.ArrayList;
import java.util.List;

import xce.friendfinder.FriendFinderInfoCachePrx;
import xce.friendfinder.FriendFinderInfoCachePrxHelper;
import xce.friendfinder.HighSchoolInfo;
import xce.friendfinder.NetworkInfo;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class FriendFinderInfoCacheAdapter extends ReplicatedClusterAdapter {

    private static final String ctr_endpoints = "ControllerFriendFinderInfoCache";

    private static int _interval = 120;

    private FriendFinderInfoCacheAdapter() {
        super(ctr_endpoints, _interval);
    }

    private static FriendFinderInfoCacheAdapter _instance = new FriendFinderInfoCacheAdapter();

    public static FriendFinderInfoCacheAdapter getInstance() {
        return _instance;
    }

    public List<Integer> getFriendListByInfo(int userId, int limit) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache " + userId
                + " limit=" + limit);
        List<Integer> result = new ArrayList<Integer>();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(userId);
            int[] ret = prx.getFriendListByInfo(userId, limit);
            for (int one : ret) {
                result.add(one);
            }
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getFriendListByInfo timeout " + userId + " limit="
                            + limit + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    public List<Integer> getInfoMatchFriendList(int userId, int limit) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache " + userId
                + " limit=" + limit);
        List<Integer> result = new ArrayList<Integer>();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(userId);
            int[] ret = prx.getInfoMatchFriendList(userId, limit);
            for (int one : ret) {
                result.add(one);
            }
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getInfoMatchFriendList timeout " + userId + " limit="
                            + limit + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    public List<Integer> getFriendListByGuestInfo(int hostId, int guestId, int limit) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache hostId="
                + hostId + " guestId=" + guestId + " limit=" + limit);
        List<Integer> result = new ArrayList<Integer>();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(guestId);
            int[] ret = prx.getFriendListByGuestInfo(hostId, guestId, limit);
            for (int one : ret) {
                result.add(one);
            }
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getFriendListByGuestInfo timeout hostId=" + hostId
                            + " guestId=" + guestId + " limit=" + limit + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    public List<Integer> getGuestInfoMatchFriendList(int hostId, int guestId, int limit) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache hostId="
                + hostId + " guestId=" + guestId + " limit=" + limit);
        List<Integer> result = new ArrayList<Integer>();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(guestId);
            int[] ret = prx.getGuestInfoMatchFriendList(hostId, guestId, limit);
            for (int one : ret) {
                result.add(one);
            }
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getGuestInfoMatchFriendList timeout hostId=" + hostId
                            + " guestId=" + guestId + " limit=" + limit + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    public List<Integer> getGuestInfoMatchFriendListBatch(int[] hostIds, int guestId) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache hostIds.length="
                + hostIds.length + " guestId=" + guestId);
        List<Integer> result = new ArrayList<Integer>();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(guestId);
            int[] ret = prx.getGuestInfoMatchFriendListBatch(hostIds, guestId);
            for (int one : ret) {
                result.add(one);
            }
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getGuestInfoMatchFriendListBatch timeout hostIds.length="
                            + hostIds.length + " guestId=" + guestId + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    public List<Integer> getCommonFriendListByGuestInfo(int userId, int minsharecount, int limit) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache userId="
                + userId + " minsharecount=" + minsharecount + " limit=" + limit);
        List<Integer> result = new ArrayList<Integer>();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(userId);
            int[] ret = prx.getCommonFriendListByGuestInfo(userId, minsharecount, limit);
            for (int one : ret) {
                result.add(one);
            }
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getCommonFriendListByGuestInfo timeout userId=" + userId
                            + " minsharecount=" + minsharecount + " limit=" + limit + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    public HighSchoolInfo getHighSchoolInfo(int userId) {
        return getFriendFinderInfoCacheTwoway(userId).getHighSchoolInfo(userId);
    }

    @Deprecated
    public NetworkInfo getNetworkInfoWithHostage(int userId) {
        TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderInfoCache " + userId);
        NetworkInfo result = new NetworkInfo();
        FriendFinderInfoCachePrx prx = null;
        try {
            prx = getFriendFinderInfoCacheTwoway(userId);
            result = prx.getNetworkInfoWithHostage(userId);
        } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(
                    "FriendFinderInfoCache getNetworkInfoWithHostage timeout" + userId + " " + prx);
            stat.endException(e);
        } finally {
            stat.endFinally();
        }
        return result;
    }

    protected FriendFinderInfoCachePrx getFriendFinderInfoCacheTwoway(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
            FriendFinderInfoCachePrx prx = FriendFinderInfoCachePrxHelper.uncheckedCast(prx0
                    .ice_timeout(300).ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

    protected FriendFinderInfoCachePrx getFriendFinderInfoCacheOneway(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
            FriendFinderInfoCachePrx prx = FriendFinderInfoCachePrxHelper.uncheckedCast(prx0
                    .ice_timeout(300).ice_oneway());
            return prx;
        } else {
            return null;
        }
    }

    //    public static void main(String[] args) {
    //                int userId = 238111132;
    //                NetworkInfo info = FriendFinderInfoCacheAdapter.getInstance().getNetworkInfoWithHostage(
    //                        userId);
    //                System.out.println(info.univInfo.length);
    //                System.out.println(info.highschoolInfo.length);
    //                System.out.println(info.juniorschoolInfo.length);
    //                System.out.println(info.elementaryschoolInfo.length);
    //        
    //                HighSchoolInfo tmp = FriendFinderInfoCacheAdapter.getInstance().getHighSchoolInfo(userId);
    //                System.out.println("tmp highschool : " + tmp.highschoolseq.length);
    //                System.out.println("tmp junior : " + tmp.juniorschoolseq.length);
    //                System.out.println("tmp college : " + tmp.collegeseq.length);
    //                System.exit(0);
    //    }
}
