package com.xiaonei.xce.buddyapplycache;

public abstract class BuddyApplyCacheDataN {
	private int applicant;
	private int time;
	private String why;
	private String statFrom;
	private String groupName;
		
	public int getApplicant() {
		return applicant;
	}

	public void setApplicant(int applicant) {
		this.applicant = applicant;
	}

	public int getTime() {
		return time;
	}

	public void setTime(int time) {
		this.time = time;
	}

	public String getWhy() {
		return why;
	}

	public void setWhy(String why) {
		this.why = why;
	}

	public String getStatFrom() {
		return statFrom;
	}

	public void setStatFrom(String statFrom) {
		this.statFrom = statFrom;
	}

	public String getGroupName() {
		return groupName;
	}

	public void setGroupName(String groupName) {
		this.groupName = groupName;
	}

	protected void setBase(xce.buddyapplycache.BuddyApplyCacheDataN one){
		setApplicant(one.applicant);
		setTime(one.time);
		setWhy(one.why);
		setStatFrom(one.statfrom);
		setGroupName(one.groupname);
	}
}
