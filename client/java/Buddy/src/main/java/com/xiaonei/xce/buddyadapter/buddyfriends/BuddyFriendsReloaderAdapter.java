package com.xiaonei.xce.buddyadapter.buddyfriends;

import xce.buddy.BuddyFriendsLoaderPrx;
import xce.buddy.BuddyFriendsLoaderPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

/**
 * @author yuyang
 * 
 */
public class BuddyFriendsReloaderAdapter extends ReplicatedClusterAdapter {

    private static final String ctr_endpoints = "ControllerBuddyFriendsLoader";

    private static int _interval = 120;

    private BuddyFriendsReloaderAdapter() {
        super(ctr_endpoints, _interval);
    }

    private static BuddyFriendsReloaderAdapter _instance = new BuddyFriendsReloaderAdapter();

    /**
     * @return
     */
    public static BuddyFriendsReloaderAdapter getInstance() {
        return _instance;
    }

    void reload(int userId) {
        getBuddyFriendsLoaderOneway(userId).reload(userId);
    }

    /**
     * @param userId
     * @return
     */
    protected BuddyFriendsLoaderPrx getBuddyFriendsLoaderTwoway(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
            BuddyFriendsLoaderPrx prx = BuddyFriendsLoaderPrxHelper.uncheckedCast(prx0.ice_timeout(
                    300).ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

    /**
     * @param userId
     * @return
     */
    protected BuddyFriendsLoaderPrx getBuddyFriendsLoaderOneway(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
            BuddyFriendsLoaderPrx prx = BuddyFriendsLoaderPrxHelper
                    .uncheckedCast(prx0.ice_oneway());
            return prx;
        } else {
            return null;
        }
    }
}
