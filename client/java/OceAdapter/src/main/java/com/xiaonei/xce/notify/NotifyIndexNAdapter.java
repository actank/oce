package com.xiaonei.xce.notify;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import xce._notify.NotifyContent;
import xce._notify.NotifyIndexManagerNPrx;
import xce._notify.NotifyIndexManagerNPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;


public class  NotifyIndexNAdapter extends AbstractModProxyHolder<Integer, NotifyIndexManagerNPrx>{

	private static NotifyIndexNAdapter instance;
	private static String endpoints = "@NotifyIndexN";

	private NotifyIndexNAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY),
				"M"+endpoints, 5);
	}

	public static NotifyIndexNAdapter getInstance() {
		if (instance == null) {
			instance = new NotifyIndexNAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public NotifyIndexManagerNPrx uncheckedCast(ObjectPrx baseProxy) {
		return NotifyIndexManagerNPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private NotifyIndexManagerNPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private NotifyIndexManagerNPrx getManager(int uid) {
		return getProxy(uid);
	}

/*
 *   void RemoveRMessageById(int uid, long notifyId, int type, int bigtype);
 *     void RemoveRMessageByIds(int uid, MyUtil::LongSeq notifyIds, int type, int bigtype);
 *       void RemoveRMessageBySource(int uid, int type, long source, int bigtype);
 *
 * */
	public void RemoveRMessageById(int uid, long notifyId, int type){
		getManagerOneway(uid).RemoveRMessageById(uid, notifyId, type, 1);
	}
	public void RemoveRMessageByIds(int uid, long[] notifyIds, int type){
		getManagerOneway(uid).RemoveRMessageByIds(uid, notifyIds, type, 1);
	}
	public void RemoveRMessageBySource(int uid, int type, long source){
		getManagerOneway(uid).RemoveRMessageBySource(uid, type, source, 1);
	}
	public void RemoveRMessageBySourceSeq(int uid, int type, long[] sources){
		getManagerOneway(uid).RemoveRMessageBySourceSeq(uid, type, sources, 1);
	}
/*
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
	}		public static void main(String[] args) throws Exception {
		int[] types = {16};
		NotifyIndexNAdapter.instance.RemoveRMessageById(232626782, 123456789, 14);
		//NotifyIndexNAdapter.instance.getUnreadCountByTypes(220678672,types);
	}*/

	
}
