package com.xiaonei.xce.test.buddybyapplicantcache;

import com.xiaonei.xce.buddybyapplicantcache.BuddyByApplicantCacheAccepterInfo;
import com.xiaonei.xce.buddybyapplicantcache.BuddyByApplicantCacheAdapter;
import com.xiaonei.xce.buddybyapplicantcache.BuddyByApplicantCacheData;
import com.xiaonei.xce.buddybyapplicantcache.DefaultBuddyByApplicantCacheAccepterInfo;
import com.xiaonei.xce.buddybyapplicantcache.DefaultBuddyByApplicantCacheData;
import com.xiaonei.xce.buddybyapplicantcache.DefaultBuddyByApplicantCacheFactory;

public class BuddyByApplicantCacheMiniTest {
	public static void main(String[] args) {
		int id = 3269806;
		int accepterId = 233940269;
//		int id = 233940269;
		int begin = 0;
		int limit = -1;
		BuddyByApplicantCacheAdapter _adapter = new BuddyByApplicantCacheAdapter<DefaultBuddyByApplicantCacheAccepterInfo, DefaultBuddyByApplicantCacheData>(new DefaultBuddyByApplicantCacheFactory());
		
		DefaultBuddyByApplicantCacheData _data = (DefaultBuddyByApplicantCacheData) _adapter.getBuddyByApplicantCacheData(id, begin, limit);
		
		System.out.println("totalCount: " + _data.getTotalCount());
		
		for (BuddyByApplicantCacheAccepterInfo el : _data.getAccepters()) {
			System.out.println("id: " + el.getAccepter() + " Time: " + el.getTime());
			System.out.println("----------------------");
		}
//		
//		DefaultBuddyByApplicantCacheAccepterInfo accepter = new DefaultBuddyByApplicantCacheAccepterInfo();
//		accepter.setAccepter(accepterId);
//		_adapter.addAccepter(id, accepter);
		
//		_adapter.reload(id);
//		_adapter.clear(id);
//		_adapter.removeAccepter(id, accepterId);
		
		System.exit(0);
	}
}
