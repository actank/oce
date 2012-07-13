package xce.util.service;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import xce.util.channel.Channel;
import Ice.ObjectPrx;

public abstract class ReplicatedServiceAdapter extends ServiceAdapter {
  
  public static class XceLog {
    private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
        .getLog("xceLog");

    public static org.apache.commons.logging.Log getLogger() {
      return _logger;
    }

  }
  
	private final Timer _timer = new Timer(true);
	protected IceGrid.QueryPrx query = IceGrid.QueryPrxHelper
			.uncheckedCast(channel.getCommunicator().stringToProxy(
					"XiaoNei/Query"));
	protected Map<ObjectPrx, Map<Boolean, List<ObjectPrx>>> replicatedProxy = new ConcurrentHashMap<ObjectPrx, Map<Boolean, List<ObjectPrx>>>();

	protected abstract boolean verifyProxy(Ice.ObjectPrx proxy);

	protected ReplicatedServiceAdapter(String endpoints, int cluster,
			Channel channel,String q) {
		super(endpoints, cluster, channel);
		query = IceGrid.QueryPrxHelper
		.uncheckedCast(channel.getCommunicator().stringToProxy(
				q));
		_timer.schedule(new ValidateTask(), 10000, 10000);
	}

	@Override
	protected ObjectPrx locateProxy(Vector ts, String name, int id,
			Channel.Invoke iv, Class helper, int timeout) {
		ObjectPrx proxy = super.locateProxy(ts, name, id, iv, helper, timeout);
		return locateValidateProxy(proxy);
	}

	@Override
	protected ObjectPrx locateProxy(String name, Channel.Invoke iv,
			Class helper, int timeout) {
		ObjectPrx proxy = super.locateProxy(name, iv, helper, timeout);
		return locateValidateProxy(proxy);
	}

	private ObjectPrx locateValidateProxy(Ice.ObjectPrx proxy) {
		if (!replicatedProxy.containsKey(proxy)) {
			replicatedProxy.put(proxy, getReplicatedProxy(proxy));
		}
		Map<Boolean, List<ObjectPrx>> allReplica = replicatedProxy.get(proxy);
		List<ObjectPrx> resultList;
		if (allReplica.get(true).size() != 0) {
			resultList = allReplica.get(true);
		} else if (allReplica.get(false).size() != 0) {
			resultList = allReplica.get(false);
		} else {
			replicatedProxy.remove(proxy);
			return proxy;
		}
		return resultList.get((int) (Math.random() * resultList.size()));
	}

	class ValidateTask extends TimerTask {
		@Override
		public void run() {
			try {
				for (Entry<ObjectPrx, Map<Boolean, List<ObjectPrx>>> entry : replicatedProxy
						.entrySet()) {
					ObjectPrx key = entry.getKey();
					entry.setValue(getReplicatedProxy(key));
				}
			} catch (Exception e) {
				XceLog
						.getLogger()
						.warn(
								"ReplicatedServiceAdapter ValidateTask main loop exception.",
								e);
			}
		}
	}

	protected Map<Boolean, List<ObjectPrx>> getReplicatedProxy(
			Ice.ObjectPrx proxy) {
		Map<Boolean, List<Ice.ObjectPrx>> result = new HashMap<Boolean, List<ObjectPrx>>();
		result.put(true, new ArrayList<ObjectPrx>());
		result.put(false, new ArrayList<ObjectPrx>());
		Ice.ObjectPrx[] replica = query.findAllReplicas(proxy);
		for (Ice.ObjectPrx one : replica) {
			boolean isValid = false;
			try {
				isValid = verifyProxy(one);
			} catch (Throwable e) {
				XceLog.getLogger().warn(
						"verifyProxy exception " + proxy.ice_toString(), e);
			} finally {
				result.get(isValid).add(one);
				if (XceLog.getLogger().isInfoEnabled()) {
					XceLog.getLogger().info(
							one.ice_toString() + " = " + isValid);
				}
			}
		}
		return result;
	}
}
