package com.xiaonei.xce.buddyadapter.buddyhotcache;

import java.util.ArrayList;
import java.util.List;

import xce.buddy.BuddyHotCacheManagerPrx;
import xce.buddy.BuddyHotCacheManagerPrxHelper;
import xce.buddy.HotData;
import xce.clusterstate.ReplicatedClusterAdapter;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

/**
 * BuddyHotCacheAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public final class BuddyHotCacheAdapter extends ReplicatedClusterAdapter{

	private static BuddyHotCacheAdapter _adapter = new BuddyHotCacheAdapter();

	private static final String ctr_endpoints = "ControllerBuddyHotCache";

	private static int _interval = 120;

	protected BuddyHotCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	/**
	 * @return
	 */
	public static BuddyHotCacheAdapter getInstance() {
		return _adapter;
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyHotCacheManagerPrx getBuddyHotCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyHotCacheManagerPrx prx = BuddyHotCacheManagerPrxHelper
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
	protected BuddyHotCacheManagerPrx getBuddyHotCacheManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyHotCacheManagerPrx prx = BuddyHotCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	static enum WriteType{
		AddHotFriend,
		DelHotFriend
	} 
	
	/**
	 * @param host
	 * @param data
	 * @param type
	 * @param method
	 */
	private void write(int host, HotFriendData data, WriteType type, String method){
		TimeCost cost = TimeCost.begin("BuddyHotCacheAdapter." + method+" id: "+ host);
		BuddyHotCacheManagerPrx prx = null ;
		HotData hdata = new HotData();
		hdata.hotfriend = data.getHotFriend();
		hdata.timestamp = data.getTimestamp();
		hdata.memo = data.getMemo();
		hdata.position = data.getPosition();
		try {
			if (host <= 0) {
				return;
			}
			prx = getBuddyHotCacheManager(host);
			switch(type){
			case AddHotFriend:
				prx.addHotFriend(host,hdata);
				break;
			case DelHotFriend:
				prx.deleteHotFriend(host,hdata);
				break;
			default:
				break;
			}
		} catch (ConnectTimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + "." + method + " [ " + prx + "] id=" + host + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + "." + method + " [ " + prx + "] id=" + host + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
	}
	
	/**
	 * @param host
	 * @param data
	 */
	public void addHotFriend(int host, HotFriendData data){
		this.write(host, data, WriteType.AddHotFriend, "addHotFriend");
	}

	/**
	 * @param host
	 * @param data
	 */
	public void deleteHotFriend(int host, HotFriendData data){
		this.write(host, data, WriteType.DelHotFriend, "deleteHotFriend");
	}
	
	/**
	 * @param host
	 * @param datalist
	 */
	public void setHotFriend(int host, List<HotFriendData> datalist){
		TimeCost cost = TimeCost.begin("BuddyHotCacheAdapter.setHotFriend id: "+ host);
		BuddyHotCacheManagerPrx prx = null ;
		List<HotData> hdatalist = new ArrayList<HotData>();
		for (HotFriendData data : datalist) {
			HotData hdata = new HotData();
			hdata.hotfriend = data.getHotFriend();
			hdata.timestamp = data.getTimestamp();
			hdata.memo = data.getMemo();
			hdata.position = data.getPosition();
			hdatalist.add(hdata);
		}
		try {
			if (host <= 0) {
				return;
			}
			prx = getBuddyHotCacheManager(host);
			prx.setHotFriend(host, (HotData[]) hdatalist.toArray());
		} catch (ConnectTimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setHotFriend [ " + prx + "] id=" + host + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setHotFriend [ " + prx + "] id=" + host + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
	}
	
	/**
	 * @param host
	 * @return
	 */
	public List<Integer> getHotFriendList(int host){
		TimeCost cost = TimeCost.begin("BuddyHotCacheAdapter.getHotFriendList id: "+ host);
		BuddyHotCacheManagerPrx prx = null ;
		List<Integer> result = new ArrayList<Integer>();;
		try {
			prx = getBuddyHotCacheManager(host);
			int[] ret = prx.getHotFriendList(host);
			for (int one : ret) {
				result.add(one);
			}
		} catch (ConnectTimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getHotFriendList [ " + prx + "] id=" + host + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getHotFriendList [ " + prx + "] id=" + host + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
		return result;
	}
	
	/**
	 * @param host
	 * @return
	 */
	public int getHotFriendCount(int host){
		TimeCost cost = TimeCost.begin("BuddyHotCacheAdapter.getHotFriendCount id: "+ host);
		BuddyHotCacheManagerPrx prx = null ;
		int count = 0;
		try {
			prx = getBuddyHotCacheManager(host);
			count = prx.getHotFriendCount(host);		
		} catch (ConnectTimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getHotFriendCount [ " + prx + "] id=" + host + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getHotFriendCount [ " + prx + "] id=" + host + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
		return count;
	}
	
	
	/**
	 * @param host
	 * @return
	 */
	public List<HotFriendData> getHotFriendListData(int host){
		TimeCost cost = TimeCost.begin("BuddyHotCacheAdapter.getHotFriendListData id: "+ host);
		BuddyHotCacheManagerPrx prx = null ;
		List<HotFriendData> result = new ArrayList<HotFriendData>();;
		try {
			prx = getBuddyHotCacheManager(host);
			HotData[] dataset= prx.getHotFriendListData(host);
			for (HotData one : dataset) {
				result.add(new HotFriendData(one));
			}
		} catch (ConnectTimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getHotFriendListData [ " + prx + "] id=" + host + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getHotFriendListData [ " + prx + "] id=" + host + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
		return result;
	}
	
	/**
	 * @param host
	 */
	public void reload(int host) {
		TimeCost cost = TimeCost.begin("BuddyHotCacheAdapter.reload id: "+host);
		BuddyHotCacheManagerPrx prx = null ;
		try {
			if (host <= 0) {
				return;
			}
			prx = getBuddyHotCacheManager(host);
			prx.reload(host);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".reload [ " + prx + "] id=" + host + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
	}
}
