package com.xiaonei.xce.userapicache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.userapicache.UserApiCacheData1;
import xce.userapicache.UserApiCacheManagerPrx;
import xce.userapicache.UserApiCacheManagerPrxHelper;
import xce.util.tools.MathUtil;

public class IceUserApiCacheAdapter extends
	ReplicatedClusterAdapter implements UserApiCacheInvoker {
    private static final String ctr_endpoints = "ControllerUserApiCache";
    private static int _interval = 120;

    public IceUserApiCacheAdapter() {
	super(ctr_endpoints, _interval);
    }

    @Override
    public Map<Integer, UserApiCache> getUserApiCacheList(List<Integer> userIds) {
	TimeCost cost = TimeCost
		.begin("UserApiCacheAdapter.getUserApiCacheList");
	try {
	    Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
	    int cluster = getCluster();
	    for (int i = 0; i < cluster; ++i) {
		splitTargets.put(i, new ArrayList<Integer>());
	    }
	    for (int i : userIds) {
		if (i > 0) {
		    splitTargets.get(i % cluster).add(i);
		}
	    }
	    Map<java.lang.Integer, UserApiCache> result = new HashMap<java.lang.Integer, UserApiCache>();
	    if (userIds.isEmpty()) {
		return result;
	    }
	    for (int i = 0; i < cluster; ++i) {
		List<Integer> subUserIds = splitTargets.get(i);
		if (subUserIds.isEmpty()) {
		    continue;
		}
		UserApiCacheManagerPrx prx = null;
		try {
		    prx = getUserApiCacheManager(i);
		    Map<java.lang.Integer, UserApiCacheData1> subResult = prx
			    .getUserApiCache1List(MathUtil
				    .list2array(subUserIds));
		    for (Entry<Integer, UserApiCacheData1> entry : subResult
			    .entrySet()) {
			UserApiCache o = new UserApiCache();
			o.setBase(entry.getValue());
			result.put(entry.getKey(), o);
		    }
		} catch (TimeoutException e) {
		    Oce.getLogger().error(
			    this.getClass().getName()
				    + ".getUserApiCacheList [ " + prx
				    + " ]  idSize=" + subUserIds.size()
				    + " Ice.TimeoutException");
		    throw e;
		}
	    }
	    return result;
	} finally {
	    cost.endFinally();
	}
    }

    protected UserApiCacheManagerPrx getUserApiCacheManager(int userId) {
	Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
	if (prx0 != null) {
	    UserApiCacheManagerPrx prx = UserApiCacheManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
	    return prx;
	} else {
	    return null;
	}
    }

    protected UserApiCacheManagerPrx getUserApiCacheManagerOneway(int userId) {

	Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
	if (prx0 != null) {
	    UserApiCacheManagerPrx prx = UserApiCacheManagerPrxHelper
		    .uncheckedCast(prx0.ice_timeout(300).ice_oneway());
	    return prx;
	} else {
	    return null;
	}
    }

}
