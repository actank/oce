package com.xiaonei.xce.usercount;

import java.util.Vector;

import xce.usercount.UserCountManagerPrx;
import xce.usercount.UserCountManagerPrxHelper;
import xce.usercount.UserCounter;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import xce.util.tools.UpTime;
import Ice.TimeoutException;
import xce.clusterstate.ReplicatedClusterAdapter;

public final class UserCountAdapter extends ReplicatedClusterAdapter{

	private static UserCountAdapter _adapter = new UserCountAdapter();

	public static final int ONLINE_FRIENDS = 0;
	public static final int POKE = 1;
	public static final int CLASS_INVITE = 2;
	public static final int GUEST_REQUEST = 3;
	public static final int FRIEND_MEMO_REQUEST = 4;
	public static final int NEW_MESSAGE = 5;
	public static final int IM_LEAVE_WORD = 6;
	public static final int TEAM_INVITE = 7;
	public static final int TRIBE_REQUEST = 8;
	public static final int FRIEND_COUNT = 9;
	public static final int REG_APPLY = 10;
	public static final int RECOMMENDED_FRIEND = 11;
	public static final int NOTIFY = 12;
	public static final int EVENT_INVITE = 13;
	public static final int APP_INVITE = 14;
	public static final int PHOTO_TAG_INVITE = 15;
	public static final int SURVEY_INVITE = 16;
	public static final int GAME_INVITE = 17;
	public static final int TEST_INVITE = 18;
	public static final int CALL_INVITE = 19;
	public static final int NEW_REQUEST = 20;
	public static final int CONTACT_REQUEST = 21;
	public static final int OFFER_MSN_CONTACT = 22;
	public static final int PAGE_ADMIN_INVS = 23;	
	public static final int TOTAL_TYPE_SIZE = 24;

	private static final String ctr_endpoints = "ControllerUserCount";

	private static final String service_name = "M";
	
	private static int _interval = 120;

	protected UserCountAdapter() {
		super(ctr_endpoints, _interval);
	}

	public static UserCountAdapter getInstance() {
		return _adapter;
	}

	protected UserCountManagerPrx getUserCountManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserCountManagerPrx prx = UserCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserCountManagerPrx getUserCountManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserCountManagerPrx prx = UserCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void inc(int userId, int type, int step) {
		TimeCost cost = TimeCost.begin("UserCountAdapter.inc id: "+userId+" type: "+type+" step: "+step);		
		try {
			if (userId <= 0) {
				return;
			}	
			getUserCountManagerOneway(userId).inc(userId, type, step);
		} finally {
			cost.endFinally();
		}
	}

	public void dec(int userId, int type, int step) {
		TimeCost cost = TimeCost.begin("UserCountAdapter.dec id: "+userId+" type: "+type+" step: "+step);
		try {
			if (userId <= 0) {
				return;
			}	
			getUserCountManagerOneway(userId).dec(userId, type, step);
		} finally {
			cost.endFinally();
		}
	}

	public void set(int userId, int type, int step) {
		TimeCost cost = TimeCost.begin("UserCountAdapter.set id: "+userId+" type: "+type+" step: "+step);		
		try {
			if (userId <= 0) {
				return;
			}			
			getUserCountManagerOneway(userId).set(userId, type, step);
		} finally {
			cost.endFinally();
		}
	}

	public int get(int userId, int type) {
		TimeCost cost = TimeCost.begin("UserCountAdapter.get id: "+userId+" type: "+type);
		UserCountManagerPrx prx = null ;
		try {
			if (userId <= 0) {
				return 0;
			}
			int ret = 0;
			try {
				prx = getUserCountManager(userId) ;
				ret = prx.get(userId, type);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".get [ " + prx + "] id=" + userId + " type=" + type + " Ice.TimeoutException");
			} catch (Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".get " + " id=" + userId + " type=" + type , e);
			}
			return ret;
		} finally {
			cost.endFinally();
		}
	}

	public UserCountData getAll(int userId) {
		TimeCost cost = TimeCost.begin("UserCountAdapter.getAll id: "+userId);
		UserCountManagerPrx prx = null ;
		try {
			if (userId <= 0) {
				return new UserCountData();
			}
			try {
				prx = getUserCountManager(userId) ;
				UserCounter counter = prx.getAll(userId);
				return new UserCountData(counter);
			} catch (TimeoutException e) 
			{
				Oce.getLogger().error(this.getClass().getName() + ".getAll [ " + prx + "] id=" + userId + " Ice.TimeoutException");
			} catch (Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".getAll " + " id=" + userId , e);
			}
			return new UserCountData();
		} finally {
			cost.endFinally();
		}
	}

	public void reload(int userId) {
		TimeCost cost = TimeCost.begin("UserCountAdapter.reload id: "+userId);
		UserCountManagerPrx prx = null ;
		try {
			if (userId <= 0) {
				return;
			}
			prx = getUserCountManager(userId);
			prx.reload(userId);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".reload [ " + prx + "] id=" + userId + " Ice.TimeoutException");
		} 
		finally {
			cost.endFinally();
		}
	}
}
