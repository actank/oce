package mop.hi.oce.domain.model;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import xce.util.tools.DateFormatter;


public class ClassMember {
	private int id;

	private int classId;

	private int memberId;

	private String memberName;

	private int role;

	private Date logonTime;

	private int logonCount;

	private int schoolFlag;

	private String tinyurl;

	public static String ID = "ID";

	public static String CLASSID = "CLASSID";

	public static String MEMBERID = "MEMBERID";

	public static String MEMBERNAME = "MEMBERNAME";

	public static String ROLE = "ROLE";

	public static String LOGONTIME = "LOGONTIME";

	public static String LOGONCOUNT = "LOGON_COUNT";

	public static String SCHOOLFLAG = "SCHOOLFLAG";

	public static String TINYURL = "TINYURL";

	private Map<String, String> props = new HashMap<String, String>();

	public static int FOUNDER_TYPE = -1; // -1:创始人 0:普通成员 1:管理员 4:被封禁'

	public static int MEM_TYPE = 0;

	public static int ADMIN_TYPE = 1;

	public static int BLOCK_TYPE = 4;

	public static int APPLY_TYPE = 10;

	public Map<String, String> getProperties() {
		return props;
	}

	public int getClassId() {
		return classId;
	}

	public void setClassId(int classId) {
		this.classId = classId;
		props.put(this.CLASSID, String.valueOf(this.classId));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getLogonCount() {
		return logonCount;
	}

	public void setLogonCount(int logonCount) {
		this.logonCount = logonCount;
		props.put(this.LOGONCOUNT, String.valueOf(this.logonCount));
	}

	public Date getLogonTime() {
		return logonTime;
	}

	public void setLogonTime(Date logonTime) {
		this.logonTime = logonTime;
		props.put(this.LOGONTIME, DateFormatter.format(this.logonTime));
	}

	public int getMemberId() {
		return memberId;
	}

	public void setMemberId(int memberId) {
		this.memberId = memberId;
		props.put(this.MEMBERID, String.valueOf(this.memberId));
	}

	public String getMemberName() {
		return memberName;
	}

	public void setMemberName(String memberName) {
		this.memberName = memberName;
		props.put(this.MEMBERNAME, this.memberName);
	}

	public int getRole() {
		return role;
	}

	public void setRole(int role) {
		this.role = role;
		props.put(this.ROLE, String.valueOf(this.role));
	}

	public int getSchoolFlag() {
		return schoolFlag;
	}

	public void setSchoolFlag(int schoolFlag) {
		this.schoolFlag = schoolFlag;
		props.put(this.SCHOOLFLAG, String.valueOf(this.schoolFlag));
	}

	public String getTinyurl() {
		return tinyurl;
	}

	public void setTinyurl(String tinyurl) {
		this.tinyurl = tinyurl;
		props.put(this.TINYURL, this.tinyurl);
	}

	public static ClassMember parse(Map<String, String> properties) {
		try {
			ClassMember result = new ClassMember();

			result.setClassId(Integer.parseInt(properties
					.get(ClassMember.CLASSID)));
			result.setId(Integer.parseInt(properties.get(ClassMember.ID)));
			result.setLogonCount(Integer.parseInt(properties
					.get(ClassMember.LOGONCOUNT)));
			result.setLogonTime(DateFormatter.parse(properties
					.get(ClassMember.LOGONTIME)));
			result.setMemberId(Integer.parseInt(properties
					.get(ClassMember.MEMBERID)));
			result.setMemberName(properties.get(ClassMember.MEMBERNAME));
			result.setRole(Integer.parseInt(properties.get(ClassMember.ROLE)));
			result.setSchoolFlag(Integer.parseInt(properties
					.get(ClassMember.SCHOOLFLAG)));
			result.setTinyurl(properties.get(ClassMember.TINYURL));
			result.props.clear();
			return result;
		} catch (Exception e) {
			// System.err.println(e.getStackTrace());
			return null;
		}

	}

}
