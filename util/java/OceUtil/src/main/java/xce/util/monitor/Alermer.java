package xce.util.monitor;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.LinkedBlockingDeque;

import xce.monitor.XiaoNeiMonitorPrx;
import xce.monitor.XiaoNeiMonitorPrxHelper;
import xce.util.storm.Topic;
import xce.util.storm.TopicManager;

public class Alermer {
	public static final String CONNECTTIMEOUTEXCEPTION = "ConnectTimeoutException";

	public static final String TIMEOUTEXCEPTION = "TimeoutException";

	public static final String SQLEXCEPTION = "SQLException";

	public static final String EXCEPTION = "Exception";

	private static Alermer alermer = new Alermer();

	private AlermThread _thread = new AlermThread();

	private LinkedBlockingDeque<List<String>> alermmaps = new LinkedBlockingDeque<List<String>>(
			1000);

	public static Alermer getInstance() {
		return alermer;
	}

	private Alermer() {
		_thread.start();
	}

	public void report(String name, String type) {
		/*
		 * XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrxHelper.uncheckedCast(new
		 * Topic(TopicManager.getInstance("XceMonitorStorm"),
		 * type).getPublisher()); Map<String, String> content = new HashMap<String,
		 * String>(); content.put("name", name); monitor.report(content);
		 */
		report(name, type, "");
	}
	
	private String processExceptionMessage(String message){
		StringBuffer buffer=new StringBuffer();
		String[] s=message.split(" ");
		for (int i=0;i<s.length;i++){
			if(s[i].contains("Exception")){
				continue;
			}
			buffer.append(s[i]);
			buffer.append(" ");
		}
		return buffer.toString();
	}

	public void report(String name, String type, String message) {
		List<String> content = new ArrayList<String>();
		content.add(name);
		content.add(type);
		content.add(processExceptionMessage(message));
		alermmaps.offerFirst(content);
	}

	private class AlermThread extends Thread {
		public void run() {
		}
	}

}
