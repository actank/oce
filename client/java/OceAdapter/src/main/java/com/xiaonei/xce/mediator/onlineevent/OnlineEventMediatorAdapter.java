package com.xiaonei.xce.mediator.onlineevent;
import xce.mediator.onlineevent.OnlineEventMediatorManagerPrx;
import xce.mediator.onlineevent.OnlineEventMediatorManagerPrxHelper;
import java.util.Map;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import xce.clusterstate.ReplicatedClusterAdapter;



public class OnlineEventMediatorAdapter extends ReplicatedClusterAdapter {
	private static OnlineEventMediatorAdapter _instance = new OnlineEventMediatorAdapter();

	private static final String ctr_endpoints = "ControllerOnlineEventMediator";

	private static int _interval = 120;
	
	protected OnlineEventMediatorAdapter() {
		super(ctr_endpoints, _interval);
	}

	public static OnlineEventMediatorAdapter getInstance() {
		return _instance;
	}

	protected OnlineEventMediatorManagerPrx getOnlineEventMediatorManager() {
		Ice.ObjectPrx prx0 = getValidProxy(0, 300);
		if (prx0 != null) {
			OnlineEventMediatorManagerPrx prx = OnlineEventMediatorManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public void sendOnlineEventWapOff(int userId) {
		OnlineEventMediatorManagerPrx prx=null;
		try {
			prx=getOnlineEventMediatorManager();
			prx.sendOnlineEventSingle(userId, xce.mediator.onlineevent.OFFLINEWAP.value);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".sendOnlineEventSingle [" + prx + "] id = " + userId + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".sendOnlineEventSingle [" + prx + "] id = " + userId + e);
		}
	}
	
	public void sendOnlineEventWapOn(int userId) {
		OnlineEventMediatorManagerPrx prx=null;
		try {
			prx=getOnlineEventMediatorManager();
			prx.sendOnlineEventSingle(userId,xce.mediator.onlineevent.ONLINEWAP.value);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".sendOnlineEventSingle [" + prx + "] id = " + userId + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".sendOnlineEventSingle [" + prx + "] id = " + userId + e);
		}
	}

/*	public void sendOnlineEventSingle(int userId, int onlineType) {
		try {
			getOnlineEventMediatorManager().sendOnlineEventSingle(userId, onlineType);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".sendOnlineEventSingle " + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".sendOnlineEventSingle " + e);
		}
	}

	public void sendOnlineEventBatch(Map<Integer, Integer> data) {
		try {
			getOnlineEventMediatorManager().sendOnlineEventBatch(data);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".sendOnlineEventSingle " + e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".sendOnlineEventSingle " + e);
		}
	}*/
	
	public static void main(String[] args) {
		try {
			int userId = 233940269;
			
			OnlineEventMediatorAdapter _adapter = OnlineEventMediatorAdapter.getInstance();
			//上线
			_adapter.sendOnlineEventWapOn(userId);
			//下线
			_adapter.sendOnlineEventWapOff(userId);
		} catch (Exception e) {
			System.out.println(e);
		}

		System.exit(0);
	}
}
