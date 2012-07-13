package com.xiaonei.xce.buddyadapter.buddybyapplicantcache;

/**
 * @author yuyang
 * 
 */
public class DefaultBuddyByApplicantCacheFactory
		implements
		BuddyByApplicantCacheFactory<DefaultBuddyByApplicantCacheAccepterInfo, DefaultBuddyByApplicantCacheData> {

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.xiaonei.xce.buddyadapter.buddybyapplicantcache.BuddyByApplicantCacheFactory
	 * #createBuddyByApplicantCacheAccepterInfo()
	 */
	@Override
	public DefaultBuddyByApplicantCacheAccepterInfo createBuddyByApplicantCacheAccepterInfo() {
		// TODO Auto-generated method stub
		return new DefaultBuddyByApplicantCacheAccepterInfo();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.xiaonei.xce.buddyadapter.buddybyapplicantcache.BuddyByApplicantCacheFactory
	 * #createBuddyByApplicantCacheData()
	 */
	@Override
	public DefaultBuddyByApplicantCacheData createBuddyByApplicantCacheData() {
		// TODO Auto-generated method stub
		return new DefaultBuddyByApplicantCacheData();
	}

}
