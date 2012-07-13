package xce.util.monitor;

import java.util.HashMap;
import java.util.Map;

import xce.util.storm.Topic;
import xce.util.storm.TopicManager;
import xce.monitor.XiaoNeiMonitorPrx;
import xce.monitor.XiaoNeiMonitorPrxHelper;

public class XiaoNeiMonitor {

	private static XiaoNeiMonitorPrx _timeoutPrx;
	static {
		_timeoutPrx = XiaoNeiMonitorPrxHelper.uncheckedCast(new Topic(
				TopicManager.getInstance("XceMonitorStorm"), "Count")
				.getPublisher());
		long time = System.currentTimeMillis();
		
	}

	public static void reportTimeout(String name, String message) {
		Map<String, String> data = new HashMap<String, String>();
		data.put("name", name);
		_timeoutPrx.report(data);
	}

	public static void main(String[] args) {
		for (int i = 1; i < 1000; ++i) {
			reportTimeout("FFF", "");
		}
		System.exit(0);
	}
}
