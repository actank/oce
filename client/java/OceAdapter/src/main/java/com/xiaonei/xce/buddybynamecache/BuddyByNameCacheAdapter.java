package com.xiaonei.xce.buddybynamecache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import Ice.ObjectPrx;
import xce.buddy.BuddyByNameCacheManagerPrx;
import xce.buddy.BuddyByNameCacheManagerPrxHelper;
import xce.buddy.BuddyByNameLoaderPrx;
import xce.buddy.BuddyByNameLoaderPrxHelper;
import xce.util.service.ReplicatedServiceAdapter;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.buddybynamecache.BuddyByNameCacheAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;


import xce.clusterstate.ReplicatedClusterAdapter;

public class BuddyByNameCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByNameCache";
	private static int _interval = 120;	
	
	private BuddyByNameCacheAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static BuddyByNameCacheAdapter _instance = new BuddyByNameCacheAdapter();
	public static BuddyByNameCacheAdapter getInstance() {
		return _instance;
	}

	public int getFriendCount(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByNameCache "
				+ userId);
		String prxstr = "NULL Prx";
		try {
			BuddyByNameCacheManagerPrx prx = getBuddyByNameCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			return prx.getFriendCount(userId);
		} catch (Ice.ConnectTimeoutException e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getFriendCount " + userId + " Ice.ConnectTimeoutException " + prxstr);
                        stat.endException(e);
                } catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getFriendCount " + userId + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error( this.getClass().getName() + ".getFriendCount " + userId + " " + prxstr, e);
                        stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return 0;
	}
	
	public Map<Integer,Integer> getFriendCounts(List<Integer> userIds) {
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}

		/* collect results */
		Map<Integer,Integer> allResults = new HashMap<Integer,Integer>();
		if (userIds.isEmpty()){
			return allResults;
		}
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"BuddyByNameCache getFriendCounts start:" + userIds.get(0) + " size:"
				+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if ( subUserIds.isEmpty() ){
				continue;
			}
			try {
				Map<Integer, Integer> subTargets = getBuddyByNameCacheManagerTwoway(i)
					.getFriendCounts(MathUtil.list2array(subUserIds));
				allResults.putAll( subTargets );
			} catch (TimeoutException e) {
				cost.endStep("SubBatch start:" + subUserIds.get(0)
						+ " size:" + subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}
	
	public List<Integer> getFriendList(int userId,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByNameCache "
				+ userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByNameCacheManagerPrx prx = getBuddyByNameCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendList(userId,limit);
                        for( int one: ret ){
                                result.add( one );
                        }
		} catch (Ice.ConnectTimeoutException e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getFriendList " + userId + " Ice.ConnectTimeoutException " + prxstr);
                        stat.endException(e);
                } catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getFriendList " + userId + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getFriendList " + userId + " " + prxstr, e);
                        stat.endException(e);
                } finally {
			stat.endFinally();
		}
		return result;
	}

	public List<Integer> getFriendList(int userId,int begin, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByNameCache "
				+ userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByNameCacheManagerPrx prx = getBuddyByNameCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendListN(userId,begin,limit);
                        for( int one: ret ){
                                result.add( one );
                        }
		} catch (Ice.ConnectTimeoutException e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getFriendList " + userId + " Ice.ConnectTimeoutException " + prxstr);
                        stat.endException(e);
                } catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getFriendList " + userId + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
                        Oce.getLogger().error( this.getClass().getName() + ".getFriendList " + userId + " " + prxstr, e);
                        stat.endException(e);
                } finally {
			stat.endFinally();
		}
		return result;
	}
	
	public Map<Integer,List<Integer> > getFriendLists(List<Integer> userIds) {
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}

		/* collect results */
		Map<Integer,List<Integer> > allResults = new HashMap<Integer,List<Integer> >();
		if (userIds.isEmpty()){
			return allResults;
		}
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"BuddyByNameCache getFriendLists start:" + userIds.get(0) + " size:"
				+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if ( subUserIds.isEmpty() ){
				continue;
			}
			try {
				Map<Integer,int[] > subTargets = getBuddyByNameCacheManagerTwoway(i)
					.getFriendLists(MathUtil.list2array(subUserIds));
				for( Entry<Integer, int[]> one : subTargets.entrySet()){
					int key = one.getKey();
					List<Integer> value = new ArrayList<Integer>();
					for( int two : one.getValue() ){
						value.add(two);
					}
					allResults.put(key, value);
				}
			} catch (TimeoutException e) {
				cost.endStep("SubBatch start:" + subUserIds.get(0)
						+ " size:" + subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	public void reload(int userId) {
		BuddyByNameLoaderAdapter.getInstance().reload(userId);
	}

	protected BuddyByNameCacheManagerPrx getBuddyByNameCacheManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			BuddyByNameCacheManagerPrx prx = BuddyByNameCacheManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected BuddyByNameCacheManagerPrx getBuddyByNameCacheManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			BuddyByNameCacheManagerPrx prx = BuddyByNameCacheManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		int userId1 = 24496255;
		int userId2 = 68126;
		List<Integer> batch = new ArrayList<Integer>();
		batch.add(userId1);
		batch.add(userId2);
		List<Integer> listRes1 = BuddyByNameCacheAdapter.getInstance().getFriendList(userId1,-1);
		List<Integer> listRes2 = BuddyByNameCacheAdapter.getInstance().getFriendList(userId2,-1);
		Map<Integer, List<Integer>> listResBatch = BuddyByNameCacheAdapter.getInstance().getFriendLists( batch );
		int countRes1 = BuddyByNameCacheAdapter.getInstance().getFriendCount(userId1);
		int countRes2 = BuddyByNameCacheAdapter.getInstance().getFriendCount(userId2);
		Map<Integer, Integer> countResBatch = BuddyByNameCacheAdapter.getInstance().getFriendCounts( batch );
		System.out.println( "--Single:");
		System.out.println( userId1 + ".list.size() = " + listRes1.size() );
		System.out.println( userId2 + ".list.size() = " + listRes2.size() );
		System.out.println( userId1 + ".count = " + countRes1 );
		System.out.println( userId2 + ".count = " + countRes2 );
		System.out.println( "--Batch:");
		for ( Entry<Integer,List<Integer>> one : listResBatch.entrySet() ) {
			System.err.println( one.getKey() + ".list.size() = " + one.getValue().size() );
		}
		for ( Entry<Integer,Integer> one : countResBatch.entrySet() ) {
			System.err.println( one.getKey() + ".count = " + one.getValue() );
		}
		System.exit(0);
		/*
		*/
	}
}

class BuddyByNameLoaderAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByNameLoader";
	private static int _interval = 120;	
	
	private BuddyByNameLoaderAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static BuddyByNameLoaderAdapter _instance = new BuddyByNameLoaderAdapter();
	public static BuddyByNameLoaderAdapter getInstance() {
		return _instance;
	}

	public void reload(int userId) {
		try {
			getBuddyByNameLoaderOneway(userId).reload(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("BuddyCacheByNameLoader timeout ");
		}
	}

	protected BuddyByNameLoaderPrx getBuddyByNameLoaderOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			BuddyByNameLoaderPrx prx = BuddyByNameLoaderPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
