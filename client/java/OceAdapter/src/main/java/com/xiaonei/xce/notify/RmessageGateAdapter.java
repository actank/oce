package com.xiaonei.xce.notify;

import java.util.ArrayList;
import java.util.List;

import xce._notify.RmessageGatePrx;
import xce._notify.RmessageGatePrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class RmessageGateAdapter extends AbstractModProxyHolder<Integer, RmessageGatePrx>{
	private static RmessageGateAdapter instance;
	private static String endpoints = "@RmessageGate";

	private RmessageGateAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY),
				"M"+endpoints, 1);
	}
	public static RmessageGateAdapter getInstance() {
		if (instance == null) {
			instance = new RmessageGateAdapter();
		}
		return instance;
	}
	@Override
		protected int mod(Integer key, int size) {
			return key % size;
		}

	@Override
		public RmessageGatePrx uncheckedCast(ObjectPrx baseProxy) {
			return RmessageGatePrxHelper.uncheckedCast(baseProxy);
		}
	@Override
		protected ObjectPrx initProxy(ObjectPrx baseProxy) {
			return baseProxy;
		}

	private RmessageGatePrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}
	private RmessageGatePrx getManager(int uid) {
		return getProxy(uid);
	}
	public String getHtmlContent(int uid) {
		String res = getManager(uid).GetHtmlContent(uid);
		return res;
	}
}
