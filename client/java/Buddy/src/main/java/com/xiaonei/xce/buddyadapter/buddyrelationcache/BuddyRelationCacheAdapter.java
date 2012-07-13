package com.xiaonei.xce.buddyadapter.buddyrelationcache;

import java.util.ArrayList;

import xce.buddy.BuddyDesc;
import xce.buddy.BuddyRelationCacheManagerPrx;
import xce.buddy.BuddyRelationCacheManagerPrxHelper;
import xce.buddy.BuddyRelationLoaderPrx;
import xce.buddy.BuddyRelationLoaderPrxHelper;
import xce.buddy.Relationship;
import xce.clusterstate.ReplicatedClusterAdapter;
import Ice.ObjectPrx;

import com.xiaonei.xce.domain.buddy.BuddyRelation;
import com.xiaonei.xce.domain.buddy.CheckedBuddyRelation;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

/**
 * BuddyRelationCacheAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public class BuddyRelationCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyRelationCache";
	private static int _interval = 120;

	public BuddyRelationCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	// 避免重复连接，应用Singleton模式
	private static BuddyRelationCacheAdapter _instance = new BuddyRelationCacheAdapter();

	public static BuddyRelationCacheAdapter getInstance() {
		return _instance;
	}

	/**
	 * @param fromId
	 * @param toId
	 * @return
	 */
	public BuddyRelation getRelation(int fromId, int toId) {
		// TimeCost stat =
		// TimeCost.begin("[BuddyRelationCache::getRelation] relation[" + fromId +
		// ":" + toId + "]");
		Relationship relation = new Relationship();
		relation.from = fromId;
		relation.to = toId;
		xce.buddy.BuddyDesc desc = xce.buddy.BuddyDesc.NoPath;
		BuddyRelationCacheManagerPrx prx = null;
		try {
			prx = getBuddyRelationCacheManagerTwoway(relation.from);
			desc = prx.getRelation(relation);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelation [" + prx.toString() + "]"
							+ " fromId: " + fromId + "toId" + toId
							+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelation [" + prx.toString() + "]"
							+ " fromId: " + fromId + "toId" + toId + " Ice.TimeoutException");
		} finally {
			// stat.endFinally();
		}

		return new BuddyRelation(relation.from, relation.to,
				com.xiaonei.xce.domain.buddy.BuddyRelation.parseBuddyDesc(desc));
	}

	/**
	 * @param fromId
	 * @param toId
	 * @return
	 */
	public CheckedBuddyRelation getCheckedRelation(int fromId, int toId) {
		BuddyRelationCacheManagerPrx prx = null;
		try {
			prx = getBuddyRelationCacheManagerTwoway(fromId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelation [" + prx.toString() + "]"
							+ " fromId: " + fromId + "toId" + toId
							+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelation [" + prx.toString() + "]"
							+ " fromId: " + fromId + "toId" + toId + " Ice.TimeoutException");
		} finally {
			// stat.endFinally();
		}
		return getRelation(fromId, toId, prx);
	}

	/**
	 * @param fromId
	 * @param toId
	 * @param prx
	 * @return
	 */
	private CheckedBuddyRelation getRelation(int fromId, int toId, ObjectPrx prx) {
		// TimeCost stat =
		// TimeCost.begin("[BuddyRelationCache::getRelation] relation[" + fromId +
		// ":" + toId + "]");
		Relationship relation = new Relationship();
		relation.from = fromId;
		relation.to = toId;
		xce.buddy.BuddyDesc desc = xce.buddy.BuddyDesc.NoPath;
		boolean sucess = false;
		try {
			desc = BuddyRelationCacheManagerPrxHelper.uncheckedCast(prx).getRelation(
					relation);
			sucess = true;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelation [" + prx.toString() + "]"
							+ " fromId: " + fromId + "toId" + toId
							+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelation [" + prx.toString() + "]"
							+ " fromId: " + fromId + "toId" + toId + " Ice.TimeoutException");
		} finally {
			// stat.endFinally();
		}

		BuddyRelation r = new BuddyRelation(relation.from, relation.to,
				com.xiaonei.xce.domain.buddy.BuddyRelation.parseBuddyDesc(desc));
		return new CheckedBuddyRelation(r, sucess);
	}

	/**
	 * @param fromId
	 * @param toId
	 * @return
	 */
	public boolean checkRelation(int fromId, int toId) {
		ArrayList<ObjectPrx> prxs = getAllProxy(fromId);
		if (prxs.size() == 0) {
			System.out.println("no prx");
			return true;
		}
		CheckedBuddyRelation firstRelation = getRelation(fromId, toId, prxs.get(0));
		if (firstRelation == null) {
			System.out.println("first prx == null");
			return false;
		}
		for (int i = 1; i < prxs.size(); ++i) {
			CheckedBuddyRelation r = getRelation(fromId, toId, prxs.get(i));
			if (!firstRelation.equals(r)) {
				return false;
			}
			System.out.println("check ok " + i);
		}
		return true;
	}

	/**
	 * @param relation
	 */
	public void setRelation(BuddyRelation relation) {
		TimeCost stat = TimeCost
				.begin("[BuddyRelationCache::setRelation] relation[" + relation.fromId
						+ ":" + relation.toId + "]");
		try {
			BuddyRelationLoaderAdapter.getInstance().setRelation(relation);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger()
					.error(
							this.getClass().getName() + ".setRelation relation["
									+ relation.fromId + ":" + relation.toId
									+ " Ice.TimeoutException");
		} finally {
			stat.endFinally();
		}
	}

	/**
	 * @param userId
	 */
	public void reload(int userId) {
		BuddyRelationLoaderAdapter.getInstance().reload(userId);
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyRelationCacheManagerPrx getBuddyRelationCacheManagerTwoway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyRelationCacheManagerPrx prx = BuddyRelationCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyRelationCacheManagerPrx getBuddyRelationCacheManagerOneway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyRelationCacheManagerPrx prx = BuddyRelationCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}

/**
 * @author yuyang
 * 
 */
class BuddyRelationLoaderAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyRelationLoader";
	private static int _interval = 120;

	private BuddyRelationLoaderAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyRelationLoaderAdapter _instance = new BuddyRelationLoaderAdapter();

	public static BuddyRelationLoaderAdapter getInstance() {
		return _instance;
	}

	/**
	 * @param userId
	 */
	public void reload(int userId) {
		try {
			getBuddyRelationLoaderOneway(userId).reload(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".Reload userId: " + userId
							+ " Ice.TimeoutException");
		}
	}

	/**
	 * @param relation
	 */
	public void setRelation(BuddyRelation relation) {
		BuddyRelationLoaderPrx prx = null;
		try {
			Relationship _relation = new Relationship();
			_relation.from = relation.fromId;
			_relation.to = relation.toId;
			BuddyDesc _desc = BuddyRelation.parseBuddyDesc(relation.getDesc());
			prx = getBuddyRelationLoaderTwoway(_relation.from);
			prx.setRelation(_relation, _desc);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setRelation [" + prx.toString()
							+ "] relation[" + relation.fromId + ":" + relation.toId
							+ " Ice.TimeoutException");
		}
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyRelationLoaderPrx getBuddyRelationLoaderOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyRelationLoaderPrx prx = BuddyRelationLoaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyRelationLoaderPrx getBuddyRelationLoaderTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyRelationLoaderPrx prx = BuddyRelationLoaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String args[]) {
		BuddyRelationCacheAdapter.getInstance().checkRelation(67913, 342549289);
		BuddyRelationCacheAdapter.getInstance().checkRelation(67913, 67913);
		BuddyRelationCacheAdapter.getInstance().checkRelation(67913, 123459845);
		CheckedBuddyRelation r = BuddyRelationCacheAdapter.getInstance()
				.getCheckedRelation(67913, 342549289);
		System.out.println(r.getSuccess() + " " + r.getRelation().getDesc());
	}
}
