package com.xiaonei.xce.notify;

import java.util.Map;


public class NotifyStormTask implements Runnable{
	private Map<String, String> context;
	private NotifyStormObserver observer;

	public NotifyStormTask(NotifyStormObserver observer, Map<String, String> context) {
		this.observer=observer;
		this.context=context;
	}
	@Override
	public void run() {
		observer.handle(context);
	}

}
