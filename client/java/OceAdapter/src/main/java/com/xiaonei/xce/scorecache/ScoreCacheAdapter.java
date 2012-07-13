package com.xiaonei.xce.scorecache;

import com.xiaonei.xce.log.Oce;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.scorecache.ScoreDataN;
import xce.scorecache.ScoreCacheNManagerPrx;
import xce.scorecache.ScoreCacheNManagerPrxHelper;
import xce.scorecache.ScoreLoaderNPrx;
import xce.scorecache.ScoreLoaderNPrxHelper;

import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;
import com.xiaonei.xce.scorecache.ScoreCache.ScoreSignType;

import xce.clusterstate.ReplicatedClusterAdapter;

public final class ScoreCacheAdapter<T extends ScoreCache> extends
		ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerScoreCacheN";
	private ScoreCacheFactory<T> _factory;
	private static int _interval = 120;

	private static ScoreCacheAdapter<DefaultScoreCache> _instance = new ScoreCacheAdapter<DefaultScoreCache>(
			new DefaultScoreCacheFactory());

	public static ScoreCacheAdapter<DefaultScoreCache> getInstance() {
		return _instance;
	}

	public ScoreCacheAdapter(ScoreCacheFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public T getScoreData(int userId) {
		TimeCost cost = TimeCost.begin("getScoreData");
		T o = _factory.createScoreCache();
		ScoreCacheNManagerPrx prx = null;
		try {
			prx = getScoreCacheManagerTwoway(userId);
			if (prx != null) {
				ScoreDataN data = prx.getScoreDataN(userId);
				o.setBase(data);
			} else {
				return null;
			}
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getScoreData [" + prx + "] userId="
							+ userId + " Ice.ConnectTimeoutException");
			o = null;
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getScoreData [" + prx + "] userId="
							+ userId + " Ice.TimeoutException");
			o = null;
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getScoreData [" + prx + "] userId="
							+ userId, e);
			o = null;
		} finally {
			cost.endFinally();
		}
		return o;
	}

	public Map<Integer, T> getScoreDataMap(List<Integer> userIds) {
		TimeCost cost = TimeCost.begin("GetScoreCacheMap");

		Map<Integer, T> allResults = new HashMap<Integer, T>();
		if (userIds.isEmpty()) {
			XceStat.getLogger().warn(
					this.getClass().getName() + ".getScoreCacheMap "
							+ " idSize=0");
			return allResults;
		}
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			ScoreCacheNManagerPrx prx = null;
			try {
				Map<Integer, ScoreDataN> subTargets = null;
				prx = getScoreCacheManagerTwoway(i);
				if (prx != null) {
					subTargets = prx.getScoreDataNMap(MathUtil
							.list2array(subUserIds));
				}
				if (subTargets != null) {
					for (Entry<Integer, ScoreDataN> obj : subTargets.entrySet()) {
						T o = _factory.createScoreCache();
						o.setBase(obj.getValue());
						allResults.put(o.getId(), o);
					}
				}
			} catch (ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getScoreCacheMap [" + prx + "]"
								+ " idSize=" + subUserIds.size()
								+ " Ice.ConnectTimeoutException");
			} catch (TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getScoreCacheMap [" + prx + "]"
								+ " idSize=" + subUserIds.size()
								+ " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getScoreCacheMap [" + prx + "]"
								+ " idSize=" + subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	public T getScoreDataNotNull(int userId) {
		TimeCost cost = TimeCost.begin("getScoreDataNotNull");
		T o = _factory.createScoreCache();
		ScoreCacheNManagerPrx prx = null;
		try {
			prx = getScoreCacheManagerTwoway(userId);
			if (prx != null) {
				ScoreDataN data = prx.getScoreDataNNotNull(userId);
				o.setBase(data);
			} else {
				return null;
			}
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getScoreDataNotNull [" + prx + "] userId="
							+ userId + " Ice.ConnectTimeoutException");
			o = null;
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getScoreDataNotNull [" + prx + "] userId="
							+ userId + " Ice.TimeoutException");
			o = null;
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getScoreDataNotNull [" + prx + "] userId="
							+ userId, e);
			o = null;
		} finally {
			cost.endFinally();
		}
		return o;
	}

	public Map<Integer, T> getScoreDataMapNotNull(List<Integer> userIds) {
		TimeCost cost = TimeCost.begin("GetScoreCacheMapNotNull");

		Map<Integer, T> allResults = new HashMap<Integer, T>();
		if (userIds.isEmpty()) {
			XceStat.getLogger().warn(
					this.getClass().getName() + ".getScoreCacheMapNotNull "
							+ " idSize=0");
			return allResults;
		}
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			ScoreCacheNManagerPrx prx = null;
			try {
				Map<Integer, ScoreDataN> subTargets = null;
				prx = getScoreCacheManagerTwoway(i);
				if (prx != null) {
					subTargets = prx.getScoreDataNMapNotNull(MathUtil
							.list2array(subUserIds));
				}
				if (subTargets != null) {
					for (Entry<Integer, ScoreDataN> obj : subTargets.entrySet()) {
						T o = _factory.createScoreCache();
						o.setBase(obj.getValue());
						allResults.put(o.getId(), o);
					}
				}
			} catch (ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getScoreCacheMapNotNull [" + prx + "]"
								+ " idSize=" + subUserIds.size()
								+ " Ice.ConnectTimeoutException");
			} catch (TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getScoreCacheMapNotNull [" + prx + "]"
								+ " idSize=" + subUserIds.size()
								+ " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".getScoreCacheMapNotNull [" + prx + "]"
								+ " idSize=" + subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	public List<T> getScoreCacheList(List<Integer> userIds) {
		/* re-order the result from map */
		TimeCost cost = TimeCost
				.begin(XceStat.getLogger(), "GetScoreCacheList");
		Map<Integer, T> allResults = getScoreDataMap(userIds);
		List<T> result = new ArrayList<T>();
		for (int userId : userIds) {
			T obj = allResults.get(userId);
			if (obj == null)
				continue;
			result.add(obj);
		}
		cost.endFinally();
		return result;
	}

	public void setLoginDays(int userId, int historyLoginDays,
			int continueLoginDays, long lastLoginTime, int loginType) {
		ScoreLoaderAdapter.getInstance().setLoginDays(userId, historyLoginDays,
				continueLoginDays, lastLoginTime, loginType);
	}

	public void updateLoginType(int userId, int loginType) {
		ScoreLoaderAdapter.getInstance().updateLoginType(userId, loginType);
	}

	public DefaultScoreCache setLoginDaysWithRes(int userId, int historyLoginDays,
			int continueLoginDays, long lastLoginTime, int loginType) {
		return ScoreLoaderAdapter.getInstance().setLoginDaysWithRes(userId, historyLoginDays,
				continueLoginDays, lastLoginTime, loginType);
	}

	public DefaultScoreCache updateLoginTypeWithRes(int userId, int loginType) {
		return ScoreLoaderAdapter.getInstance().updateLoginTypeWithRes(userId, loginType);
	}

	public void incScore(int userId, int inc) {
		ScoreLoaderAdapter.getInstance().incScore(userId, inc);
	}

	public void setScore(int userId, int score) {
		ScoreLoaderAdapter.getInstance().setScore(userId, score);
	}

	public void decScore(int userId, int dec) {
		ScoreLoaderAdapter.getInstance().decScore(userId, dec);
	}

	public void incAwardCount(int userId, int inc) {
		ScoreLoaderAdapter.getInstance().incAwardCount(userId, inc);
	}

	public void setAwardCount(int userId, int awardCount) {
		ScoreLoaderAdapter.getInstance().setAwardCount(userId, awardCount);
	}

	public void decAwardCount(int userId, int dec) {
		ScoreLoaderAdapter.getInstance().decAwardCount(userId, dec);
	}

	public void setOn(int userId, ScoreSignType type) {
		ScoreLoaderAdapter.getInstance().setOn(userId, type);
	}

	public void setOff(int userId, ScoreSignType type) {
		ScoreLoaderAdapter.getInstance().setOff(userId, type);
	}

	public void setNewlyUpgrade(int userId, boolean on) {
		ScoreLoaderAdapter.getInstance().setNewlyUpgrade(userId, on);
	}

	public void setDoubleScore(int userId, boolean on) {
		ScoreLoaderAdapter.getInstance().setDoubleScore(userId, on);
	}

	public void setMask(long mask) {
		ScoreLoaderAdapter.getInstance().setMask(mask);
	}

	protected ScoreCacheNManagerPrx getScoreCacheManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			ScoreCacheNManagerPrx prx = ScoreCacheNManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		try {
			ScoreCacheAdapter<DefaultScoreCache> adapter = new ScoreCacheAdapter<DefaultScoreCache>(
					new DefaultScoreCacheFactory());
			int[] ids = {702613154,263873238,702621036,263875730,702918267,264259809,702613267,263875152};
			List<Integer> userIds = new ArrayList<Integer>();
			for( int i: ids){
				userIds.add(i);
//				adapter.setOn(i, ScoreSignType.TYPE_NO_FOOTPRINT);
			}
			Map<Integer, DefaultScoreCache> a = adapter.getScoreDataMap(userIds);
			for( Entry<Integer,DefaultScoreCache> one : a.entrySet() ){
				System.out.println(one.toString());
			}
//			int userId = 24496255;
//			 DefaultScoreCache ret = adapter.getScoreData(userId);
//			 System.out.println(ret.toString());
//			 
//			 adapter.incAwardCount(userId, 2);
//			 adapter.incScore(userId, 2);
//			 adapter.decAwardCount(userId, 1);
//			 adapter.decScore(userId, 1);
//
//			 adapter.setOn(userId, ScoreSignType.TYPE_FOOTPRINT_REMOVABLE);
//			 adapter.setOn(userId, ScoreSignType.TYPE_COLOURFUL_TALKBOX);
//			 Thread.sleep(500);
//			 adapter.setOff(userId, ScoreSignType.TYPE_COLOURFUL_TALKBOX);
//
//			 Thread.sleep(1000);
//			 ret = adapter.getScoreData(userId);
//			 System.out.println(ret.toString());
			 
//			List<Integer> ids = new ArrayList<Integer>();
//			ids.add(userId);
//			Map<Integer, DefaultScoreCache> map = adapter.getScoreDataMap(ids);
//			for (Entry<Integer, DefaultScoreCache> entry : map.entrySet()) {
//				System.out.println(entry.toString());
//			}
		} catch (Exception e) {
			Oce.getLogger().error(e);
		}
		System.exit(0);
	}
}

class ScoreLoaderAdapter<T extends ScoreCache> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerScoreLoaderN";
	private ScoreCacheFactory<T> _factory;
	private static int _interval = 120;

	private ScoreLoaderAdapter(ScoreCacheFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	private static ScoreLoaderAdapter<DefaultScoreCache> _instance = new ScoreLoaderAdapter<DefaultScoreCache>(
			new DefaultScoreCacheFactory());

	public static ScoreLoaderAdapter<DefaultScoreCache> getInstance() {
		return _instance;
	}

	public void setLoginDays(int userId, int historyLoginDays,
			int continueLoginDays, long lastLoginTime, int loginType) {
		try {
			getScoreLoaderOneway(userId).setLoginDays(userId, historyLoginDays,
					continueLoginDays, lastLoginTime, loginType);

		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setLoginDays " + " userId="
							+ userId + " historyLoginDays=" + historyLoginDays
							+ " continueLoginDays=" + continueLoginDays
							+ " lastLoginTime=" + lastLoginTime + " loginType="
							+ loginType + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setLoginDays " + " userId="
							+ userId + " historyLoginDays=" + historyLoginDays
							+ " continueLoginDays=" + continueLoginDays
							+ " lastLoginTime=" + lastLoginTime + " loginType="
							+ loginType + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setLoginDays " + " userId="
							+ userId + " historyLoginDays=" + historyLoginDays
							+ " continueLoginDays=" + continueLoginDays
							+ " lastLoginTime=" + lastLoginTime + " loginType="
							+ loginType, e);
		}
	}

	public void updateLoginType(int userId, int loginType) {
		try {
			getScoreLoaderOneway(userId).updateLoginType(userId, loginType);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateLoginType "
							+ " userId=" + userId + " loginType=" + loginType
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateLoginType "
							+ " userId=" + userId + " loginType=" + loginType
							+ " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateLoginType "
							+ " userId=" + userId + " loginType=" + loginType,
					e);
		}
	}

	public T setLoginDaysWithRes(int userId, int historyLoginDays,
			int continueLoginDays, long lastLoginTime, int loginType) {
		T o = _factory.createScoreCache();
		try {
			ScoreDataN data = getScoreLoaderTwoway(userId).setLoginDaysWithRes(userId, historyLoginDays,
					continueLoginDays, lastLoginTime, loginType);
			o.setBase(data);
			return o;
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setLoginDaysWithRes " + " userId="
							+ userId + " historyLoginDays=" + historyLoginDays
							+ " continueLoginDays=" + continueLoginDays
							+ " lastLoginTime=" + lastLoginTime + " loginType="
							+ loginType + " Ice.ConnectTimeoutException");
			throw e;
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setLoginDaysWithRes " + " userId="
							+ userId + " historyLoginDays=" + historyLoginDays
							+ " continueLoginDays=" + continueLoginDays
							+ " lastLoginTime=" + lastLoginTime + " loginType="
							+ loginType + " Ice.TimeoutException");
			throw e;
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setLoginDaysWithRes " + " userId="
							+ userId + " historyLoginDays=" + historyLoginDays
							+ " continueLoginDays=" + continueLoginDays
							+ " lastLoginTime=" + lastLoginTime + " loginType="
							+ loginType, e);
			throw new RuntimeException(e);
		}
	}

	public T updateLoginTypeWithRes(int userId, int loginType) {
		T o = _factory.createScoreCache();
		try {
			ScoreDataN data = getScoreLoaderTwoway(userId).updateLoginTypeWithRes(userId, loginType);
			o.setBase(data);
			return o;
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateLoginTypeWithRes "
							+ " userId=" + userId + " loginType=" + loginType
							+ " Ice.ConnectTimeoutException");
			throw e;
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateLoginTypeWithRes "
							+ " userId=" + userId + " loginType=" + loginType
							+ " Ice.TimeoutException");
			throw e;
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".updateLoginTypeWithRes "
							+ " userId=" + userId + " loginType=" + loginType,
					e);
			throw new RuntimeException(e);
		}
	}

	public void incScore(int userId, int inc) {
		try {
			getScoreLoaderOneway(userId).incScore(userId, inc);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".incScore " + " userId="
							+ userId + " inc=" + inc
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".incScore " + " userId="
							+ userId + " inc=" + inc + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".incScore " + " userId="
							+ userId + " inc=" + inc, e);
		}
	}

	public void setScore(int userId, int score) {
		try {
			getScoreLoaderOneway(userId).setScore(userId, score);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setScore " + " userId="
							+ userId + " score=" + score
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setScore " + " userId="
							+ userId + " score=" + score + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setScore " + " userId="
							+ userId + " score=" + score, e);
		}
	}

	public void decScore(int userId, int dec) {
		try {
			getScoreLoaderOneway(userId).decScore(userId, dec);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".decScore " + " userId="
							+ userId + " dec=" + dec
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".decScore " + " userId="
							+ userId + " dec=" + dec + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".decScore " + " userId="
							+ userId + " dec=" + dec, e);
		}
	}

	public void setAwardCount(int userId, int awardCount) {
		try {
			getScoreLoaderOneway(userId).setAwardCount(userId, awardCount);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setAwardCount " + " userId="
							+ userId + " awardCount=" + awardCount
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setAwardCount " + " userId="
							+ userId + " awardCount=" + awardCount + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setAwardCount " + " userId="
							+ userId + " awardCount=" + awardCount, e);
		}
	}

	public void incAwardCount(int userId, int inc) {
		try {
			getScoreLoaderOneway(userId).incAwardCount(userId, inc);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".incAwardCount " + " userId="
							+ userId + " inc=" + inc
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".incAwardCount " + " userId="
							+ userId + " inc=" + inc + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".incAwardCount " + " userId="
							+ userId + " inc=" + inc, e);
		}
	}

	public void decAwardCount(int userId, int dec) {
		try {
			getScoreLoaderOneway(userId).decAwardCount(userId, dec);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".decAwardCount " + " userId="
							+ userId + " dec=" + dec
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".decAwardCount " + " userId="
							+ userId + " dec=" + dec + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".decAwardCount " + " userId="
							+ userId + " dec=" + dec, e);
		}
	}

	public void setOn(int userId, ScoreSignType type) {
		try {
			getScoreLoaderOneway(userId).setOn(userId, type.type);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setOn " + " userId=" + userId
							+ " type=" + type.name()
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setOn " + " userId=" + userId
							+ " type=" + type.name() + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setOn " + " userId=" + userId
							+ " type=" + type.name(), e);
		}
	}

	public void setOff(int userId, ScoreSignType type) {
		try {
			getScoreLoaderOneway(userId).setOff(userId, type.type);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setOff " + " userId="
							+ userId + " type=" + type.name()
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setOff " + " userId="
							+ userId + " type=" + type.name()
							+ " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setOff " + " userId="
							+ userId + " type=" + type.name(), e);
		}
	}

	public void setNewlyUpgrade(int userId,boolean on) {
		if( on ){
			try {
				getScoreLoaderOneway(userId).setOn(userId, 0);
			} catch (ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOn " + " userId=" + userId
						+ " type=" + 0
						+ " Ice.ConnectTimeoutException");
			} catch (TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOn " + " userId=" + userId
						+ " type=" + 0 + " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOn " + " userId=" + userId
						+ " type=" + 0, e);
			}
		} else{
			try {
				getScoreLoaderOneway(userId).setOff(userId, 0);
			} catch (ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOff " + " userId=" + userId
						+ " type=" + 0
						+ " Ice.ConnectTimeoutException");
			} catch (TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOff " + " userId=" + userId
						+ " type=" + 0 + " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOff " + " userId=" + userId
						+ " type=" + 0, e);
			}
		}
	}

	public void setDoubleScore(int userId,boolean on) {
		if( on ){
			try {
				getScoreLoaderOneway(userId).setOn(userId, 62);
			} catch (ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOn " + " userId=" + userId
						+ " type=" + 62
						+ " Ice.ConnectTimeoutException");
			} catch (TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOn " + " userId=" + userId
						+ " type=" + 62 + " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOn " + " userId=" + userId
						+ " type=" + 62, e);
			}
		} else{
			try {
				getScoreLoaderOneway(userId).setOff(userId, 62);
			} catch (ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOff " + " userId=" + userId
						+ " type=" + 62
						+ " Ice.ConnectTimeoutException");
			} catch (TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOff " + " userId=" + userId
						+ " type=" + 62 + " Ice.TimeoutException");
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".setOff " + " userId=" + userId
						+ " type=" + 62, e);
			}
		}
	}

	public void setMask(long mask) {
		try {
			getScoreLoaderOneway(0).setMask(mask);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setMask " + " mask=" + mask
							+ " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setMask " + " mask=" + mask
							+ " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setMask " + " mask=" + mask,
					e);
		}
	}

	public void reload(int userId) {
		try {
			getScoreLoaderOneway(userId).reload(userId);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".reload " + " userId="
							+ userId + " Ice.ConnectTimeoutException");
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".reload " + " userId="
							+ userId + " Ice.TimeoutException");
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".reload " + " userId="
							+ userId, e);
		}
	}

	protected ScoreLoaderNPrx getScoreLoaderOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			ScoreLoaderNPrx prx = ScoreLoaderNPrxHelper.uncheckedCast(prx0
					.ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	protected ScoreLoaderNPrx getScoreLoaderTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 3000);
		if (prx0 != null) {
			ScoreLoaderNPrx prx = ScoreLoaderNPrxHelper
					.uncheckedCast(prx0.ice_timeout(3000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
