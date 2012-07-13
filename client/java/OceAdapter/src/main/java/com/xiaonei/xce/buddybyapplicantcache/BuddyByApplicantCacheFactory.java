package com.xiaonei.xce.buddybyapplicantcache;

public interface BuddyByApplicantCacheFactory<T extends BuddyByApplicantCacheAccepterInfo, W extends BuddyByApplicantCacheData> {
	public T createBuddyByApplicantCacheAccepterInfo();
	public W createBuddyByApplicantCacheData();
}