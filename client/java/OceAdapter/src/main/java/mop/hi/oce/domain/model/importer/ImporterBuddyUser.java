package mop.hi.oce.domain.model.importer;

public class ImporterBuddyUser {
	public final String email;
	public final int userId;
	public final String userName;
	public final String tinyurl;
	public final int currentNetworkId;
	public final int currentNetworkStage;
	public final String currentNetworkName;
	
//	public ImporterBuddyUser(String e, int u, String n, String t, int nid, int nst, String nname){
//		email = e;
//		userId = u;
//		userName = n;
//		tinyurl=t;
//		currentNetworkId = nid;
//		currentNetworkStage = nst;
//		currentNetworkName = nname;
//	}
	
	public String getEmail() {
		return email;
	}

	public int getUserId() {
		return userId;
	}

	public String getUserName() {
		return userName;
	}

	public String getTinyurl() {
		return tinyurl;
	}

	public int getCurrentNetworkId() {
		return currentNetworkId;
	}

	public int getCurrentNetworkStage() {
		return currentNetworkStage;
	}

	public String getCurrentNetworkName() {
		return currentNetworkName;
	}

	public ImporterBuddyUser(com.xiaonei.svc.importer.BuddyUserInfo svc){
		email = svc.email;
		userId = svc.userId;
		userName = svc.userName;
		tinyurl = svc.tinyurl;
		currentNetworkId = svc.currentNetworkId;
		currentNetworkStage = svc.currentNetworkStage;
		currentNetworkName = svc.currentNetworkName;
	}
}
