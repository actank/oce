package com.xiaonei.search.tools;

import java.lang.management.ManagementFactory;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.TimeUnit;

import javax.management.InstanceAlreadyExistsException;
import javax.management.MBeanServer;
import javax.management.ObjectName;
import javax.management.StandardMBean;

import org.apache.log4j.Logger;

import com.yammer.metrics.core.CounterMetric;
import com.yammer.metrics.core.GaugeMetric;
import com.yammer.metrics.core.HistogramMetric;
import com.yammer.metrics.core.MeterMetric;
import com.yammer.metrics.core.Metric;
import com.yammer.metrics.core.TimerMetric;
import com.yammer.metrics.reporting.JmxReporter.Counter;
import com.yammer.metrics.reporting.JmxReporter.CounterMBean;
import com.yammer.metrics.reporting.JmxReporter.Gauge;
import com.yammer.metrics.reporting.JmxReporter.GaugeMBean;
import com.yammer.metrics.reporting.JmxReporter.HistogramMBean;
import com.yammer.metrics.reporting.JmxReporter.Meter;
import com.yammer.metrics.reporting.JmxReporter.MeterMBean;
import com.yammer.metrics.reporting.JmxReporter.TimerMBean;

public class JmxUtil {

	private static final Logger log = Logger.getLogger(JmxUtil.class);
	private static final MBeanServer MbeanServer = ManagementFactory
			.getPlatformMBeanServer();
	private static final List<ObjectName> RegisteredBeans = Collections
			.synchronizedList(new LinkedList<ObjectName>());

	public static final String Domain = "com.xiaonei.xce";

	public static void registerMBean(StandardMBean bean, String key, String val) {
		ObjectName objectName = null;
		try {
			log.info("registering jmx mbean: " + objectName);
			objectName = new ObjectName(Domain, key, val);
			MbeanServer.registerMBean(bean, objectName);
			RegisteredBeans.add(objectName);
		} catch (Exception e) {
			log.error(e.getMessage(), e);
			if (e instanceof InstanceAlreadyExistsException) {
				RegisteredBeans.add(objectName);
			}
		}
	}

	private static void unregisterMBeans() {
		for (ObjectName mbeanName : RegisteredBeans) {
			try {
				log.info("unregistering jmx mbean: " + mbeanName);
				MbeanServer.unregisterMBean(mbeanName);
			} catch (Exception e) {
				log.error(e.getMessage(), e);
			}
		}
		RegisteredBeans.clear();
	}

	static {
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				unregisterMBeans();
			}
		});
	}

}
