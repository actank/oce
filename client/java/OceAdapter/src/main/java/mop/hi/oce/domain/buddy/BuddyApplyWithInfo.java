package mop.hi.oce.domain.buddy;

import java.util.Date;

import xce.util.tools.DateFormatter;


public class BuddyApplyWithInfo extends BuddyApply {

	public static String TIME = "TIME";
	public static String WHY = "WHY";
	public static String STATFROM = "STAT_FROM";
	public static String GROUPNAME = "GROUP_NAME";

	private Date time;
	private String why;
	private String statfrom;
	private String groupname;

	public BuddyApplyWithInfo(int applicant, int accepter) {
		super(applicant, accepter);
	}

	public Date getTime() {
		return time;
	}

	public void setTime(Date time) {
		this.time = time;
		this.props.put(BuddyApplyWithInfo.TIME, DateFormatter.format(time));
	}

	public String getWhy() {
		return why;
	}

	public void setWhy(String why) {
		this.why = why;
		this.props.put(BuddyApplyWithInfo.WHY, why);
	}
	
	public String getStatFrom(){
		return statfrom;
	}

	public void setStatFrom(String statfrom){
		this.statfrom = statfrom;
		this.props.put(BuddyApplyWithInfo.STATFROM, String.valueOf(statfrom));
	}

	public String getGroupName(){
		return groupname;
	}

	public void setGroupName(String groupname){
		this.groupname = groupname;
		this.props.put(BuddyApplyWithInfo.GROUPNAME, String.valueOf(groupname));
	}

	public String toString() {
		String result = "Request: \n" + " Host=" + getApplicant() + "\n"
				+ " Guest=" + getAccepter() + "\n" + " Time="
				+ DateFormatter.format(getTime()) + "\n" + " Why=" + getWhy()
				+ "\n" + " StatFrom=" + getStatFrom() + "\n" 
				+ " GroupName=" + getGroupName() + "\n" + " END.";
		return result;
	}
}
