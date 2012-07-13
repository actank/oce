package com.xiaonei.xce.ad;

import xce.ad.AdLogManagerPrx;
import xce.ad.AdLogManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

import java.util.Map;
import java.util.List;
public class AdLogSAdapter extends
		AbstractModProxyHolder<Integer, AdLogManagerPrx> {

	private static AdLogSAdapter instance;
	private static String endpoints = "@AdLogS";

	public AdLogSAdapter() {
		super(ChannelFactory.getChannel(ChannelType.AD), "M" + endpoints, 1);
	}

	public static AdLogSAdapter getInstance() {
		if (instance == null) {
			instance = new AdLogSAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public AdLogManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return AdLogManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private AdLogManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private AdLogManagerPrx getManager(int uid) {
		return getProxy(uid);
	}

  public void PvBatch(String[] logs) {
    getManagerOneway(0).PvBatch(logs);
  }

  public void ClickForBp(String log) {
    getManagerOneway(0).ClickForBp(log);
  }

  public void ClickForCharge(String log) {
    getManagerOneway(0).ClickForCharge(log);
  }
}
