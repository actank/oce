package com.xiaonei.xce.domain.buddy;

import java.util.Date;

import xce.util.tools.DateFormatter;

/**
 * @author yuyang
 * 
 */
public class BuddyApplyWithInfo extends BuddyApply {

	public static String TIME = "TIME";
	public static String WHY = "WHY";
	public static String STATFROM = "STAT_FROM";
	public static String GROUPNAME = "GROUP_NAME";

	private Date time;
	private String why;
	private String statfrom;
	private String groupname;

	/**
	 * @param applicant
	 * @param accepter
	 */
	public BuddyApplyWithInfo(int applicant, int accepter) {
		super(applicant, accepter);
	}

	/**
	 * @return
	 */
	public Date getTime() {
		return time;
	}

	/**
	 * @param time
	 */
	public void setTime(Date time) {
		this.time = time;
		this.props.put(BuddyApplyWithInfo.TIME, DateFormatter.format(time));
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
		this.props.put(BuddyApplyWithInfo.WHY, why);
	}

	/**
	 * @return
	 */
	public String getStatFrom() {
		return statfrom;
	}

	/**
	 * @param statfrom
	 */
	public void setStatFrom(String statfrom) {
		this.statfrom = statfrom;
		this.props.put(BuddyApplyWithInfo.STATFROM, String.valueOf(statfrom));
	}

	/**
	 * @return
	 */
	public String getGroupName() {
		return groupname;
	}

	/**
	 * @param groupname
	 */
	public void setGroupName(String groupname) {
		this.groupname = groupname;
		this.props.put(BuddyApplyWithInfo.GROUPNAME, String.valueOf(groupname));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		String result = "Request: \n" + " Host=" + getApplicant() + "\n"
				+ " Guest=" + getAccepter() + "\n" + " Time="
				+ DateFormatter.format(getTime()) + "\n" + " Why=" + getWhy() + "\n"
				+ " StatFrom=" + getStatFrom() + "\n" + " GroupName=" + getGroupName()
				+ "\n" + " END.";
		return result;
	}
}
