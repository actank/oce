package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;

public class HighSchoolInfo extends Updatable {
	public static final String ID = "ID";

	public static final String USERID = "USERID";

	public static final String HIGH_SCHOOL_ID = "HIGH_SCHOOL_ID";

	public static final String HIGH_SCHOOL_NAME = "HIGH_SCHOOL_NAME";

	public static final String ENROLL_YEAR = "ENROLL_YEAR";

	public static final String H_CLASS1 = "H_CLASS1";

	public static final String H_CLASS2 = "H_CLASS2";

	public static final String H_CLASS3 = "H_CLASS3";

	protected mop.hi.svc.model.HighSchoolInfo info = new mop.hi.svc.model.HighSchoolInfo();

	public String toString()
	{
		StringBuffer str = new StringBuffer();
		str.append(this.ID).append("=").append(String.valueOf(this.getId())).append("\n");
		str.append(this.USERID).append("=").append(String.valueOf(this.getUserId())).append("\n");
		str.append(this.HIGH_SCHOOL_ID).append("=").append(String.valueOf(this.getHighSchoolId())).append("\n");
		str.append(this.HIGH_SCHOOL_NAME).append("=").append(String.valueOf(this.HIGH_SCHOOL_NAME)).append("\n");
		str.append(this.ENROLL_YEAR).append("=").append(String.valueOf(this.getEnrollYear())).append("\n");
		str.append(this.H_CLASS1).append("=").append(String.valueOf(this.getHClass1())).append("\n");
		str.append(this.H_CLASS2).append("=").append(String.valueOf(this.getHClass2())).append("\n");
		str.append(this.H_CLASS3).append("=").append(String.valueOf(this.getHClass3())).append("\n");
		return str.toString();
	}
	
	public mop.hi.svc.model.HighSchoolInfo get() {
		return info;
	}

	public void set(mop.hi.svc.model.HighSchoolInfo info) {
		this.info = info;
	}

	public int getId() {
		return info.id;
	}

	public void setId(int id) {
		this.getUpdates().put(HighSchoolInfo.ID, String.valueOf(id));
		info.id = id;
	}

	public int getEnrollYear() {
		return info.enrollYear;
	}

	public void setEnrollYear(int enrollYear) {
		this.getUpdates().put(HighSchoolInfo.ENROLL_YEAR,
				String.valueOf(enrollYear));
		info.enrollYear = enrollYear;
	}

	public String getHClass1() {
		return info.hClass1;
	}

	public void setHClass1(String class1) {
		this.getUpdates().put(HighSchoolInfo.H_CLASS1, String.valueOf(class1));
		info.hClass1 = class1;
	}

	public String getHClass2() {
		return info.hClass2;
	}

	public void setHClass2(String class2) {
		this.getUpdates().put(HighSchoolInfo.H_CLASS2, String.valueOf(class2));
		info.hClass2 = class2;
	}

	public String getHClass3() {
		return info.hClass3;
	}

	public void setHClass3(String class3) {
		this.getUpdates().put(HighSchoolInfo.H_CLASS3, String.valueOf(class3));
		info.hClass3 = class3;

	}

	public int getHighSchoolId() {
		return info.HighSchoolId;
	}

	public void setHighSchoolId(int highSchoolId) {
		this.getUpdates().put(HighSchoolInfo.HIGH_SCHOOL_ID,
				String.valueOf(highSchoolId));
		info.HighSchoolId = highSchoolId;
	}

	public String getHighSchoolName() {
		return info.HighSchoolName;
	}

	public void setHighSchoolName(String highSchoolName) {
		this.getUpdates().put(HighSchoolInfo.HIGH_SCHOOL_NAME, highSchoolName);
		info.HighSchoolName = highSchoolName;
	}

	public int getUserId() {
		return info.userId;
	}

	public void setUserId(int userId) {
		this.getUpdates().put(HighSchoolInfo.USERID, String.valueOf(userId));
		info.userId = userId;
	}

}
