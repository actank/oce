package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;
import mop.hi.svc.model.JunHighColID;
import mop.hi.svc.model.JunHighColJUNIORHIGHSCHOOLID;
import mop.hi.svc.model.JunHighColJUNIORHIGHSCHOOLNAME;
import mop.hi.svc.model.JunHighColJUNIORHIGHSCHOOLYEAR;
import mop.hi.svc.model.JunHighColUSERID;
import mop.hi.svc.model.JuniorHighSchoolInfoN;

public class JuniorHighSchoolInfo extends Updatable {

    	public static final String ID = JunHighColID.value;
	public static final String USERID = JunHighColUSERID.value;
	public static final String JUNIORHIGH_SCHOOL_ID = JunHighColJUNIORHIGHSCHOOLID.value;
	public static final String JUNIORHIGH_SCHOOL_NAME = JunHighColJUNIORHIGHSCHOOLNAME.value;
	public static final String JUNIORHIGH_SCHOOL_YEAR = JunHighColJUNIORHIGHSCHOOLYEAR.value;

	protected mop.hi.svc.model.JuniorHighSchoolInfo info = new mop.hi.svc.model.JuniorHighSchoolInfoN();

	public mop.hi.svc.model.JuniorHighSchoolInfo getInfo() {
		return info;
	}

	public String toString() {
		StringBuffer buf = new StringBuffer();
		buf.append("id:").append(getId()).append("\n");
		buf.append("getUserId:").append(getUserId()).append("\n");
		buf.append("getJunHighentarySchoolId:").append(getJunHighentarySchoolId()).append("\n");
		buf.append("getJunHighentarySchoolName:").append(getJunHighentarySchoolName()).append("\n");
		buf.append("getJunHighentarySchoolYear:").append(getJunHighentarySchoolYear()).append("\n");
		// System.out.println(buf.toString());
		return buf.toString();
	}
	
	public void setInfo(mop.hi.svc.model.JuniorHighSchoolInfo info) {
		this.info = info;
	}
  public int getId() {
    return ((JuniorHighSchoolInfoN)info).id;
  }
  public void setId(int id) {
    getUpdates().put(ID, String.valueOf(id));
    ((JuniorHighSchoolInfoN)info).id=id;
  }
	public int getUserId() {
		return info.userId;
	}

	public void setUserId(int userId) {
		getUpdates().put(USERID, String.valueOf(userId));
		info.userId = userId;
	}

	public int getJunHighentarySchoolId() {
		return info.juniorHighSchoolId;
	}

	public void setJunHighentarySchoolId(int juniorHighSchoolId) {
		getUpdates().put(JUNIORHIGH_SCHOOL_ID,
				String.valueOf(juniorHighSchoolId));
		info.juniorHighSchoolId = juniorHighSchoolId;
	}

	public String getJunHighentarySchoolName() {
		return info.juniorHighSchoolName;
	}

	public void setJunHighentarySchoolName(String juniorHighSchoolName) {
		getUpdates().put(JUNIORHIGH_SCHOOL_NAME, juniorHighSchoolName);
		info.juniorHighSchoolName = juniorHighSchoolName;
	}

	public int getJunHighentarySchoolYear() {
		return info.juniorHighSchoolYear;
	}

	public void setJunHighentarySchoolYear(int juniorHighSchoolYear) {
		getUpdates().put(JUNIORHIGH_SCHOOL_YEAR,
				String.valueOf(juniorHighSchoolYear));
		info.juniorHighSchoolYear = juniorHighSchoolYear;
	}

}
