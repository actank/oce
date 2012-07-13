package mop.hi.oce.domain.model.importer;

public class ImporterBuddyInfoStatus {
	

	public static final int BLOCK=6;
	public static final int BEBLOCKED=5;
	public static final int FRIEND=4;
	public static final int BEREQUESTED=3;
	public static final int REQUEST=2;
	public static final int USER=1;
	public static final int NOTUSER=0;
	
	public final int code;
	public final String message;

	public ImporterBuddyInfoStatus(int c, String m) {
		code = c;
		message = m;
	}

//	public com.xiaonei.svc.importer.BuddyInfoStatus toSvcStatus(){
//		com.xiaonei.svc.importer.BuddyInfoStatus result  = new com.xiaonei.svc.importer.BuddyInfoStatus();
//		result.code = code;
//		result.message = message;
//		return result;
//	}

}
