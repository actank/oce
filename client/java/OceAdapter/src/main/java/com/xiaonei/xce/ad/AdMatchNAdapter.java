package com.xiaonei.xce.ad;

import xce.ad.AdMatchManagerPrx;
import xce.ad.AdMatchManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class AdMatchNAdapter extends
		AbstractModProxyHolder<Integer, AdMatchManagerPrx> {

	private static AdMatchNAdapter instance;
	private static String endpoints = "@AdMatchN";

	public AdMatchNAdapter() {
		super(ChannelFactory.getChannel(ChannelType.AD), "M" + endpoints, 10);
	}

	public static AdMatchNAdapter getInstance() {
		if (instance == null) {
			instance = new AdMatchNAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public AdMatchManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return AdMatchManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private AdMatchManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private AdMatchManagerPrx getManager(int uid) {
		return getProxy(uid);
	}

	void bindUser(int userid, long creativeid, long zoneid){
		getManagerOneway(userid).BindUser(userid, creativeid, zoneid);
	}

  void unbindUser(int userid, long zoneid){
    getManagerOneway(userid).UnbindUser(userid, zoneid);
  }

}
