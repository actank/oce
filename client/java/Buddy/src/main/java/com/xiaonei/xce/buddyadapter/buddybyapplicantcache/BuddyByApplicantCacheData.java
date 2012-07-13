package com.xiaonei.xce.buddyadapter.buddybyapplicantcache;

import java.util.ArrayList;
import java.util.List;

import xce.buddy.AccepterInfo;

/**
 * @author yuyang
 * 
 * @param <T>
 */
public abstract class BuddyByApplicantCacheData<T extends BuddyByApplicantCacheAccepterInfo> {

	private List<T> _accepters = new ArrayList<T>();
	private int _totalCount;

	/**
	 * @return
	 */
	public List<T> getAccepters() {
		return _accepters;
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
