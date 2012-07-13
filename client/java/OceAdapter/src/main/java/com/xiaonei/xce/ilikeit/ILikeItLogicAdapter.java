package com.xiaonei.xce.ilikeit;

import com.xiaonei.xce.log.Oce;
import java.util.Map;
import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.ObjectPrx;
import Ice.TimeoutException;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;
import com.xiaonei.xce.usercache.UserCacheFactory;
import xce.serverstate.ServerStateInfo;
import xce.clusterstate.ClientInterface;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.ilikeit.GlobalId;
import xce.ilikeit.LikeInfo;
import xce.ilikeit.Likeship;
import xce.ilikeit.ILikeItManagerPrx;
import xce.ilikeit.ILikeItManagerPrxHelper;
import xce.ilikeit.UserDetail;
import xce.clusterstate.ClientInterface;

public final class ILikeItLogicAdapter extends ReplicatedClusterAdapter {

	private static int _interval = 300;
	private static final String _ctlEndpoints = "ControllerILikeItLogic";
	private static int _cluster = 1;

	private static ILikeItLogicAdapter _adapter = new ILikeItLogicAdapter();

	public static ILikeItLogicAdapter instance() {
		return _adapter;
	}

	private ILikeItLogicAdapter() {
	  super(_ctlEndpoints,_interval);
	};

	public void addLike(String gidStr, int uid) {
		ILikeItManagerPrx prx = null;
		try {
			com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId
					.fromString(gidStr);
			Likeship ship = new Likeship();
			ship.gid = new xce.ilikeit.GlobalId();
			ship.gid.type = gid.getType();
			ship.gid.id = gid.getId();
			ship.uid = uid;
			prx = getILikeItManagerOneWay(0);
			prx.addLike(ship);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLike [" + prx
							+ "] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLike [" + prx
							+ "] Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLike [" + prx + "] ", e);
		}
	};

	public void addLike(com.xiaonei.xce.globalid.GlobalId gid, int uid) {
		ILikeItManagerPrx prx = null;
		try {
			Likeship ship = new Likeship();
			ship.gid = new GlobalId();
			ship.gid.type = gid.getType();
			ship.gid.id = gid.getId();
			ship.uid = uid;
			prx = getILikeItManagerOneWay(0);
			prx.addLike(ship);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLike [" + prx
							+ "] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLike [" + prx
							+ "] Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLike [" + prx + "] ", e);
		}
	};

	public void removeLike(String gidStr, int uid) {
		ILikeItManagerPrx prx = null;
		try {
			com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId
					.fromString(gidStr);
			Likeship ship = new Likeship();
			ship.gid = new GlobalId();
			ship.gid.type = gid.getType();
			ship.gid.id = gid.getId();
			ship.uid = uid;
			prx = getILikeItManagerOneWay(0);
			prx.removeLike(ship);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeLike [" + prx
							+ "] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeLike [" + prx
							+ "] Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeLike [" + prx + "]", e);
		}
	};

	public void addLikeBatch(Likeship[] ships) {
		ILikeItManagerPrx prx = null;
		try {
			prx = getILikeItManagerOneWay(0);
			prx.addLikeBatch(ships);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLikeBatch [" + prx
							+ "] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLikeBatch [" + prx
							+ "] Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".addLikeBatch [" + prx + "] ",
					e);
		}
	};

	public void removeLikeBatch(Likeship[] ships) {
		ILikeItManagerPrx prx = null;
		try {
			prx = getILikeItManagerOneWay(0);
			prx.removeLikeBatch(ships);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeLikeBatch [" + prx
							+ "] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeLikeBatch [" + prx
							+ "] Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".removeLikeBatch [" + prx
							+ "] ", e);
		}
	};

	protected ILikeItManagerPrx getILikeItManager(long id) {
	  Long mod = id % _cluster;
		Ice.ObjectPrx prx0 = getProxy(mod.intValue());
		if (prx0 != null) {
			ILikeItManagerPrx prx = ILikeItManagerPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected ILikeItManagerPrx getILikeItManagerOneWay(int id) {
		Ice.ObjectPrx prx0 = getProxy(id);
		if (prx0 != null) {
			ILikeItManagerPrx prx = ILikeItManagerPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	// 测试
	public static void main(String[] args) {
		String gidStr = "album_342193355";
		int uid = 222525090;

		int limit = 1;
		int id = 0;
		com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId
				.fromConnect(id);
		ILikeItLogicAdapter.instance().addLike(gid, uid);
		LikeInfo info = ILikeItCacheAdapter.instance().getLikeDetail(gid, uid,
				limit, true);
		UserDetail[] userInfos = info.detailList;
		System.out.println(info.totalCount);
		for (UserDetail userinfo : userInfos) {
			System.out.println(userinfo.uid + " " + userinfo.name + " "
					+ userinfo.img);
		}
		_adapter.addLike(gidStr, uid);
		System.out.println("Add Like " + uid + " to " + gidStr);
		/*
		 * _adapter.removeLike(gidStr, uid); System.out.println("Remove Like " +
		 * uid + " to " + gidStr);
		 */

		System.exit(0);
	}
};
