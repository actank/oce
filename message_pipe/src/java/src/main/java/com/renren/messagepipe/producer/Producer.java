package com.renren.messagepipe.producer;

import java.io.PrintStream;
import java.util.TimeZone;
import java.util.concurrent.Callable;
import java.util.concurrent.TimeUnit;

import com.google.inject.Guice;
import com.google.inject.Injector;
import com.renren.messagepipe.common.BrokerConnection;
import com.renren.messagepipe.common.ClusterManager;
import com.yammer.metrics.Metrics;
import com.yammer.metrics.core.MetricsRegistry;
import com.yammer.metrics.core.TimerMetric;
import com.yammer.metrics.guice.InstrumentationModule;
import com.yammer.metrics.guice.Timed;
import com.yammer.metrics.reporting.ConsoleReporter;
import com.yammer.metrics.util.MetricPredicate;

public class Producer {

	private static Producer instance = new Producer();
	private final TimerMetric timer = Metrics.newTimer(getClass(),
			"directory-listing", TimeUnit.MILLISECONDS, TimeUnit.SECONDS);
	private static MetricsRegistry registry;
	{
		// Injector injector = Guice.createInjector(new
		// InstrumentationModule());
		// instance = injector.getInstance(Producer.class);
		// registry = injector.getInstance(MetricsRegistry.class);
		// ConsoleReporter.enable(10, TimeUnit.SECONDS);
	}

	public static Producer getInstance() {
		return instance;
	}

	// @Timed
	public boolean send(final MessageData producerData) {
		final BrokerConnection brokerConnection = ClusterManager
				.getConnection(producerData.getCluster());
		if (brokerConnection != null) {
			return brokerConnection.send(producerData);
		} else {
			return false;
		}
	}

	public void send(java.util.List<MessageData> producerDatas) {
		for (MessageData data : producerDatas) {
			send(data);
		}
	}
}
