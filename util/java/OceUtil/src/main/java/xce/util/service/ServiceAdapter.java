package xce.util.service;

import java.util.Vector;

import xce.util.channel.Channel;

public abstract class ServiceAdapter {

	protected Channel channel;
	
	protected Channel[] channels;

	protected String endpoints;

	protected int cluster;

	protected ServiceAdapter(String endpoints, int cluster, Channel channel) {
		this.endpoints = endpoints;
		this.cluster = cluster;
		this.channel = channel;
	}
	
	protected ServiceAdapter(String endpoints, int cluster, Channel[] channels) {
		this.endpoints = endpoints;
		this.cluster = cluster;
		this.channels = channels.clone();
	}

	@SuppressWarnings("unchecked")
	protected <T extends Ice.ObjectPrx> T locateProxy(Vector<T> ts,
			String name, int id, Channel.Invoke iv, Class helper, int timeout) {
		if (cluster <= 0) {
			if (1 > ts.size()) {
				synchronized (this) {
					if (1 > ts.size()) {
						ts.add((T) channel.locate(name, endpoints, iv, helper,
								timeout));
					}
				}
			}
			return (T) ts.get(0);
		} else {
			if (cluster > ts.size()) {
				synchronized (this) {
					if (cluster > ts.size()) {
						for (int i = ts.size(); i < cluster; ++i) {
							ts.add((T) channel.locate(name, endpoints + i, iv,
									helper, timeout));
						}
					}
				}
			}
			return ts.get(Math.abs(id) % cluster);
		}
	}

	@SuppressWarnings("unchecked")
	protected <T extends Ice.ObjectPrx> T locateProxy(Vector<T> ts,
			String name, int id, Channel.Invoke iv, Class helper, int timeout, int channelCount) {
		System.out.println("channels index: " + id%channelCount);
		if (cluster <= 0) {
			if (1 > ts.size()) {
				synchronized (this) {
					if (1 > ts.size()) {						
						ts.add((T) channels[id%channelCount].locate(name, endpoints, iv, helper,
								timeout));
					}
				}
			}
			return (T) ts.get(0);
		} else {
			if (cluster > ts.size()) {
				synchronized (this) {
					if (cluster > ts.size()) {
						for (int i = ts.size(); i < cluster; ++i) {
							ts.add((T) channels[id%channelCount].locate(name, endpoints + i, iv,
									helper, timeout));
						}
					}
				}
			}
			return ts.get(Math.abs(id) % cluster);
		}
	}
	
	@SuppressWarnings("unchecked")
	protected <T extends Ice.ObjectPrx> T locateProxy(String name,
			Channel.Invoke iv, Class helper, int timeout) {
		return (T) channel.locate(name, endpoints, iv, helper, timeout);
	}
}
