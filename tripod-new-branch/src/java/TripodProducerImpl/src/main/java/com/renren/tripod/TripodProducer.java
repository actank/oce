package com.renren.tripod;

import java.util.Map;

public abstract class TripodProducer {
	public abstract Map<String, String> create(String[] keys, boolean isMaster);
	public abstract String[] createKeys(String beginKey, int limit);
	public abstract String[] createHotKeys(String beginKey, int limit);
}
