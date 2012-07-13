package com.xiaonei.xce.test.buddybyapplicantcachesync;

import com.xiaonei.xce.buddyadapter.buddybyapplicantcache.BuddyByApplicantCacheAccepterInfo;
import com.xiaonei.xce.buddyadapter.buddybyapplicantcache.BuddyByApplicantCacheAdapter;
import com.xiaonei.xce.buddyadapter.buddybyapplicantcache.DefaultBuddyByApplicantCacheAccepterInfo;
import com.xiaonei.xce.buddyadapter.buddybyapplicantcache.DefaultBuddyByApplicantCacheData;
import com.xiaonei.xce.buddyadapter.buddybyapplicantcache.DefaultBuddyByApplicantCacheFactory;

public class BuddyByApplicantCacheMiniTest {
	public static void main(String[] args) {
		int id = 257026792;
		int begin = 0;
		int limit = -1;
		
		BuddyByApplicantCacheAdapter _adapter = new BuddyByApplicantCacheAdapter<DefaultBuddyByApplicantCacheAccepterInfo, DefaultBuddyByApplicantCacheData>(new DefaultBuddyByApplicantCacheFactory());	
		DefaultBuddyByApplicantCacheData _data = (DefaultBuddyByApplicantCacheData) _adapter.getBuddyByApplicantCacheDataSync(id, begin, limit);
		System.out.println("totalCount: " + _data.getTotalCount());
		for (BuddyByApplicantCacheAccepterInfo el : _data.getAccepters()) {
			System.out.println("id: " + el.getAccepter() + " Time: " + el.getTime());
			System.out.println("----------------------");
		}
		
		System.exit(0);
	}
}
