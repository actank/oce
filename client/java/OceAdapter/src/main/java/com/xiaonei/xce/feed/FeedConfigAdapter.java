package com.xiaonei.xce.feed;

import xce.feed.ConfigManagerPrx;
import xce.feed.ConfigManagerPrxHelper;
import xce.feed.UserConfig;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;
import java.util.Map;


public class FeedConfigAdapter extends AbstractModProxyHolder<Integer, ConfigManagerPrx>{

	private static FeedConfigAdapter instance;
	private static String endpoints = "@FeedConfig";

	public FeedConfigAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"M"+endpoints, 1);
	}

	public static FeedConfigAdapter getInstance() {
		if (instance == null) {
			instance = new FeedConfigAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public ConfigManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return ConfigManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private ConfigManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private ConfigManagerPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	public void setUserConfig(int uid, int stype, int appid, boolean sendConfig, boolean recvConfig){
		getManagerOneway(uid).SetUserConfig(uid, stype, appid, sendConfig, recvConfig);
	}
	public UserConfig getUserConfig(int uid, int stype, int appid){
		return getManager(uid).GetUserConfig(uid, stype, appid);
	}

  public Map<Integer,UserConfig> getUserBlockedAppDict(int uid)
  {
		return getManager(uid).GetUserBlockedAppDict(uid);
  }  

}
