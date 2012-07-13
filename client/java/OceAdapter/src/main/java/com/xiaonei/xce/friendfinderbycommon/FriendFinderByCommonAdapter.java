package com.xiaonei.xce.friendfinderbycommon;

import java.util.List;
import java.util.ArrayList;

import Ice.ObjectPrx;
import com.xiaonei.xce.friendfinderbycommon.CommonFriendData;
import xce.friendfinder.CommonFriendItem;
import xce.friendfinder.FriendFinderByCommonManagerPrx;
import xce.friendfinder.FriendFinderByCommonManagerPrxHelper;
import xce.util.service.ReplicatedServiceAdapter;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class FriendFinderByCommonAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFriendFinderByCommon";
	private static int _interval = 120;	
	
	private FriendFinderByCommonAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static FriendFinderByCommonAdapter _instance = new FriendFinderByCommonAdapter();
	public static FriendFinderByCommonAdapter getInstance() {
		return _instance;
	}

	public List<CommonFriendData> getCommonFriendItem(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByCommon "
				+ userId);

                List<CommonFriendData> result = new ArrayList<CommonFriendData>();

                CommonFriendItem[] ret;
                FriendFinderByCommonManagerPrx prx = null;
                try {
                	prx = getFriendFinderByCommonManagerTwoway(userId);
			ret = prx.getCommonFriendItemSeq(userId,limit);
			for( CommonFriendItem one: ret ){
				CommonFriendData data = new CommonFriendData();
				data.setBase( one );
				result.add( data );
			}
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByCommon getCommonFriendItem timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public List<Integer> getFriendsByCommon(int guestId,int hostId,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByCommon "
				+ guestId + " => " + hostId);

                List<Integer> result = new ArrayList<Integer>();
                FriendFinderByCommonManagerPrx prx = null;
                try {
                	prx = getFriendFinderByCommonManagerTwoway(guestId);
			int[] ret = prx.getFriendsByCommon(guestId,hostId,limit);
			for( int one: ret ){
				result.add( one );
			}
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error("FriendFinderByCommon getPUAids timeout prx = " + prx + " guestId = " + guestId + " => " + hostId);
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                return result;
	}

	public void reload(int userId) {
		getFriendFinderByCommonManagerOneway(userId).reload(userId);
	}

	protected FriendFinderByCommonManagerPrx getFriendFinderByCommonManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 1000);
		if ( prx0 != null ) {
			FriendFinderByCommonManagerPrx prx = FriendFinderByCommonManagerPrxHelper.uncheckedCast(prx0.ice_timeout(1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected FriendFinderByCommonManagerPrx getFriendFinderByCommonManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 1000);
		if ( prx0 != null ) {
			FriendFinderByCommonManagerPrx prx = FriendFinderByCommonManagerPrxHelper.uncheckedCast(prx0.ice_timeout(1000).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		System.out.println(FriendFinderByCommonAdapter.getInstance().getFriendsByCommon(24496255, 68128, 10));
	}
}

