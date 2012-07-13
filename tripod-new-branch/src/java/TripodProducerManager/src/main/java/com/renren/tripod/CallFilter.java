package com.renren.tripod;

import java.util.HashMap;

import com.xiaonei.xce.log.Tripod;

public class CallFilter {

	private class CountPair {
		private int _curCount = 0;
		private int _maxCount;

		CountPair(int maxCount) {
			_maxCount = maxCount;
		}

		synchronized boolean inc() {
			return ++_curCount >= _maxCount;
		}

		synchronized void dec() {
			--_curCount;
		}
	}

	private class FilterHolder {
		private HashMap<String, CountPair> _filterMap = new HashMap<String, CountPair>();

		public FilterHolder() {
			// add business config here
		}

		public CountPair getCounter(String businessId) {
			return _filterMap.get(businessId);
		}
	}

	FilterHolder _filterHolder = new FilterHolder();

	public boolean filter(String businessId) {
		CountPair cp = _filterHolder.getCounter(businessId);
		if (cp != null) {
			if (cp.inc()) {
				return true;
			} else {
				Tripod.getLogger().error(
						"CallFilter.filter too many call for business "
								+ businessId + " max is " + cp._maxCount);
			}
		}
		return false;
	}

	public void exitFilter(String businessId) {
		CountPair cp = _filterHolder.getCounter(businessId);
		if (cp != null) {
			cp.dec();
		}
	}

}
