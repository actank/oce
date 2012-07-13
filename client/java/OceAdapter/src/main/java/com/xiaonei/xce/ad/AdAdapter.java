package com.xiaonei.xce.ad;


public class AdAdapter {
	private static AdAdapter instance;

	protected AdAdapter() {
	};

	public static AdAdapter getInstance() {
		if (instance == null) {
			instance = new AdAdapter();
		}
		return instance;
	}
	
	public void reloadGroupById(long groupid) {

		AdAnalysisAdapter.getInstance().reloadGroupById(groupid);

	}

	public void reloadMemberById(long memberid) {
		AdAnalysisAdapter.getInstance().reloadMemberById(memberid);
	}

	public void reloadCampaignById(long planid) {
		AdAnalysisAdapter.getInstance().reloadCampaignById(planid);
	}

	public void reloadCreativeById(long creativeid) {
		AdAnalysisAdapter.getInstance().reloadCreativeById(creativeid);
	}
	
	public void bindUser(int userid, long creativeid, long zoneid){
		AdEngineBAdapter.getInstance().bindUser(userid, creativeid, zoneid);
	}
    public void unbindUser(int userid, long zoneid){
    	AdEngineBAdapter.getInstance().unbindUser(userid, zoneid);
    }
	
}
