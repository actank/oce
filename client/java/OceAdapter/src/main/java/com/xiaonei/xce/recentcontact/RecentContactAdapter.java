package com.xiaonei.xce.recentcontact;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import xce.clusterstate.ReplicatedClusterAdapter;

import xce.recentcontact.ContactInfo;
import xce.recentcontact.ContactType;
import xce.recentcontact.RecentContactManagerPrx;
import xce.recentcontact.RecentContactManagerPrxHelper;

public class RecentContactAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerRecentContact";
	private static int _interval = 120;
	
	private static RecentContactAdapter _instance = new RecentContactAdapter();
	
	private RecentContactAdapter(){
		super(ctr_endpoints,_interval);
	}
	
	public static RecentContactAdapter getInstance() {
		return _instance;
	}
	
	protected RecentContactManagerPrx getRecentContactManager(int userId) {
		
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			RecentContactManagerPrx prx = RecentContactManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}	
	}
	
	protected RecentContactManagerPrx getRecentContactOnewayManager(int userId) {
		
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			RecentContactManagerPrx prx = RecentContactManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}	
	}

	public ContactInfo[] get(int userId, ContactType type, int begin, int limit){
		ContactInfo[] result = null;
		RecentContactManagerPrx prx= null;
		try {
	//		TimeCost cost = TimeCost.begin("get[" + userId + "]");
			prx=getRecentContactManager(userId);
			result = prx.get(userId, type, begin, limit);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".get [ " + prx + " ] id="  + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".get [ " + prx + " ] id="  + userId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".get " + userId, e);
		}	
		return result;
	}
	
	public void add(int userId, int toId, ContactType type){
		ContactInfo contactInfo = new ContactInfo();
		contactInfo.ownerId=userId;
		contactInfo.guestId=toId;
		ContactType t=type;
		contactInfo.type= t;
		
		ContactInfo[] contactInfos={contactInfo};
		addObj(userId,contactInfos);
	}
	
	public void addObj(int userId, ContactInfo[] contactInfos){
		RecentContactManagerPrx prx= null;
		try {
	//		TimeCost cost = TimeCost.begin("addObj[" + userId + "]");
			prx=getRecentContactManager(userId);
			prx.add(contactInfos);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addObj [ " + prx + " ] id="  + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".addObj [ " + prx + " ] id="  + userId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".addObj " + userId, e);
		}		
	}
	
	public void remove(int userId, int toRemove){
		RecentContactManagerPrx prx= null;
		try {
	//		TimeCost cost = TimeCost.begin("remove[" + userId + "]");
			getRecentContactOnewayManager(userId).remove(userId, toRemove);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".remove " + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".remove " + userId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".remove " + userId, e);
		}
	}
	
	public static void main(String[] args) {

		RecentContactAdapter test = RecentContactAdapter.getInstance();

		int ownerId = 412425;
		int guestId = 142;
		ContactType type = ContactType.XNMESSAGE;
		test.add(ownerId, guestId, type);

//		for(int i=0;i<10;i++){
//			test.add(ownerId, guestId+i, type);
//		}
		
		
		ContactInfo[] result = test.get(ownerId, type, 0, 10);
		System.out.println(result.length);
		for(int i=0; i<result.length;i++){
			System.out.print(result[i].ownerId+"	");
			System.out.print(result[i].guestId+"		");
			System.out.print(result[i].time+"	");
			System.out.println(result[i].type);	
		}
		
//		test.remove(ownerId, guestId);
//		
//		System.out.println();
//		ContactInfo[] result1 = test.get(ownerId, type, 0, 10);
//		System.out.println(result1.length);
//		for(int i=0; i<result1.length;i++){
//			System.out.print(result[i].ownerId+"	");
//			System.out.print(result[i].guestId+"		");
//			System.out.print(result[i].time+"	");
//			System.out.println(result[i].type);
//		}

		System.exit(0);
	}	
}
