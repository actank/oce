package com.xiaonei.xce.ad;

import xce.ad.AdAnalysisManagerPrx;
import xce.ad.AdAnalysisManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class AdAnalysisAdapter extends
		AbstractModProxyHolder<Integer, AdAnalysisManagerPrx> {

	private static AdAnalysisAdapter instance;
	private static String endpoints = "@AdAnalysis";

	public AdAnalysisAdapter() {
		super(ChannelFactory.getChannel(ChannelType.AD), "M" + endpoints, 1);
	}

	public static AdAnalysisAdapter getInstance() {
		if (instance == null) {
			instance = new AdAnalysisAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public AdAnalysisManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return AdAnalysisManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private AdAnalysisManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private AdAnalysisManagerPrx getManager(int uid) {
		return getProxy(uid);
	}

	void reloadGroupById(long groupid) {

		getManager((int) groupid).ReloadGroupById(groupid);

	}

	void reloadMemberById(long memberid) {
		getManager((int) memberid).ReloadMemberById(memberid);
	}

	void reloadCampaignById(long planid) {
		getManager((int) planid).ReloadCampaignById(planid);
	}

	void reloadCreativeById(long creativeid) {
		getManager((int) creativeid).ReloadCreativeById(creativeid);
	}

}
