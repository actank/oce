package com.xiaonei.xce.buddyadapter.buddybyapplicantcache;

/**
 * @author yuyang
 * 
 * @param <T>
 * @param <W>
 */
public interface BuddyByApplicantCacheFactory<T extends BuddyByApplicantCacheAccepterInfo, W extends BuddyByApplicantCacheData> {
	public T createBuddyByApplicantCacheAccepterInfo();

	public W createBuddyByApplicantCacheData();
}
