package com.xiaonei.xce.userurl;

import java.util.Vector;

import xce.userbase.UserUrlData;
import xce.userbase.UserUrlManagerPrx;
import xce.userbase.UserUrlManagerPrxHelper;
import xce.util.tools.Warmupble;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import xce.clusterstate.ReplicatedClusterAdapter;
public final class UserUrlAdapter<T extends UserUrlInfo> extends ReplicatedClusterAdapter implements Warmupble {
	private static final String ctr_endpoints = "ControllerUserBase";

	private static final String service_name = "U";
	
	private static int _interval = 120;

	protected UserUrlFactory<T> _factory;

	public UserUrlAdapter(UserUrlFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}


  private Vector<UserUrlManagerPrx> managers = new Vector<UserUrlManagerPrx>();

	protected UserUrlManagerPrx getUserUrl10sManagerPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserUrlManagerPrx prx = UserUrlManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserUrlManagerPrx getUserUrlManagerPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			UserUrlManagerPrx prx = UserUrlManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserUrlManagerPrx getUserUrlManagerOnewayPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserUrlManagerPrx prx = UserUrlManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
  public T getUserUrl(int id) {
    TimeCost cost = TimeCost.begin("UserUrlAdapter.getUserUrl id: " + id);
    UserUrlManagerPrx prx = null;
    try {
		T result = _factory.create();
		if (id<=0) {
			return result;
		}
		prx = getUserUrlManagerPrx(id);
		UserUrlData data = prx .getUserUrl(id);
		result.parse(data);
		return result;
	} catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserUrl [" + prx + "] id=" + id
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

  public void setUserUrl(int id, UserUrlInfo obj) {
    TimeCost cost = TimeCost.begin("UserUrlAdapter.setUserUrl id: " + id);
    UserUrlManagerPrx prx = null;
    try {
	if (id<=0) {
		return;
	}
      if (obj.getId() != 0 && id != obj.getId()) {
        Oce.getLogger().error(
            "com.xiaonei.xce.userurl.UserUrlAdapter.setUserUrl id diff");
        obj.setId(id);
      }
      prx = getUserUrlManagerPrx(id);
      prx.setUserUrl(id, obj.demark());
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setUserUrl[ " + prx + "] id=" + id
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

  public void createUserUrl(int id, UserUrlInfo obj) {
    TimeCost cost = TimeCost.begin("UserUrlAdapter.createUserUrl id: " + id);
    UserUrlManagerPrx prx = null;
    try {
	if (id<=0) {
		return;
	}
      if (id != obj.getId()) {
        Oce.getLogger().error(
            "com.xiaonei.xce.userurl.UserUrlAdapter.createUserUrl id diff");
      }
      prx = getUserUrl10sManagerPrx(id);
      prx.createUserUrl(obj.demark());
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".createUserUrl[ " + prx + "] id=" + id
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

  public void reload(int id) {
    TimeCost cost = TimeCost.begin("UserUrlAdapter.getUserUrl id: " + id);
    UserUrlManagerPrx prx = null;
    try {
	if (id<=0) {
		return;
	}
	prx = getUserUrlManagerPrx(id);
      prx.reload(id);
    } finally {
      cost.endFinally();
    }
  }

  @Override
  public boolean warmup() {
    boolean success = true;
    for (Ice.ObjectPrx prx : managers) {
      try {
        prx.ice_ping();
      } catch (Throwable e) {
        success = false;
      }
    }
    return success;
  }
}
