package com.xiaonei.xce.buddybyapplicantcache;

public class DefaultBuddyByApplicantCacheFactory implements BuddyByApplicantCacheFactory<DefaultBuddyByApplicantCacheAccepterInfo, DefaultBuddyByApplicantCacheData> {

	@Override
	public DefaultBuddyByApplicantCacheAccepterInfo createBuddyByApplicantCacheAccepterInfo() {
		// TODO Auto-generated method stub
		return new DefaultBuddyByApplicantCacheAccepterInfo();
	}

	@Override
	public DefaultBuddyByApplicantCacheData createBuddyByApplicantCacheData() {
		// TODO Auto-generated method stub
		return new DefaultBuddyByApplicantCacheData();
	}
	
}