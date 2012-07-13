package com.xiaonei.xce.buddyadapter.buddyapplycache;

/**
 * @author yuyang
 *
 */
public abstract class BuddyApplyCacheDataN {
	private int applicant;
	private int time;
	private String why;
	private String statFrom;
	private String groupName;
		
	/**
	 * @return
	 */
	public int getApplicant() {
		return applicant;
	}

	/**
	 * @param applicant
	 */
	public void setApplicant(int applicant) {
		this.applicant = applicant;
	}

	/**
	 * @return
	 */
	public int getTime() {
		return time;
	}

	/**
	 * @param time
	 */
	public void setTime(int time) {
		this.time = time;
	}

	/**
	 * @return
	 */
	public String getWhy() {
		return why;
	}

	/**
	 * @param why
	 */
	public void setWhy(String why) {
		this.why = why;
	}

	/**
	 * @return
	 */
	public String getStatFrom() {
		return statFrom;
	}

	/**
	 * @param statFrom
	 */
	public void setStatFrom(String statFrom) {
		this.statFrom = statFrom;
	}

	/**
	 * @return
	 */
	public String getGroupName() {
		return groupName;
	}

	/**
	 * @param groupName
	 */
	public void setGroupName(String groupName) {
		this.groupName = groupName;
	}

	/**
	 * @param one
	 */
	protected void setBase(xce.buddyapplycache.BuddyApplyCacheDataN one){
		setApplicant(one.applicant);
		setTime(one.time);
		setWhy(one.why);
		setStatFrom(one.statfrom);
		setGroupName(one.groupname);
	}
	
	/**
	 * @param one
	 */
	protected void setBase(xce.distcache.buddyapplycache.BuddyApplyCacheData.ApplyData one){
		setApplicant((int)one.getApplicant());
		setTime(one.getTime());
		setWhy(one.getWhy());
		setStatFrom(one.getStatfrom());
		setGroupName(one.getGroupname());
	}
}
