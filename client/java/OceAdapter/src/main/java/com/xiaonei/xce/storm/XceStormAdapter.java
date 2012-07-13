package com.xiaonei.xce.storm;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.ArrayBlockingQueue;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import java.util.Map.Entry;

import com.xiaonei.service.util.descriptor.SvcDescriptorAdapter;

import xce.storm.ObserverPrx;
import xce.storm.TopicManagerPrx;
import xce.storm.TopicManagerPrxHelper;
import xce.util.channel.Channel;
import Ice.ObjectAdapter;
import java.util.concurrent.ThreadPoolExecutor;

import xce.clusterstate.ReplicatedClusterAdapter;

public class XceStormAdapter extends TimerTask {

	private class XceStormPrxHolder extends ReplicatedClusterAdapter {
		
		private static final String ctr_endpoints = "ControllerXceStorm";

		private static final int _interval = 120;

		public XceStormPrxHolder() {
			super(ctr_endpoints, _interval);
		}
	
		public TopicManagerPrx getTopicManager() {
			Ice.ObjectPrx prx0 = getValidProxy(0, 300);
			if (prx0 != null) {
				TopicManagerPrx prx = TopicManagerPrxHelper
						.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
				return prx;
			} else {
				return null;
			}
		}
	
		public TopicManagerPrx getTopicManagerOneway() {
			Ice.ObjectPrx prx0 = getValidProxy(0, 300);
			if (prx0 != null) {
				TopicManagerPrx prx = TopicManagerPrxHelper
						.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
				return prx;
			} else {
				return null;
			}
		}
	
	}

	protected Channel channel;

	protected ObjectAdapter adapter;
	
	protected XceStormPrxHolder prxHolder = new XceStormPrxHolder();

	private Map<String, ArrayList<XceStormListener>> listeners = new HashMap<String, ArrayList<XceStormListener>>();

	private static XceStormAdapter instance;

	private ThreadPoolExecutor pool;

	public ThreadPoolExecutor getThreadPoolExecutor() {
		return pool;
	}

	public static XceStormAdapter getInstance() {
		if (instance == null) {
			synchronized (XceStormAdapter.class) {
				if (instance == null) {
					instance = new XceStormAdapter();
				}
			}
		}
		return instance;
	}

	private XceStormAdapter() {
		pool = new ThreadPoolExecutor(1, 5, 10, TimeUnit.SECONDS, new ArrayBlockingQueue<Runnable>(1000), new ThreadPoolExecutor.AbortPolicy());
		channel = Channel.newOceChannel("@XceStorm");
		String uuid = UUID.randomUUID().toString();
		channel.getProperties().setProperty(uuid + ".Endpoints", "tcp");
		channel.getProperties().setProperty(uuid + ".ThreadPool.Size", "1");
		channel.getProperties().setProperty(uuid + ".ThreadPool.SizeMax", "1000");
		channel.getProperties().setProperty(uuid + ".ThreadPool.StackSize", "131072");
		adapter = channel.getCommunicator().createObjectAdapter(uuid);
		adapter.activate();

		new Timer().schedule(this, 30 * 60 * 1000, 30 * 60 * 1000);
	}

	public void destroy() {
		adapter.destroy();
	}

	public void subscribe(String name, XceStormListener listener) {
		if (null == name) {
			throw new NullPointerException("the name is null");
		}
		boolean found = false;
		synchronized (this) {
			ArrayList<XceStormListener> list = listeners.get(name);
			if (list == null) {
				list = new ArrayList<XceStormListener>();
				listeners.put(name, list);
			}
			if (!list.contains(listener)) {
				list.add(listener);
			} else {
				found = true;
			}
		}
		if (!found) {
			prxHolder.getTopicManagerOneway().subscribe(name, listener.register(adapter));
		} else {
			prxHolder.getTopicManagerOneway().subscribe(name, listener.getProxy());
		}
	}

	public void unsubscribe(String name, XceStormListener listener) {
		boolean found = false;
		synchronized (this) {
			ArrayList<XceStormListener> list = listeners.get(name);
			if (list != null && list.contains(listener)) {
				found = true;
				list.remove(listener);
			}
		}
		if (found) {
			prxHolder.getTopicManagerOneway().unsubscribe(name, listener.deregister(adapter));
		}
	}

	public void notify(String name, Map<String, String> props) {
		prxHolder.getTopicManagerOneway()._notify(name, props);
		// The following line should be removed if all notification through
		// SvcDescriptor
		// is replaced
		// SvcDescriptorAdapter.getInstance().notify(name, props);
	}

	public Map<String, ObserverPrx[]> query(String name) {
		return prxHolder.getTopicManager().query(name);
	}

	public void run() {
		for (Entry<String, ArrayList<XceStormListener>> entry : listeners
				.entrySet()) {
			for (XceStormListener listener : entry.getValue()) {
				try {
					subscribe(entry.getKey(), listener);
				} catch (Exception e) {
					// do nothing, just try next schedule
				}
			}
		}
	}

	public static void main(String[] args) throws Exception {
		System.out.println("start testing ...");
		XceStormListener listener = new XceStormListenerExample();
		XceStormAdapter.getInstance().subscribe("Test", listener);
		Map<String, ObserverPrx[]> pool = XceStormAdapter.getInstance().query(
				"PAGE_PROMOTE_STORM");
		for (ObserverPrx observer : pool.get("PAGE_PROMOTE_STORM")) {
			System.out.println(observer);
		}
		while (true) {
			Map<String, String> properties = new HashMap<String, String>();
			properties.put("key", new Date().toString());
			XceStormAdapter.getInstance().notify("Test", properties);

			Thread.sleep(1 * 1000);
		}
	}
}

class XceStormListenerExample extends XceStormListener {
	public void handle(Map<String, String> properties) {
		System.out.println(properties);
	}
}
