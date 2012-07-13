package com.xiaonei.xce.notify;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import xce._notify.NotifyContent;
import xce._notify.NotifyIndexManagerPrx;
import xce._notify.NotifyIndexManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;


class  NotifyIndexAdapter extends AbstractModProxyHolder<Integer, NotifyIndexManagerPrx>{

	private static NotifyIndexAdapter instance;
	private static String endpoints = "@NotifyIndex";

	private NotifyIndexAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY),
				"M"+endpoints, 5);
	}

	public static NotifyIndexAdapter getInstance() {
		if (instance == null) {
			instance = new NotifyIndexAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public NotifyIndexManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return NotifyIndexManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private NotifyIndexManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private NotifyIndexManagerPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	void removeById(int uid, long notifyId){
		getManagerOneway(uid).removeById(uid, notifyId);
	}
	void removeByIds(int uid, long[] notifyIds){
		getManagerOneway(uid).removeByIds(uid, notifyIds);
	}
	void removeBySource(int uid, int type, long source){
		getManagerOneway(uid).removeBySource(uid, type,source);
	}
	
	int getCount(int user, int[] types){
		return getManager(user).getCount(user,types);
	}
	int getUnreadCountByTypes(int user, int[] types){
		return getManager(user).getUnreadCountByTypes(user,types);
	}
	
	List<Integer> getFromIdSeq(int user, int[] types, int begin, int limit){
		int [] ids =  getManager(user).getFromIdSeq(user,types,begin, limit);
		List<Integer> res = new ArrayList<Integer>();
		for(int i = 0; i < ids.length; ++i){
			res.add(ids[i]);
		}
		return res;
	}
	
	void removeBySourceAndSender(int uid, int type, long source, int sender){
		getManagerOneway(uid).removeBySourceAndSender(uid, type, source, sender);
	}	
	public static void main(String[] args) throws Exception {
		int[] types = {16};
		NotifyIndexAdapter.instance.getUnreadCountByTypes(220678672,types);
	}
}
