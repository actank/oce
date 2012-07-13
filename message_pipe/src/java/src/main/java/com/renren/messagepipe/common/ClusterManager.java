package com.renren.messagepipe.common;

import java.util.Map;

import org.apache.log4j.Logger;

import com.google.common.base.Function;
import com.google.common.collect.MapMaker;

public class ClusterManager {

	private Logger logger = Logger.getLogger(ClusterManager.class);

	@SuppressWarnings("deprecation")
	private static Map<String, BrokerConnection> brokerConnections = new MapMaker()
			.concurrencyLevel(32).makeComputingMap(
					new Function<String, BrokerConnection>() {

						@Override
						public BrokerConnection apply(String cluster) {
							try {
								BrokerConnection brokerConnection = new BrokerConnection(
										cluster);
								ZKManager.subscribeClusterChanges(cluster,
										brokerConnection);
								return brokerConnection;
							} catch (Throwable e) {
								System.out.println("ClusterPool init cluster:"
										+ cluster + " fail " + e);
								return null;
							}
						}
					});

	public static BrokerConnection getConnection(String cluster) {
		return brokerConnections.get(cluster);
	}
}
