package com.renren.messagepipe.common;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;

import com.renren.messageconfig.MessageConfigException;
import com.renren.messageconfig.NamespaceListener;
import com.renren.messageconfig.ZkClient;

public class ZKManager {

	public String getZKClusterPath(String clusterName) {
		return "/" + clusterName;
	}

	static final String zkAddress = "10.22.202.59:2181";

	public static void subscribeClusterChanges(String cluster,
			NamespaceListener listener) throws MessageConfigException {

		// read from /cluster to get AllBrokers
		// client restart when i change child?
		ZkClient.getInstance(zkAddress).addNamespaceListener(listener);
	}

	public static Broker getZKBrokerInfo(String cluster, String brokerInfo) {
		// Splitter.on('|').trimResults().omitEmptyStrings().split(brokerInfo);
		// String nodeInfo = zk.readData("/" + cluster + "/" + nodeId);
		String[] infos = brokerInfo.split("\\|");
		assert (infos.length == 3);

		String host = infos[0];
		int port = 61616;
		try {
			port = Integer.valueOf(infos[1]);
		} catch (Throwable e) {
			System.out.println("parse error " + e);
		}
		String status = infos[2];

		return new Broker(cluster, host, port, status);
	}

	public static List<Broker> getZKBrokers(String cluster)
			throws MessageConfigException {
		List<Broker> brokers = new ArrayList<Broker>();
		List<String> childs;
		childs = ZkClient.getInstance(zkAddress).getNodes(cluster);
		for (String child : childs) {
			brokers.add(getZKBrokerInfo(cluster, child));
		}

		return brokers;
	}
}
