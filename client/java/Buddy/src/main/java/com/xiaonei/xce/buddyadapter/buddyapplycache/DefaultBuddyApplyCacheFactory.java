package com.xiaonei.xce.buddyadapter.buddyapplycache;

public class DefaultBuddyApplyCacheFactory
		implements
		BuddyApplyCacheFactoryN<DefaultBuddyApplyCacheData, DefaultBuddyApplyCacheWithTotal> {

	public DefaultBuddyApplyCacheData createBuddyApplyCacheDataN() {
		// TODO Auto-generated method stub
		return new DefaultBuddyApplyCacheData();
	}

	public DefaultBuddyApplyCacheWithTotal createBuddyApplyCacheWithTotalN() {
		// TODO Auto-generated method stub
		return new DefaultBuddyApplyCacheWithTotal();
	}
}
