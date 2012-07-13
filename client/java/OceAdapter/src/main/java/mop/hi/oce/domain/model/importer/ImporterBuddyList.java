package mop.hi.oce.domain.model.importer;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

public class ImporterBuddyList {

	public final ImporterOwner owner;
	public final ImporterBuddyInfo[] buddies;
	public final ImporterBuddyListStatus status;
	public Map<String, ImporterBuddyUser> users;
	
	public ImporterBuddyList(com.xiaonei.svc.importer.BuddyList svc) {
		owner = new ImporterOwner(svc.owner);
		status = new ImporterBuddyListStatus(svc.status.code, svc.status.message);
		buddies = new ImporterBuddyInfo[svc.buddies.length];
		for (int i = 0; i < svc.buddies.length; ++i) {
			buddies[i] = new ImporterBuddyInfo(svc.buddies[i]);
		}
		users = new HashMap<String, ImporterBuddyUser>();
		for (Object entry: svc.users.entrySet()) {
			Entry<String, com.xiaonei.svc.importer.BuddyUserInfo> e = (Entry<String, com.xiaonei.svc.importer.BuddyUserInfo>)entry;
			ImporterBuddyUser user =new ImporterBuddyUser(e.getValue()); 
			String email = e.getKey();
			users.put(email, user);
		}
	}

//	public com.xiaonei.svc.importer.BuddyList toSvcBuddyList() {
//		com.xiaonei.svc.importer.BuddyList result = new com.xiaonei.svc.importer.BuddyList();
//		result.owner = owner.toSvcOwner();
//		result.status = status.toSvcStatus();
//		result.buddies = new com.xiaonei.svc.importer.BuddyInfo[buddies.length];
//		for (int i = 0; i < buddies.length; ++i) {
//			result.buddies[i] = buddies[i].toSvcBuddy();
//		}
//		return result;
//	}
}
