package com.xiaonei.xce.notify;

import java.util.Map;

import xce._notify.NotifyDispatcherPrx;
import xce._notify.NotifyDispatcherPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

class  NotifyDispatcherAdapter extends AbstractModProxyHolder<Integer, NotifyDispatcherPrx>{

	private static NotifyDispatcherAdapter instance;
	private static String endpoints = "@NotifyDispatcher";

	private NotifyDispatcherAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY),
				"M"+endpoints, 1);
	}

	public static NotifyDispatcherAdapter getInstance() {
		if (instance == null) {
			instance = new NotifyDispatcherAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public NotifyDispatcherPrx uncheckedCast(ObjectPrx baseProxy) {
		return NotifyDispatcherPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private NotifyDispatcherPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private NotifyDispatcherPrx getManager(int uid) {
		return uncheckedCast(getProxy(uid).ice_twoway());
	}
	
	public void dispatch(Map<String,String> props){
		try {
			getManager(0).dispatch(props);
		} catch(xce._notify.KeyAbsentException e){
			e.printStackTrace();
		}catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void deliverWebpager(Map<String,String> props){
		try {
			getManager(0).deliverWebpager(props);
		} catch(xce._notify.KeyAbsentException e){
			e.printStackTrace();
		}catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
