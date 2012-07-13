package com.xiaonei.xce.notify;

import java.util.ArrayList;
import java.util.List;

import xce._notify.NotifyGatePrx;
import xce._notify.NotifyGatePrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class NotifyGateAdapter extends AbstractModProxyHolder<Integer, NotifyGatePrx>{

		private static NotifyGateAdapter instance;
		private static String endpoints = "@NotifyGate";

		private NotifyGateAdapter() {
			super(ChannelFactory.getChannel(ChannelType.NOTIFY),
					"M"+endpoints, 5);
		}

		public static NotifyGateAdapter getInstance() {
			if (instance == null) {
				instance = new NotifyGateAdapter();
			}
			return instance;
		}

		@Override
		protected int mod(Integer key, int size) {
			return key % size;
		}

		@Override
		public NotifyGatePrx uncheckedCast(ObjectPrx baseProxy) {
			return NotifyGatePrxHelper.uncheckedCast(baseProxy);
		}

		@Override
		protected ObjectPrx initProxy(ObjectPrx baseProxy) {
			return baseProxy;
		}
		
		private NotifyGatePrx getManagerOneway(int uid) {
			return uncheckedCast(getProxy(uid).ice_oneway());
		}

		private NotifyGatePrx getManager(int uid) {
			return getProxy(uid);
		}
		
		public List<String> getByType(int uid,  int view, int[] type, long lastNotifyId, int limit){
			List<String> l = new ArrayList<String>();
			String[] res = getManager(uid).getByType(uid, view, type, lastNotifyId, limit);
			for(int i = 0 ;i < res.length; ++i){
				l.add(res[i]);
			}
			return l;
		}
		public List<String> getReplySeq(int uid, int view, long lastNotifyId, int limit){
			List<String> l = new ArrayList<String>();
			String[] res = getManager(uid).getReplySeq(uid, view, lastNotifyId, limit);
			for(int i = 0 ;i < res.length; ++i){
				l.add(res[i]);
			}
			return l;
		}
		public List<String> getNotifySeq(int uid, int view, long lastNotifyId, int limit){
			List<String> l = new ArrayList<String>();
			String[] res = getManager(uid).getNotifySeq(uid, view, lastNotifyId, limit);
			for(int i = 0 ;i < res.length; ++i){
				l.add(res[i]);
			}
			return l;
		}
		
		public List<String> getByType2(int uid, int view, int[] type, int begin, int limit){
			List<String> l = new ArrayList<String>();
			String[] res = getManager(uid).getByType2(uid, view, type, begin, limit);
			for(int i = 0 ;i < res.length; ++i){
				l.add(res[i]);
			}
			return l;
		}
		public List<String> getReplySeq2(int uid, int view, int begin, int limit){
			List<String> l = new ArrayList<String>();
			String[] res = getManager(uid).getReplySeq2(uid, view, begin, limit);
			for(int i = 0 ;i < res.length; ++i){
				l.add(res[i]);
			}
			return l;
		}
		public List<String> getNotifySeq2(int uid, int view, int begin, int limit){
			List<String> l = new ArrayList<String>();
			String[] res = getManager(uid).getNotifySeq2(uid, view, begin, limit);
			for(int i = 0 ;i < res.length; ++i){
				l.add(res[i]);
			}
			return l;
		}
		//  void RemoveLogicSource(int uid, int type, long source);
		//    void RemoveLogicBySourceSeq(int uid, int type, MyUtil::LongSeq sources);
		public void RemoveLogicBySource(int uid, int type, long source){
			getManagerOneway(uid).RemoveLogicSource(uid, type, source);
		}
		public void RemoveLogicBySourceSeq(int uid, int type, long[] sources){
			getManagerOneway(uid).RemoveLogicBySourceSeq(uid, type, sources);
		}
}
