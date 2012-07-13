package com.renren.messagepipe.consumer;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.jms.Message;

import com.renren.messagepipe.common.BrokerConnection;
import com.renren.messagepipe.common.ClusterManager;
import com.renren.messagepipe.producer.MessageData;
import com.renren.messagepipe.producer.Producer;

public abstract class Consumer extends Thread {

	private BrokerConnection brokerConnection;
	private String cluster;
	private String subject;
	private int maxDelay;

	static public final int MAX_DELAY = 1000;

	public Consumer(String cluster, String subject) {
		this(cluster, subject, MAX_DELAY);
		this.start();
	}

	public Consumer(String cluster, String subject, int maxDelay) {
		assert (maxDelay > 0);
		this.maxDelay = maxDelay;
		this.cluster = cluster;
		this.subject = subject;
		brokerConnection = ClusterManager.getConnection(cluster);
	}

	public abstract List<MessageData> onMessage(
			List<MessageData> receiveMessages);

	public void run() {
		while (true) {
			try {
				List<BytesMessage> buffers = brokerConnection.receive(subject);
				List<MessageData> datas = new ArrayList<MessageData>(
						buffers.size());
				for (BytesMessage bm : buffers) {
					try {
						byte[] body = new byte[(int) bm.getBodyLength()];
						bm.readBytes(body);
						MessageData data = new MessageData(cluster, subject,
								body);
						datas.add(data);
					} catch (JMSException e) {
					}
				}
				if (!datas.isEmpty()) {
					List<MessageData> failMessage = onMessage(datas);
					if (failMessage != null && !failMessage.isEmpty()) {
						Producer.getInstance().send(failMessage);
					}
				}
				if (!buffers.isEmpty()) {
					try {
						buffers.get(buffers.size() - 1).acknowledge();
					} catch (JMSException e) {
					}
				}
				Thread.sleep(maxDelay);
			} catch (InterruptedException e) {
			}
		}
	}
}
