package com.xiaonei.xce.buddyapplycache;

import java.util.ArrayList;
import java.util.List;

public abstract class BuddyApplyCacheWithTotalN<T extends BuddyApplyCacheDataN> {

	private List<T> _buddyApplyCacheN = new ArrayList<T>();
	private int _totalCount;

	public List<T> getBuddyApplyCacheN() {
		return _buddyApplyCacheN;
	}

	public int getTotalCount() {
		return _totalCount;
	}

	protected void setBase(xce.buddyapplycache.BuddyApplyCacheWithTotalN xcedata,
			BuddyApplyCacheFactoryN<T, ?> factory) {
		_totalCount = xcedata.totalCount;
		for (xce.buddyapplycache.BuddyApplyCacheDataN apply: xcedata.caches) {
			T o = factory.createBuddyApplyCacheDataN();
			o.setBase(apply);
			_buddyApplyCacheN.add(o);
		}
	}
}
