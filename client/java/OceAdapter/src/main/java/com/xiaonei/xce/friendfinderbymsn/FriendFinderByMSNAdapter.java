package com.xiaonei.xce.friendfinderbymsn;

import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;
import java.util.Map.Entry;

import Ice.ObjectPrx;
import xce.friendfinder.MSNdata;
import xce.friendfinder.FriendFinderByMSNManagerPrx;
import xce.friendfinder.FriendFinderByMSNManagerPrxHelper;
import xce.util.service.ReplicatedServiceAdapter;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class FriendFinderByMSNAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFriendFinderByMSN";
	private static int _interval = 120;	
	
	private FriendFinderByMSNAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static FriendFinderByMSNAdapter _instance = new FriendFinderByMSNAdapter();
	public static FriendFinderByMSNAdapter getInstance() {
		return _instance;
	}

	public MSNData getMSNdata(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);
		FriendFinderByMSNManagerPrx prx = null;
                MSNData result = new MSNData();

                MSNdata ret = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId); 
			ret = prx.getMSNdata(userId);
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getMSNdata timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                if (ret == null){
                        return result;
		}

                result.setBase(ret);

                return result;
	}

	public Map<Integer,List<Integer> > getPUAmap(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                Map<Integer,List<Integer> > result = new HashMap<Integer,List<Integer> >();
                Map<Integer,int[] > ret = new HashMap<Integer,int[] >();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId); 
			ret = prx.getPUAmap(userId);
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUAmap timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

		for( Entry<Integer, int[]> one : ret.entrySet()){
			int key = one.getKey();
			List<Integer> value = new ArrayList<Integer>();
			for( int two : one.getValue() ){
				value.add(two);
			}
			result.put(key, value);
		}

                return result;
	}

	public List<Integer> getPUAids(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                List<Integer> result = new ArrayList<Integer>();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId);
			int[] ret = prx.getPUAids(userId);
			for( int one: ret ){
				result.add( one );
			}
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUAids timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public List<Integer> getPUAidsWithMask(int userId,int[] mask) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                List<Integer> result = new ArrayList<Integer>();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId);
			int[] ret = prx.getPUAidsWithMask(userId,mask);
			for( int one: ret ){
				result.add( one );
			}
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUAidsWithMask timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public Map<Integer,List<Integer> > getPUKmap(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                Map<Integer,List<Integer> > result = new HashMap<Integer,List<Integer> >();
                Map<Integer,int[] > ret = new HashMap<Integer,int[] >();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId); 
			ret = prx.getPUKmap(userId);
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUKmap timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

		for( Entry<Integer, int[]> one : ret.entrySet()){
			int key = one.getKey();
			List<Integer> value = new ArrayList<Integer>();
			for( int two : one.getValue() ){
				value.add(two);
				Oce.getLogger().info(two);
			}
			result.put(key, value);
		}

                return result;
	}

	public List<Integer> getPUKids(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                List<Integer> result = new ArrayList<Integer>();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId);
			int[] ret = prx.getPUKids(userId);
			for( int one: ret ){
				result.add( one );
			}

                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUKids timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public List<Integer> getPUKidsRandom(int userId,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                List<Integer> result = new ArrayList<Integer>();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId);
			int[] ret = prx.getPUKidsRandom(userId,limit);
			for( int one: ret ){
				result.add( one );
			}

                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUKidsRandom timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public List<Integer> getPUKidsWithMask(int userId,int[] mask) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByMSN "
				+ userId);

                List<Integer> result = new ArrayList<Integer>();
                FriendFinderByMSNManagerPrx prx = null;
                try {
                	prx = getFriendFinderByMSNManagerTwoway(userId);
			int[] ret = prx.getPUKidsWithMask(userId,mask);
			for( int one: ret ){
				result.add( one );
			}

                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByMSN getPUKidsWithMask timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public void add(int hostId, int guestId, int type ){
		getFriendFinderByMSNManagerOneway(hostId).add(hostId,guestId,type);
	}

	public void removePUA(int hostId, int guestId ){
		getFriendFinderByMSNManagerOneway(hostId).removePUA(hostId,guestId);
	}
	public void removePUK(int hostId, int guestId ){
		getFriendFinderByMSNManagerOneway(hostId).removePUK(hostId,guestId);
	}

	public void reload(int userId) {
		getFriendFinderByMSNManagerOneway(userId).reload(userId);
	}

	protected FriendFinderByMSNManagerPrx getFriendFinderByMSNManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderByMSNManagerPrx prx = FriendFinderByMSNManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected FriendFinderByMSNManagerPrx getFriendFinderByMSNManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderByMSNManagerPrx prx = FriendFinderByMSNManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

}

