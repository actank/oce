package com.xiaonei.xce.ad;

import xce.ad.AdMatchManagerPrx;
import xce.ad.AdMatchManagerPrxHelper;
import xce.ad.AdInfo;
import xce.ad.AdResultMapHolder;
import xce.ad.AdResultMapHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

import java.util.Map;
import java.util.List;
public class AdEngineUAdapter extends
		AbstractModProxyHolder<Integer, AdMatchManagerPrx> {

	private static AdEngineUAdapter instance;
	private static String endpoints = "@AdEngineU";

	public AdEngineUAdapter() {
		super(ChannelFactory.getChannel(ChannelType.AD), "M" + endpoints, 46);
	}

	public static AdEngineUAdapter getInstance() {
		if (instance == null) {
			instance = new AdEngineUAdapter();
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

  //AdResultMap GetAdsTest(int uid,
  Map<Long, AdInfo[] > GetAdsTest(int uid,
                    int stage,
                    int gender,
                    int age,
                    String currentArea,
                    String ipArea,
                    long school,
                    int grade,
                    int count,
                    String referer,
                    int rotateindex) {
    return getManagerOneway(uid).GetAdsTest(uid, stage, gender, age, currentArea, ipArea, school, grade, count, referer, rotateindex);
  }


	void bindUser(int userid, long creativeid, long zoneid){
		getManagerOneway(userid).BindUser(userid, creativeid, zoneid);
	}

  void unbindUser(int userid, long zoneid){
    getManagerOneway(userid).UnbindUser(userid, zoneid);
  }

}
