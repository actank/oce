package com.xiaonei.xce.buddyadapter.buddycount;

import xce.buddy.BuddyCountManagerPrx;
import xce.buddy.BuddyCountManagerPrxHelper;
import xce.buddy.BuddyCounter;
import xce.clusterstate.ReplicatedClusterAdapter;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

/**
 * BuddyCountAdapter 仅仅提供给好友系统
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public final class BuddyCountAdapter extends ReplicatedClusterAdapter {

    private static BuddyCountAdapter _adapter = new BuddyCountAdapter();

    private static final String ctr_endpoints = "ControllerBuddyCount";

    private static int _interval = 120;

    protected BuddyCountAdapter() {
	super(ctr_endpoints, _interval);
    }

    /**
     * @return
     */
    public static BuddyCountAdapter getInstance() {
	return _adapter;
    }

    /**
     * @param userId
     * @return
     */
    protected BuddyCountManagerPrx getBuddyCountManager(int userId) {
	Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
	if (prx0 != null) {
	    BuddyCountManagerPrx prx = BuddyCountManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
	    return prx;
	} else {
	    return null;
	}
    }

    /**
     * @param userId
     * @return
     */
    protected BuddyCountManagerPrx getBuddyCountManagerOneway(int userId) {
	Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
	if (prx0 != null) {
	    BuddyCountManagerPrx prx = BuddyCountManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(300).ice_oneway());
	    return prx;
	} else {
	    return null;
	}
    }

    /**
     * @param userId
     * @param type
     * @param step
     */
    public void inc(int userId, int type, int step) {
	TimeCost cost = TimeCost.begin("BuddyCountAdapter.inc id: " + userId
		+ " type: " + type + " step: " + step);
	try {
	    if (userId <= 0) {
		return;
	    }
	    getBuddyCountManagerOneway(userId).inc(userId, type, step);
	} finally {
	    cost.endFinally();
	}
    }

    /**
     * @param userId
     * @param type
     * @param step
     */
    public void dec(int userId, int type, int step) {
	TimeCost cost = TimeCost.begin("BuddyCountAdapter.dec id: " + userId
		+ " type: " + type + " step: " + step);
	try {
	    if (userId <= 0) {
		return;
	    }
	    getBuddyCountManagerOneway(userId).dec(userId, type, step);
	} finally {
	    cost.endFinally();
	}
    }

    /**
     * @param userId
     * @param type
     * @param step
     */
    public void set(int userId, int type, int step) {
	TimeCost cost = TimeCost.begin("BuddyCountAdapter.set id: " + userId
		+ " type: " + type + " step: " + step);
	try {
	    if (userId <= 0) {
		return;
	    }
	    getBuddyCountManagerOneway(userId).set(userId, type, step);
	} finally {
	    cost.endFinally();
	}
    }

    /**
     * @param userId
     * @param type
     * @return
     */
    public int get(int userId, int type) {
	TimeCost cost = TimeCost.begin("BuddyCountAdapter.get id: " + userId
		+ " type: " + type);
	BuddyCountManagerPrx prx = null;
	try {
	    if (userId <= 0) {
		return 0;
	    }
	    int ret = 0;
	    try {
		prx = getBuddyCountManager(userId);
		ret = prx.get(userId, type);
	    } catch (TimeoutException e) {
		Oce.getLogger().error(
			this.getClass().getName() + ".get [ " + prx + "] id="
				+ userId + " type=" + type
				+ " Ice.TimeoutException");
	    } catch (Throwable e) {
		Oce.getLogger().error(
			this.getClass().getName() + ".get " + " id=" + userId
				+ " type=" + type, e);
	    }
	    return ret;
	} finally {
	    cost.endFinally();
	}
    }

    /**
     * @param userId
     * @return
     */
    public BuddyCountData getAll(int userId) {
	TimeCost cost = TimeCost
		.begin("BuddyCountAdapter.getAll id: " + userId);
	BuddyCountManagerPrx prx = null;
	try {
	    if (userId <= 0) {
		return new BuddyCountData();
	    }
	    try {
		prx = getBuddyCountManager(userId);
		BuddyCounter counter = prx.getAll(userId);
		return new BuddyCountData(counter);
	    } catch (TimeoutException e) {
		Oce.getLogger().error(
			this.getClass().getName() + ".getAll [ " + prx
				+ "] id=" + userId + " Ice.TimeoutException");
	    } catch (Throwable e) {
		Oce.getLogger().error(
			this.getClass().getName() + ".getAll " + " id="
				+ userId, e);
	    }
	    return new BuddyCountData();
	} finally {
	    cost.endFinally();
	}
    }

    /**
     * @param userId
     */
    public void reload(int userId) {
	TimeCost cost = TimeCost
		.begin("BuddyCountAdapter.reload id: " + userId);
	BuddyCountManagerPrx prx = null;
	try {
	    if (userId <= 0) {
		return;
	    }
	    prx = getBuddyCountManager(userId);
	    prx.reload(userId);
	} catch (TimeoutException e) {
	    Oce.getLogger().error(
		    this.getClass().getName() + ".reload [ " + prx + "] id="
			    + userId + " Ice.TimeoutException");
	} finally {
	    cost.endFinally();
	}
    }
}
