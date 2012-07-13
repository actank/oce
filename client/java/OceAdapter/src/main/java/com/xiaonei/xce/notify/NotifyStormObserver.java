package com.xiaonei.xce.notify;

import java.util.Map;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.storm.XceStormAdapter;
import com.xiaonei.xce.storm.XceStormTask;

import xce._notify.Observer;
import xce._notify.ObserverPrx;
import xce._notify.ObserverPrxHelper;


public abstract class NotifyStormObserver  extends Observer  {
	private ObserverPrx proxy;

	public abstract void handle(Map<String, String> context);

	public ObserverPrx register(Ice.ObjectAdapter adapter) {
		proxy = ObserverPrxHelper.uncheckedCast(adapter.addWithUUID(this).ice_timeout(300));
		return proxy;
	}

	public ObserverPrx deregister(Ice.ObjectAdapter adapter) {
		adapter.remove(proxy.ice_getIdentity());
		return proxy;
	}

	public ObserverPrx getProxy() {
		return proxy;
	}


	public boolean update(Map<String, String> context, Ice.Current current) {
		try {
			NotifyStormAdapter.getInstance().getThreadPoolExecutor().execute(new NotifyStormTask(this, context));
		} catch (Exception e){
			Oce.getLogger().error(this.getClass().getName() + " " + e);
		}
		
		return true;
	}
}
