package com.xiaonei.xce.storm;

import java.util.Map;

import com.xiaonei.xce.log.Oce;

import xce.storm.Observer;
import xce.storm.ObserverPrx;
import xce.storm.ObserverPrxHelper;


public abstract class XceStormListener extends Observer {
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
			XceStormAdapter.getInstance().getThreadPoolExecutor().execute(new XceStormTask(this, context));
		} catch (Exception e){
			Oce.getLogger().error(this.getClass().getName() + " " + e);
		}
		return true;
	}
}
