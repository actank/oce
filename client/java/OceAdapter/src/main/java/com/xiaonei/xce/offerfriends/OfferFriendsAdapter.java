package com.xiaonei.xce.offerfriends;

import com.xiaonei.xce.log.Oce;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import xce.offerfriends.OfferDataN;
import xce.offerfriends.OfferFriendsCacheNManagerPrx;
import xce.offerfriends.OfferFriendsCacheNManagerPrxHelper;
import xce.offerfriends.OfferFriendsLoaderNPrx;
import xce.offerfriends.OfferFriendsLoaderNPrxHelper;

import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class OfferFriendsAdapter<T extends Offer> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerOfferFriendsCacheN";
	private static int _interval = 120;
	private final OfferFactory<T> _factory;

	public OfferFriendsAdapter(OfferFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	private static OfferFriendsAdapter<DefaultOffer> _instance = new OfferFriendsAdapter<DefaultOffer>(
			new DefaultOfferFactory());

	public static OfferFriendsAdapter<DefaultOffer> getInstance() {
		return _instance;
	}

	protected OfferFriendsCacheNManagerPrx getOfferFriendsCacheNManager(int userId){
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			OfferFriendsCacheNManagerPrx prx = OfferFriendsCacheNManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected OfferFriendsCacheNManagerPrx getOfferFriendsCacheNManagerLongTimeout(int userId){
		Ice.ObjectPrx prx0 = getValidProxy(userId, 1000);
		if ( prx0 != null ) {
			OfferFriendsCacheNManagerPrx prx = OfferFriendsCacheNManagerPrxHelper.uncheckedCast(prx0.ice_timeout(1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public List<T> getFastOffer(int userId, int limit) {
		OfferFriendsCacheNManagerPrx prx=null;		
		try {
			prx = getOfferFriendsCacheNManager(userId);
			List<T> result = new ArrayList<T>();
			OfferDataN[] offers = prx.getFastOffer(userId, limit);
			for(OfferDataN one : offers) {
				T obj = _factory.create();
				obj.setBase(one.userId, one.weight, one.sign);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			System.err.println("OfferFriends getFastOffer( " + userId + ") ["+ prx +"] Ice.ConnectTimeoutException");
		}catch (Ice.TimeoutException e) {
			System.err.println("OfferFriends getFastOffer(" + userId + ")  ["+ prx +"] Timeout");
		} catch (Exception e){
			e.printStackTrace();
		}
		return new ArrayList<T>();
	}
	
	public List<T> getBestOffer(int userId, int limit) {
		OfferFriendsCacheNManagerPrx prx=null;
		try {
			prx=getOfferFriendsCacheNManagerLongTimeout(userId);
			List<T> result = new ArrayList<T>();
			OfferDataN[] offers = prx.getBestOffer(userId, limit);
			for(OfferDataN one : offers) {
				T obj = _factory.create();
				obj.setBase(one.userId, one.weight, one.sign);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			System.err.println("OfferFriends getBestOffer( " + userId + ")  ["+ prx +"] Ice.ConnectTimeoutException");
		}catch (Ice.TimeoutException e) {
			System.err.println("OfferFriends getBestOffer(" + userId + ")  ["+ prx +"] Timeout");
		} catch (Exception e){
			e.printStackTrace();
		}
		return new ArrayList<T>();
	}
	
	public List<T> getBestOfferByStage(int stage, int userId, int limit) {
		OfferFriendsCacheNManagerPrx prx=null;
		try {
			prx = getOfferFriendsCacheNManagerLongTimeout(userId);
			List<T> result = new ArrayList<T>();
			OfferDataN[] offers = prx.getBestOfferByStage(stage, userId, limit);
			for(OfferDataN one : offers) {
				T obj = _factory.create();
				obj.setBase(one.userId, one.weight, one.sign);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			System.err.println("OfferFriends getBestOffer( " + userId + ")  ["+ prx +"] Ice.ConnectTimeoutException");
		}catch (Ice.TimeoutException e) {
			System.err.println("OfferFriends getBestOffer(" + userId + ")  ["+ prx +"] Timeout");
		} catch (Exception e){
			e.printStackTrace();
		}
		return new ArrayList<T>();
	}

	public List<T> getRandomOffer(int userId, int limit) {
		OfferFriendsCacheNManagerPrx prx=null;		
		try {
			prx = getOfferFriendsCacheNManager(userId);
			List<T> result = new ArrayList<T>();
			OfferDataN[] offers = prx.getRandomOffer(userId, limit);
			for(OfferDataN one : offers) {
				T obj = _factory.create();
				obj.setBase(one.userId, one.weight, one.sign);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			System.err.println("OfferFriends getRandomOffer( " + userId + ") ["+ prx +"] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			System.err.println("OfferFriends getRandomOffer(" + userId + ") ["+ prx +"] Timeout");
		} catch (Exception e){
			e.printStackTrace();
		}
		return new ArrayList<T>();
	}
	
	public void apply(int applicant, int acceptor) {
                OfferFriendsLoaderAdapter.getInstance().apply(applicant,acceptor);
	}
	
	public void accept(int acceptor, int applicant) {
                OfferFriendsLoaderAdapter.getInstance().accept(acceptor,applicant);
	}
	
	public void denyOffer(int hostId, int guestId) {
                OfferFriendsLoaderAdapter.getInstance().denyOffer(hostId,guestId);
	}
	
	public void ignoreOffers(int userId, int[] ids) {
                OfferFriendsLoaderAdapter.getInstance().ignoreOffers(userId,ids);
	}
	
        public void rebuild(int userId) {
                OfferFriendsLoaderAdapter.getInstance().rebuild(userId,true);
        }

        public void rebuildBatch(int[] userIds) {
                OfferFriendsLoaderAdapter.getInstance().rebuildBatch(userIds,true);
        }

	public String debug(int userId) {
                return OfferFriendsLoaderAdapter.getInstance().debug(userId);
	}
	
        public void reload(int userId) {
                OfferFriendsLoaderAdapter.getInstance().reload(userId);
        }

	public void reloadBatch(int[] userIds) {
                OfferFriendsLoaderAdapter.getInstance().reloadBatch(userIds);
	}

	
	public static void main(String[] args) {
//		int userId = 24496255;  // 叶湘
		int userId = 238111132;	// 张楠
//		int userId = 1331;  	// 翟禹 
//		int userId = 239737004;	// 蔡建山
//		int userId = 222677625;	// 高亚楠 
//		int userId = 249386140;	// 袁景瑞
//		int userId = 235715761;	// 蒋擎 
//		int userId = 202911262;	// 白伯纯
//		int userId = 85432256;	// 黄晶
//		int userId = 281264530;	// 于杨
//		int userId = 281018810;	// 田广超
//		int userId = 220840375;	// 谭博侃
//		int userId = 200000032;	// 庄洪斌
//		int userId = 233940269;	// 葛广鹤 
//		int userId = 200000053;	// 刘启荣 
//		int userId = 80578957;	// 曲浩
//		int userId = 68126;		// 赵宁
//		int userId = 241618718;	// 李园花
//		int userId = 222525090;	// 张函
//		int userId = 227366242;	// 邵军辉
//		int userId = 19875;		// 郭金钢
//		int userId = 190136739;	// 曾哥
		int limit = 100;
//		OfferFriendsAdapter<DefaultOffer> adapter = new OfferFriendsAdapter<DefaultOffer>(new DefaultOfferFactory());
//		List<DefaultOffer> result = OfferFriendsAdapter.getInstance().getBestOffer(userId, limit);
//		OfferFriendsAdapter.getInstance().rebuild(userId);
//		System.out.println("size : " + result.size());
		

        //		int stage = xce.offerfriends.HIGHSCHOOL.value;
        //		List<DefaultOffer> result = OfferFriendsAdapter.getInstance().getBestOfferByStage(stage, userId, limit);
        //		System.out.println("size : " + result.size());
		System.exit(0);
	}
}

class OfferFriendsLoaderAdapter extends ReplicatedClusterAdapter {
        private static final String ctr_endpoints = "ControllerOfferFriendsLoaderN";
        private static int _interval = 120;

        private OfferFriendsLoaderAdapter() {
                super(ctr_endpoints, _interval);
        }

        private static OfferFriendsLoaderAdapter _instance = new OfferFriendsLoaderAdapter();

        public static OfferFriendsLoaderAdapter getInstance() {
                return _instance;
        }

        protected OfferFriendsLoaderNPrx getOfferFriendsLoaderTwoway(int userId) {
                Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
                if (prx0 != null) {
                        OfferFriendsLoaderNPrx prx = OfferFriendsLoaderNPrxHelper.uncheckedCast(prx0
                                        .ice_timeout(300).ice_twoway());
                        return prx;
                } else {
                        return null;
                }
        }

        protected OfferFriendsLoaderNPrx getOfferFriendsLoaderOneway(int userId) {
                Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
                if (prx0 != null) {
                        OfferFriendsLoaderNPrx prx = OfferFriendsLoaderNPrxHelper.uncheckedCast(prx0
                                        .ice_timeout(300).ice_oneway());
                        return prx;
                } else {
                        return null;
                }
        }

	public void apply(int applicant, int acceptor) {
                try {
                        getOfferFriendsLoaderOneway(applicant).apply(applicant,acceptor);
                } catch (ConnectTimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".apply " + " applicant="
                                                        + applicant + " acceptor=" + acceptor + " Ice.ConnectTimeoutException");
                } catch (TimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".apply " + " applicant="
                                                        + applicant + " acceptor=" + acceptor + " Ice.TimeoutException");
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".apply " + " applicant="
                                                        + applicant + " acceptor=" + acceptor, e);
                }
	}
	
	public void accept(int acceptor, int applicant) {
                try {
                        getOfferFriendsLoaderOneway(acceptor).accept(acceptor,applicant);
                } catch (ConnectTimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".accept " + " acceptor="
                                                        + acceptor + " applicant=" + applicant + " Ice.ConnectTimeoutException");
                } catch (TimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".accept " + " acceptor="
                                                        + acceptor + " applicant=" + applicant + " Ice.TimeoutException");
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".accept " + " acceptor="
                                                        + acceptor + " applicant=" + applicant, e);
                }
	}
	
	public void denyOffer(int hostId, int guestId) {
                try {
                        getOfferFriendsLoaderOneway(hostId).denyOffer(hostId,guestId);
                } catch (ConnectTimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".denyOffer " + " hostId="
                                                        + hostId + " guestId=" + guestId + " Ice.ConnectTimeoutException");
                } catch (TimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".denyOffer " + " hostId="
                                                        + hostId + " guestId=" + guestId + " Ice.TimeoutException");
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".denyOffer " + " hostId="
                                                        + hostId + " guestId=" + guestId, e);
                }
	}
	
        public void ignoreOffers(int userId, int[] ids) {
                try {
                        getOfferFriendsLoaderOneway(userId).ignoreOffers(userId,ids);
                } catch (ConnectTimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".ignoreOffers " + " userId="
                                                        + userId + " Ice.ConnectTimeoutException");
                } catch (TimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".ignoreOffers " + " userId="
                                                        + userId + " Ice.TimeoutException");
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".ignoreOffers " + " userId="
                                                        + userId, e);
                }
        }
	
        public void rebuild(int userId,boolean now) {
                try {
                        getOfferFriendsLoaderOneway(userId).rebuild(userId,now);
                } catch (ConnectTimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".rebuild " + " userId="
                                                        + userId + " Ice.ConnectTimeoutException");
                } catch (TimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".rebuild " + " userId="
                                                        + userId + " Ice.TimeoutException");
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".rebuild " + " userId="
                                                        + userId, e);
                }
        }
	public void rebuildBatch(int[] userIds,boolean now) {
                if (userIds.length==0) {
                        XceStat.getLogger().warn(
                                        this.getClass().getName() + ".rebuildBatch "
                                                        + " idSize=0");
                        return;
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
                        try {
				OfferFriendsLoaderNPrx prx = getOfferFriendsLoaderOneway(i);
				if (prx != null) {
					prx.rebuildBatch(MathUtil.list2array(subUserIds),now);
				}
                        } catch (ConnectTimeoutException e) {
                                Oce.getLogger().error(
                                                this.getClass().getName() + ".rebuildBatch "
                                                                + " idSize=" + subUserIds.size()
                                                                + " Ice.ConnectTimeoutException");
                        } catch (TimeoutException e) {
                                Oce.getLogger().error(
                                                this.getClass().getName() + ".rebuildBatch "
                                                                + " idSize=" + subUserIds.size()
                                                                + " Ice.TimeoutException");
                        } catch (java.lang.Throwable e) {
                                Oce.getLogger().error(
                                                this.getClass().getName() + ".rebuildBatch "
                                                                + " idSize=" + subUserIds.size(), e);
                        }
                }
        }

	public String debug(int userId) {
		String res = new String();
		OfferFriendsLoaderNPrx prx= getOfferFriendsLoaderTwoway(userId);
                try {
                	prx=getOfferFriendsLoaderTwoway(userId);
                        res = prx.debug(userId);
                } catch (ConnectTimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".debug ["+ prx + "] userId="
                                                        + userId + " Ice.ConnectTimeoutException");
                } catch (TimeoutException e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".debug ["+ prx + "] userId="
                                                        + userId + " Ice.TimeoutException");
                } catch (java.lang.Throwable e) {
                        Oce.getLogger().error(
                                        this.getClass().getName() + ".debug " + " userId="
                                                        + userId, e);
                }
		return res;
	}
	
        public void reload(int userId) {
                try {
                        getOfferFriendsLoaderOneway(userId).reload(userId);
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
	public void reloadBatch(int[] userIds) {
                if (userIds.length==0) {
                        XceStat.getLogger().warn(
                                        this.getClass().getName() + ".reloadBatch "
                                                        + " idSize=0");
                        return;
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
                        try {
				OfferFriendsLoaderNPrx prx = getOfferFriendsLoaderOneway(i);
				if (prx != null) {
					prx.reloadBatch(MathUtil.list2array(subUserIds));
				}
                        } catch (ConnectTimeoutException e) {
                                Oce.getLogger().error(
                                                this.getClass().getName() + ".reloadBatch "
                                                                + " idSize=" + subUserIds.size()
                                                                + " Ice.ConnectTimeoutException");
                        } catch (TimeoutException e) {
                                Oce.getLogger().error(
                                                this.getClass().getName() + ".reloadBatch "
                                                                + " idSize=" + subUserIds.size()
                                                                + " Ice.TimeoutException");
                        } catch (java.lang.Throwable e) {
                                Oce.getLogger().error(
                                                this.getClass().getName() + ".reloadBatch "
                                                                + " idSize=" + subUserIds.size(), e);
                        }
                }
        }

}


