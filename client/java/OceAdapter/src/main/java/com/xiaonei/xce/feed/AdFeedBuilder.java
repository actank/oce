package com.xiaonei.xce.feed;
import java.util.HashMap;
import java.util.Map;

public class AdFeedBuilder {
//	private int actor;
//	private int visitor;
//	private String visitorName;
//	private long trackId;
//	private int shareLevel;
	private Map<String,String> props;
	
	public AdFeedBuilder(int actor, int visitor, String visitorName, long trackId, int shareLevel) {
		props = new HashMap<String, String>();
		props.put("actor", Integer.toString(actor));
		props.put("user_id", Integer.toString(visitor));
		props.put("user_name", visitorName);
		props.put("expose_id", Long.toString(trackId));
		props.put("expose_num", Integer.toString(shareLevel));
	}

	public Map<String,String> getProps() {
		return props;
	}

}
