package mop.hi.oce.domain.model.importer;

public class ImporterBuddyListStatus {
	
	public static final boolean isOK(int status){
		if(status >=200 && status < 300){
			return true;
		}
		return false;
	}
	
	public static final boolean isPending(int status){
		if(status >=300 && status < 400){
			return true;
		}
		return false;
	}
	
	public static final boolean isError(int status){
		if(status >=400 && status < 500){
			return true;
		}
		return false;
	}
	
	public static final boolean isException(int status){
		if(status >=500){
			return true;
		}
		return false;
	}
	
	public final int code;
	public final String message;

	public ImporterBuddyListStatus(int c, String m) {
		code = c;
		message = m;
	}

//	public com.xiaonei.svc.importer.BuddyListStatus toSvcStatus(){
//		com.xiaonei.svc.importer.BuddyListStatus result  = new com.xiaonei.svc.importer.BuddyListStatus();
//		result.code = code;
//		result.message = message;
//		return result;
//	}
}
