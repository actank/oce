package com.renren.messagepipe.common;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import javax.jms.BytesMessage;
import javax.jms.Message;

import com.renren.messageconfig.MessageConfigException;
import com.renren.messageconfig.NamespaceListener;
import com.renren.messagepipe.producer.MessageData;

public class BrokerConnection extends NamespaceListener {

	private static final int DEFAULT_MAX_TRY = 3;
	private List<Broker> allBrokers;
	private List<Broker> validBrokers = new ArrayList<Broker>();
	private static AtomicInteger allSeed = new AtomicInteger(0);
	private static AtomicInteger validSeed = new AtomicInteger(0);

	public BrokerConnection(String cluster) throws MessageConfigException {
		super(cluster);
		allBrokers = ZKManager.getZKBrokers(cluster);
		for (Broker broker : allBrokers) {
			if (broker.isValid())
				validBrokers.add(broker);
		}
	}

	public boolean send(MessageData data) {

		List<Broker> brokers = validBrokers;
		if (brokers.isEmpty()) {
			return false;
		}
		int maxTry = DEFAULT_MAX_TRY;
		int N = brokers.size();
		while (maxTry-- > 0) {
			int index = ((validSeed.getAndIncrement() % N) + N) % N;
			if (brokers.get(index).send(data)) {
//				System.out.println("sucess send data to " + index);
				return true;
			}
		}
		return false;
	}

	public List<BytesMessage> receive(String subject) {
		List<Broker> brokers = allBrokers;
		if (brokers.isEmpty()) {
			return Collections.emptyList();
		}
		int maxTry = DEFAULT_MAX_TRY;
		int N = brokers.size();
		while (maxTry-- > 0) {
			int index = ((allSeed.getAndIncrement() % N) + N) % N;
			List<BytesMessage> part = brokers.get(index).receive(subject);
			if (part != null && !part.isEmpty()) {
				return part;
			} else {
				continue;
			}
		}
		return Collections.emptyList();
	}

	void printCurrentNode() {
		System.out.println("----current----valid----brokers");
		for (Broker vBroker : validBrokers) {
			System.out.println(vBroker);
		}
		System.out.println("----current-----all-----brokers");
		for (Broker vBroker : allBrokers) {
			System.out.println(vBroker);
		}
		System.out.println("-------------------------------");
	}

	// @Override

	// if we shut down a machine ,we must change the status to offline
	// and delete to avoid some message no consumer

	public void handleChildChange(String cluster, List<String> currentChilds)
			throws Exception {
		List<Broker> curValidBrokers = new ArrayList<Broker>();
		List<Broker> curAllBrokers = new ArrayList<Broker>();

		printCurrentNode();

		for (String child : currentChilds) {
			Broker broker = ZKManager.getZKBrokerInfo(cluster, child);
			int index = allBrokers.indexOf(broker);
			if (index >= 0) {
				broker = allBrokers.get(index);
			}
			curAllBrokers.add(broker);
			if (broker.isValid()) {
				curValidBrokers.add(broker);
			}
		}
		allBrokers = curAllBrokers;
		validBrokers = curValidBrokers;
		printCurrentNode();
	}

	@Override
	public boolean update(List<String> childrenNameList) {
		try {
			handleChildChange(getNamespace(), childrenNameList);
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

}
