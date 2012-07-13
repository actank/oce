package com.xiaonei.xce.storm;

import java.util.Map;


public class XceStormTask implements Runnable{
	private Map<String, String> context;
	private XceStormListener listener;

	public XceStormTask(XceStormListener listener, Map<String, String> context) {
		this.listener=listener;
		this.context=context;
	}
	@Override
	public void run() {
		listener.handle(context);
	}

}
