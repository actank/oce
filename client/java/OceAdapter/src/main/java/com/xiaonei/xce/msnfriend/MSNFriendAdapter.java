package com.xiaonei.xce.msnfriend;

import java.util.Vector;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.mfd.MSNFriendManagerPrx;
import xce.mfd.MSNFriendManagerPrxHelper;
import xce.mfd.MSNFriend;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;



public class MSNFriendAdapter extends ServiceAdapter {

	
	
	private static final String endpoints = "@MSNFriend";
	
	private Vector<MSNFriendManagerPrx> manager = new Vector<MSNFriendManagerPrx>();

	private static MSNFriendAdapter instance = new MSNFriendAdapter();


	public static MSNFriendAdapter getInstance() {
		return instance;
	}
 
	protected MSNFriendAdapter() {
		super(endpoints, 1, Channel.newOceChannel(endpoints));
	}

	private MSNFriendManagerPrx getMSNFriendManager(int id) {
		return locateProxy(manager, "M", id, Channel.Invoke.Twoway,
				MSNFriendManagerPrxHelper.class, 300);
	}

	
	public MSNFriend[] getFriendWithCommon(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "MSNFriend "
				+ userId);
    MSNFriendManagerPrx prx = null;
		try {
      prx = getMSNFriendManager(userId);
			return prx.getFriendWithCommon(userId, limit);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("getFriendWithCommon timeout " + userId+" "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return null;
	}
}
