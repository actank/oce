package com.xiaonei.xce.userbasic;

import java.util.Vector;

import xce.userbase.UserBasicManagerPrx;
import xce.userbase.UserBasicManagerPrxHelper;
import Ice.TimeoutException;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

public class UserBasicAdapter<T extends UserBasicInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserBase";

	private static final String service_name = "UBM";
	
	private static int _interval = 120;
	
	protected UserBasicFactory<T> _factory;

  private Vector<UserBasicManagerPrx> managersOneway = new Vector<UserBasicManagerPrx>();

	public UserBasicAdapter(UserBasicFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

  public T getUserBasic(int userId) throws Exception {
    TimeCost cost = TimeCost.begin("UserBasicAdapter.getUserBasic id: "
        + userId);

    UserBasicManagerPrx prx = null;
    T res = _factory.create();

    try {
	if (userId<=0) {
		return res;
	}
	 prx = getUserBasicManager(userId) ;
      res.parse(prx.getUserBasic(userId));
      return res;
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserBasic [ " + prx + " ]id=" + userId
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

	protected UserBasicManagerPrx getUserBasic10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserBasicManagerPrx prx = UserBasicManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserBasicManagerPrx getUserBasicManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserBasicManagerPrx prx = UserBasicManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserBasicManagerPrx getUserBasicManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserBasicManagerPrx prx = UserBasicManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

}


