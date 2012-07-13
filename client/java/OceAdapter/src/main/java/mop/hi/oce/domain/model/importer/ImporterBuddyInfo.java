package mop.hi.oce.domain.model.importer;

public class ImporterBuddyInfo {
	
	public ImporterBuddyInfo(com.xiaonei.svc.importer.BuddyInfo svc) {
		email = svc.email;
		name = svc.name;
		status = new ImporterBuddyInfoStatus(svc.status.code, svc.status.message);
	}

//	public com.xiaonei.svc.importer.BuddyInfo toSvcBuddy() {
//		com.xiaonei.svc.importer.BuddyInfo result = new com.xiaonei.svc.importer.BuddyInfo();
//		result.email = email;
//		result.name = name;
//		result.status = status.toSvcStatus();
//		return result;
//	}

	public final String email;
	public final String name;
	public final ImporterBuddyInfoStatus status;
	
	public String getEmail() {
		return email;
	}
	
	public String getName() {
		return name;
	}
	
	public ImporterBuddyInfoStatus getStatus() {
		return status;
	}
	
}
