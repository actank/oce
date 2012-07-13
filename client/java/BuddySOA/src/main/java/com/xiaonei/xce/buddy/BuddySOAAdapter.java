package com.xiaonei.xce.buddy;

import xce.buddy.BuddySOAManagerPrx;
import xce.buddy.BuddySOAManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import java.util.Vector;

public class BuddySOAAdapter extends ServiceAdapter {

	private static BuddySOAAdapter _instance = new BuddySOAAdapter();

	public static BuddySOAAdapter getInstance() {
		return _instance;
	}

	private BuddySOAAdapter() {
		super("@BuddySOA0", 0, Channel.newBuddyChannel("BuddySOA0"));
	}

	/*-- 以下同时列出了各同的代理调用方式, 可根据实际需要保留和删除 --*/
	private Vector<BuddySOAManagerPrx> managers = new Vector<BuddySOAManagerPrx>();
	private Vector<BuddySOAManagerPrx> managersOneway = new Vector<BuddySOAManagerPrx>();

	public BuddySOAManagerPrx getBuddySOAManager(int userId) {
		return locateProxy(managers, "M", userId, Channel.Invoke.Twoway,BuddySOAManagerPrxHelper.class, 300);
	}

	private BuddySOAManagerPrx getBuddySOAManagerOneway(int userId){
		return locateProxy(managersOneway, "M", userId, Channel.Invoke.Oneway,BuddySOAManagerPrxHelper.class, 300);
	}

	private BuddySOAManagerPrx getBuddySOAManagerUDP() {
		return locateProxy("M", Channel.Invoke.Udp, BuddySOAManagerPrxHelper.class, 300);
	}

}
