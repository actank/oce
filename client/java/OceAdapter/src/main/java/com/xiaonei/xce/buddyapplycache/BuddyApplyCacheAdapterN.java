package com.xiaonei.xce.buddyapplycache;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;

import xce.buddyapplycache.BuddyApplyCacheManagerNPrx;
import xce.buddyapplycache.BuddyApplyCacheManagerNPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import xce.clusterstate.ReplicatedClusterAdapter;

public final class BuddyApplyCacheAdapterN<T extends BuddyApplyCacheDataN, W extends BuddyApplyCacheWithTotalN<T>>
		extends ReplicatedClusterAdapter {


	private final BuddyApplyCacheFactoryN<T, W> _factory;
	private volatile Integer _hardLimit = null;
	private static final String ctr_endpoints = "ControllerBuddyApplyCacheN";
    private static int _interval = 120;
    
	public BuddyApplyCacheAdapterN(BuddyApplyCacheFactoryN<T, W> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public List<T> getBuddyApplyCache(int userId, int begin, int limit) {
		List<T> result = new ArrayList<T>();
		xce.buddyapplycache.BuddyApplyCacheDataN[] res = null;
		BuddyApplyCacheManagerNPrx prx=null;
		try {
			prx=getBuddyApplyCacheManager(userId);
			res = prx.getBuddyApplyCache(userId,
					begin, limit);
		}catch (Ice.ConnectTimeoutException e) {
                        Oce.getLogger().error(this.getClass().getName() + ".getBuddyApplyCache [ " + prx + " ] id=" + userId + " "
                                        + e);
		}catch (Ice.TimeoutException e) {
                	Oce.getLogger().error(this.getClass().getName() + ".getBuddyApplyCache [ " + prx + " ] id=" + userId + " "
					+ e);
		}catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getBuddyApplyCache " + userId + " "
					, e);
                }

		if (res == null)
			return result;

		for (xce.buddyapplycache.BuddyApplyCacheDataN one : res) {
			T obj = _factory.createBuddyApplyCacheDataN();
			obj.setBase(one);
			result.add(obj);
		}
		return result;
	}

	public W getBuddyApplyCacheWithTotal(int userId, int begin, int limit) {
		W result = _factory.createBuddyApplyCacheWithTotalN();
		BuddyApplyCacheManagerNPrx prx=null;
		try {
			prx=getBuddyApplyCacheManager(userId);
			xce.buddyapplycache.BuddyApplyCacheWithTotalN res = prx.getBuddyApplyCacheWithTotal(userId, begin, limit);
			if (res == null)
				return result;
			result.setBase(res, _factory);
		}catch (Ice.ConnectTimeoutException e) {
                        Oce.getLogger().error(this.getClass().getName() + ".getBuddyApplyCacheWithTotal [ " + prx + " ] id=" + userId + " "
                                        + e);
                }catch (Ice.TimeoutException e) {
                	Oce.getLogger().error(this.getClass().getName() + ".getBuddyApplyCacheWithTotal [ " + prx + " ] id=" + userId + " "
					+ e);
		}catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getBuddyApplyCacheWithTotal " + userId + " "
					, e);
                }

		return result;
	}

	public void removeBuddyApplyCache(int userId, int applicant) {
		BuddyApplyCacheManagerNPrx prx=null;
		try {
			prx=getBuddyApplyCacheManager(userId);
			prx.removeBuddyApplyCache(userId,
					applicant);
		}catch (Ice.ConnectTimeoutException e) {
                    	Oce.getLogger().error( this.getClass().getName() + ".removeBuddyApplyCache [ " + prx + " ] id=" + userId + " "
                                    	+ e);
		}catch (Ice.TimeoutException e) {
				Oce.getLogger().error( this.getClass().getName() + ".removeBuddyApplyCache [ " + prx + " ] id=" + userId + " "
					+ e);
		}catch (java.lang.Throwable e) {
			Oce.getLogger().error( this.getClass().getName() + ".removeBuddyApplyCache " + userId + " "
					, e);
            	}
	}

	public void clear(int userId) {
		BuddyApplyCacheManagerNPrx prx=null;
		try {
			prx=getBuddyApplyCacheManager(userId);
			prx.clear(userId);
		}catch (Ice.ConnectTimeoutException e) {
                            Oce.getLogger().error(this.getClass().getName() + ".clear [ " + prx + " ] id=" + userId + " "
                                            + e);
                    }catch (Ice.TimeoutException e) {
                    	Oce.getLogger().error(this.getClass().getName() + ".clear [ " + prx + " ] id=" + userId + " "
					+ e);
		}catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + " clear " + userId + " "
					, e);
                    }
	}

	public void rebuildBuddyApplyCache(int userId) {
		BuddyApplyCacheManagerNPrx prx=null;
		try {
			prx=getBuddyApplyCacheManager(userId);			
			prx.rebuildBuddyApplyCache(userId);
		}catch (Ice.ConnectTimeoutException e) {
                            Oce.getLogger().error(this.getClass().getName() + ".rebuildBuddyApplyCache [ " + prx + " ] id=" + userId + " "
                                            + e);
                    }catch (Ice.TimeoutException e) {
                    	Oce.getLogger().error(this.getClass().getName() + ".rebuildBuddyApplyCache [ " + prx + " ] id=" + userId + " " 
					+ e);
		}catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".rebuildBuddyApplyCache " + userId + " "
					, e);
                    }
	}

	public int getHardLimit() {
		if (_hardLimit == null) {
			synchronized (_hardLimit) {
				if (_hardLimit == null) {				
					try {
						_hardLimit = getBuddyApplyCacheManager(0)
								.getHardLimit();
					} catch (Throwable e) {
						Oce.getLogger().error("get hardLimit failed");
					}
				}
			}
		}
		return _hardLimit;
	}

	public void reload(int userId) {
		BuddyApplyCacheManagerNPrx prx=null;
		try {
			prx=getBuddyApplyCacheManager(userId);
            prx.reload(userId);
		}catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".reload [ " + prx + " ] id=" + userId + " "
                            + e);
		}catch (Ice.TimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".reload [ " + prx + " ] id=" + userId + " "
                            + e);
		}catch (java.lang.Throwable e) {
            Oce.getLogger().error( this.getClass().getName() + ".reload " + userId + " "
            		, e);
		}
	}

	private Vector<BuddyApplyCacheManagerNPrx> managers = new Vector<BuddyApplyCacheManagerNPrx>();
	private Vector<BuddyApplyCacheManagerNPrx> managersOneway = new Vector<BuddyApplyCacheManagerNPrx>();

	protected BuddyApplyCacheManagerNPrx getBuddyApplyCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
        		BuddyApplyCacheManagerNPrx prx = BuddyApplyCacheManagerNPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
                return prx;
        } else {
                return null;
        }
	}

	protected BuddyApplyCacheManagerNPrx getBuddyApplyCacheManagerOneway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
        		BuddyApplyCacheManagerNPrx prx = BuddyApplyCacheManagerNPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
                return prx;
        } else {
                return null;
        }
	}

	
}
