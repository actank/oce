package com.xiaonei.xce.log;

import org.apache.commons.logging.Log;

public class TimeCost {
	private static org.apache.commons.logging.Log remoteLog = System.getProperty("REMOTE_TIMECOST") != null ? null
			: org.apache.commons.logging.LogFactory.getLog(TimeCost.class);
	private final long beginTime = System.currentTimeMillis();
	private final Object name;
	private long stepTime;
	private final Log log;
	private boolean alreadyEnd = false;

	public static TimeCost begin(Object name) {
		return new TimeCost(XceStat.getLogger(), String.valueOf(name));
	}

	public static TimeCost begin(Log log, Object name) {
		return new TimeCost(log, String.valueOf(name));
	}

	private TimeCost(Log log, String name) {
		this.stepTime = this.beginTime;
		this.log = log;
		this.name = name;
	}

	public void endStep(String stepName) {
		if (alreadyEnd)
			return;
		String message = name + " - Step " + stepName + " - Cost: " + (System.currentTimeMillis() - stepTime);
		stepTime = System.currentTimeMillis();
		if (remoteLog != null) {
			remoteLog.info(message);
		}
		log.info(message);
	}

	public void endStep(String stepName, Throwable e) {
		if (alreadyEnd) {
			return;
		}
		String message = name + " - " + stepName + " - Cost: " + (System.currentTimeMillis() - stepTime);
		stepTime = System.currentTimeMillis();
		if (remoteLog != null) {
			remoteLog.info(message);
		}
		log.info(message, e);
	}

	public void endException(Throwable e) {
		if (alreadyEnd) {
			return;
		}
		if (remoteLog != null) {
			remoteLog.warn(name, e);
		}
		log.warn(name, e);
	}

	public void endFinally() {
		if (alreadyEnd) {
			return;
		}
		alreadyEnd = true;
		String message = name + " - Cost: " + (System.currentTimeMillis() - beginTime);
		if (remoteLog != null) {
			remoteLog.info(message);
		}
		log.info(message);
	}

	@Override
	protected void finalize() throws Throwable {
		if (!alreadyEnd) {
			String message = "Finialize an TimeCost object not ended. TimeCost should alwalys be ended in finally block. " + name;
			if (remoteLog != null) {
				remoteLog.info(message);
			}
			log.warn(message);
		}
	}
}
