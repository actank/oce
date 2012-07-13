package com.xiaonei.xce.buddybyaddtimecache;

public class DefaultBuddyByAddTimeFactory implements BuddyByAddTimeFactory<DefaultBuddyByAddTime> {

	@Override
	public DefaultBuddyByAddTime create() {
		return new DefaultBuddyByAddTime();
	}

}
