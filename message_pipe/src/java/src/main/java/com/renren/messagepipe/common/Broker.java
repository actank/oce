package com.renren.messagepipe.common;

import java.util.Collections;
import java.util.List;
import java.util.Map;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.jms.Message;

import com.google.common.base.Function;
import com.google.common.collect.MapMaker;
import com.renren.messagepipe.consumer.AsyncConsumer;
import com.renren.messagepipe.producer.MessageData;
import com.renren.messagepipe.producer.SyncProducer;

public class Broker {

	public String cluster;
	public String host;
	public int port;
	public String status;

	@SuppressWarnings("deprecation")
	private Map<String, SyncProducer> subject2Producer = new MapMaker()
			.concurrencyLevel(32).makeComputingMap(
					new Function<String, SyncProducer>() {

						@Override
						public SyncProducer apply(String subject) {
							SyncProducer producer = new SyncProducer(host,
									port, subject);
							try {
								producer.connect();
							} catch (JMSException e) {
								e.printStackTrace();
								return null;
							} catch (Throwable e) {
								e.printStackTrace();
								return null;
							}
							return producer;
						}
					});
	@SuppressWarnings("deprecation")
	private Map<String, AsyncConsumer> subject2Consumer = new MapMaker()
			.concurrencyLevel(32).makeComputingMap(
					new Function<String, AsyncConsumer>() {

						@Override
						public AsyncConsumer apply(String subject) {

							AsyncConsumer consumer = new AsyncConsumer(host,
									port, subject);
							try {
								consumer.connect();
							} catch (JMSException e) {
								e.printStackTrace();
								return null;
							} catch (Throwable e) {
								e.printStackTrace();
								return null;
							}
							return consumer;
						}
					});

	public boolean send(MessageData data) {
		try {
			SyncProducer producer = subject2Producer.get(data.getSubject());
			if (producer != null)
				producer.send(data);
			else
				return false;
		} catch (JMSException e) {
			System.out.println(e);
			return false;
		} catch (Throwable e) {
			System.out.println(e);
			return false;
		}
		return true;
	}

	public List<BytesMessage> receive(String subject) {
		try {
			AsyncConsumer consumer = subject2Consumer.get(subject);

			if (consumer != null)
				return consumer.receive();
		} catch (Throwable e) {
			System.out.println(e);
		}
		return Collections.emptyList();
	}

	public Broker(String cluster, String host, int port, String status) {
		this.cluster = cluster;
		this.host = host;
		this.port = port;
		this.status = status;
	}

	public boolean isValid() {
		return status.equals("ONLINE");
	}

	public String getZKString() {
		return "/" + cluster + "/" + host + ":" + port;
	}

	@Override
	public boolean equals(Object o) {
		if (o instanceof Broker) {
			Broker other = (Broker) o;
			return cluster.equals(other.cluster) && host.equals(other.host)
					&& port == other.port;
		}
		return false;
	}

	@Override
	public String toString() {
		return "cluster:" + cluster + " host:" + host + " port:" + port
				+ " status:" + status;
	}

}
