package com.xiaonei.xce.buddyadapter.buddybyaddtimecache;

/**
 * @author yuyang
 *
 */
public class DefaultBuddyByAddTimeFactory implements BuddyByAddTimeFactory<DefaultBuddyByAddTime> {

	@Override
	public DefaultBuddyByAddTime create() {
		return new DefaultBuddyByAddTime();
	}

}
