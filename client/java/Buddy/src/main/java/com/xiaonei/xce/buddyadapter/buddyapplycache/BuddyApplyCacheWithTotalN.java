package com.xiaonei.xce.buddyadapter.buddyapplycache;

import java.util.ArrayList;
import java.util.List;

/**
 * @author yuyang
 *
 * @param <T>
 */
public abstract class BuddyApplyCacheWithTotalN<T extends BuddyApplyCacheDataN> {

	private List<T> _buddyApplyCacheN = new ArrayList<T>();
	private int _totalCount;

	/**
	 * @return
	 */
	public List<T> getBuddyApplyCacheN() {
		return _buddyApplyCacheN;
	}

	/**
	 * @return
	 */
	public int getTotalCount() {
		return _totalCount;
	}

	/**
	 * @param xcedata
	 * @param factory
	 */
	protected void setBase(xce.buddyapplycache.BuddyApplyCacheWithTotalN xcedata,
			BuddyApplyCacheFactoryN<T, ?> factory) {
		_totalCount = xcedata.totalCount;
		for (xce.buddyapplycache.BuddyApplyCacheDataN apply: xcedata.caches) {
			T o = factory.createBuddyApplyCacheDataN();
			o.setBase(apply);
			_buddyApplyCacheN.add(o);
		}
	}
	
	protected void setBase(List<T> data) {
		_totalCount = data.size();
		_buddyApplyCacheN = data;
	}
}
