package com.xiaonei.xce.notify;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.ArrayBlockingQueue;

import java.util.List;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import java.util.Map.Entry;

import com.xiaonei.service.util.descriptor.SvcDescriptorAdapter;

import xce._notify.ObserverPrx;
import xce._notify.NotifyStorm;
import xce._notify.NotifyStormPrx;
import xce._notify.NotifyStormPrxHelper;

import xce.util.channel.Channel;
import Ice.ObjectAdapter;
import java.util.concurrent.ThreadPoolExecutor;

import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;

import Ice.ObjectPrx;

//import xce.clusterstate.ReplicatedClusterAdapter;

public class NotifyStormAdapter extends
		AbstractModProxyHolder<Integer, NotifyStormPrx> {

	private static NotifyStormAdapter instance;
	private static String endpoints = "@NotifyStorm";
	private Map<String, ArrayList<NotifyStormObserver>> observers = new HashMap<String, ArrayList<NotifyStormObserver>>();

	protected ObjectAdapter adapter;

	private NotifyStormAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY), "M" + endpoints, 1);
		String uuid = UUID.randomUUID().toString();
		_channel.getCommunicator().getProperties().setProperty(
				uuid + ".Endpoints", "tcp");
		adapter = _channel.getCommunicator().createObjectAdapter(uuid);
		// adapter =
		// ChannelFactory.getChannel(ChannelType.NOTIFY).getCommunicator().createObjectAdapter(uuid);
		adapter.activate();
		pool = new ThreadPoolExecutor(1, 10, 10, TimeUnit.SECONDS,
				new ArrayBlockingQueue<Runnable>(1000),
				new ThreadPoolExecutor.AbortPolicy());
		new Timer().schedule(new SubscribeTimer(), 1 * 1000, 30 * 1000);
	}

	public static NotifyStormAdapter getInstance() {
		if (instance == null) {
			instance = new NotifyStormAdapter();
		}
		return instance;
	}

	private ThreadPoolExecutor pool;

	public ThreadPoolExecutor getThreadPoolExecutor() {
		return pool;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public NotifyStormPrx uncheckedCast(ObjectPrx baseProxy) {
		return NotifyStormPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private NotifyStormPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private NotifyStormPrx getManager(int uid) {
		return getProxy(uid);
	}

	public void subscribe(String name, NotifyStormObserver observer) {
		if (null == name) {
			throw new NullPointerException("the name is null");
		}
		boolean found = false;
		synchronized (this) {
			ArrayList<NotifyStormObserver> list = observers.get(name);
			if (list == null) {
				list = new ArrayList<NotifyStormObserver>();
				observers.put(name, list);
			}
			if (!list.contains(observer)) {
				list.add(observer);
			} else {
				found = true;
			}
		}
		if (!found) {
			// System.out.println("not found");
			getManagerOneway(0).subscribe(name, observer.register(adapter));
		} else {
			// System.out.println("found");
			getManagerOneway(0).subscribe(name, observer.getProxy());

		}
	}

	public void unsubscribe(String name, NotifyStormObserver observer) {
		boolean found = false;
		synchronized (this) {
			ArrayList<NotifyStormObserver> list = observers.get(name);
			if (list != null && list.contains(observer)) {
				found = true;
				list.remove(observer);
			}
		}
		if (found) {
			getManagerOneway(0).unsubscribe(name, observer.deregister(adapter));
		}
	}

	public void notify(String name, Map<String, String> props) {
		getManagerOneway(0)._notify(name, props);
	}

	public Map<String, ObserverPrx[]> query(String name) {
		return getManager(0).query(name);
	}

	public void subscribeAgain() {
		Map<String, ArrayList<NotifyStormObserver>> os;
		synchronized (this) {
			os = observers;
		}
		for (Entry<String, ArrayList<NotifyStormObserver>> entry : os
				.entrySet()) {
			for (NotifyStormObserver observer : entry.getValue()) {
				try {
					subscribe(entry.getKey(), observer);
				} catch (Exception e) {
					// do nothing, just try next schedule
				}
			}
		}
	}

	// --------------------------------------------------------------------

	public static void main(String[] args) throws Exception {
		System.out.println("start testing ...");
		NotifyStormObserver observer = new NotifyStormObserverExample();
//		NotifyStormAdapter.getInstance().subscribe("APNSTEST", observer);
		NotifyStormAdapter.getInstance().subscribe("PLATFORM_REPLY", observer);


//		Map<String, ObserverPrx[]> pool = NotifyStormAdapter.getInstance().query("APNSTEST");
		Map<String, ObserverPrx[]> pool = NotifyStormAdapter.getInstance().query("PLATFORM_REPLY");
		for (ObserverPrx o : pool.get("Test")) {
			System.out.println("AAA:"+o);
		}
		
		while(true){
			Thread.sleep(1* 1000);
		}
		
//		while (true) {
//			Map<String, String> properties = new HashMap<String, String>();
//			properties.put("key", new Date().toString());
//			NotifyStormAdapter.getInstance().notify("Test", properties);
//			Thread.sleep(1 * 1000);
//		}
		
//		Thread.sleep(10 * 1000);
//		NotifyStormAdapter.getInstance().unsubscribe("Test", observer);
//		System.out.println("unsubscribe");
//		while (true) {
//			Thread.sleep(1 * 1000);
//		}
	}
}

class SubscribeTimer extends TimerTask {
	public void run() {
		NotifyStormAdapter.getInstance().subscribeAgain();
	}
}

class NotifyStormObserverExample extends NotifyStormObserver {
	public void handle(Map<String, String> properties) {
		System.out.println(properties);
	}
}
