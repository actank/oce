package com.xiaonei.xce.buddybyapplicantcache;

import java.util.ArrayList;
import java.util.List;

import xce.buddy.AccepterInfo;

import com.xiaonei.xce.buddyapplycache.BuddyApplyCacheDataN;
import com.xiaonei.xce.buddyapplycache.BuddyApplyCacheFactoryN;

public abstract class BuddyByApplicantCacheData<T extends BuddyByApplicantCacheAccepterInfo> {

	private List<T> _accepters = new ArrayList<T>();
	private int _totalCount;

	public List<T> getAccepters() {
		return _accepters;
	}

	public int getTotalCount() {
		return _totalCount;
	}

	protected void setBase(xce.buddy.BuddyByApplicantData xcedata,
			BuddyByApplicantCacheFactory<T, ?> factory) {
		
		_totalCount = xcedata.totalCount;
		for (AccepterInfo accepter : xcedata.accepters) {
			T o = factory.createBuddyByApplicantCacheAccepterInfo();
			o.setBase(accepter);
			_accepters.add(o);
		}
	}
}