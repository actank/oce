package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;
import mop.hi.svc.model.ElemColELEMENTARYSCHOOLID;
import mop.hi.svc.model.ElemColELEMENTARYSCHOOLNAME;
import mop.hi.svc.model.ElemColELEMENTARYSCHOOLYEAR;
import mop.hi.svc.model.ElemColUSERID;
import mop.hi.svc.model.ElemColID;

public class ElementarySchoolInfo extends Updatable {

	public static final String ID = ElemColID.value;
	public static final String USERID = ElemColUSERID.value;
	public static final String ELEMENTARY_SCHOOL_ID = ElemColELEMENTARYSCHOOLID.value;
	public static final String ELEMENTARY_SCHOOL_NAME = ElemColELEMENTARYSCHOOLNAME.value;
	public static final String ELEMENTARY_SCHOOL_YEAR = ElemColELEMENTARYSCHOOLYEAR.value;
	
	protected mop.hi.svc.model.ElementarySchoolInfo info = new mop.hi.svc.model.ElementarySchoolInfoN();

	public mop.hi.svc.model.ElementarySchoolInfo getInfo() {
		return info;
	}

	public String toString() {
		StringBuffer buf = new StringBuffer();
		buf.append("id:").append(getId()).append("\n");
		buf.append("getUserId:").append(getUserId()).append("\n");
		buf.append("getJunHighentarySchoolId:").append(getElementarySchoolId()).append("\n");
		buf.append("getJunHighentarySchoolName:").append(getElementarySchoolName()).append("\n");
		buf.append("getJunHighentarySchoolYear:").append(getElementarySchoolYear()).append("\n");
		// System.out.println(buf.toString());
		return buf.toString();
	}
	
	public void setInfo(mop.hi.svc.model.ElementarySchoolInfo info) {
		this.info = info;
	}
  public int getId() {
    return ((mop.hi.svc.model.ElementarySchoolInfoN)info).id;
  }
  public void setId(int id) {
    getUpdates().put(ID, String.valueOf(id));
    ((mop.hi.svc.model.ElementarySchoolInfoN)info).id = id;
  }
	public int getUserId() {
		return info.userId;
	}
	public void setUserId(int userId) {
		getUpdates().put(USERID, String.valueOf(userId));
		info.userId = userId;
	}
	public int getElementarySchoolId() {
		return info.elementarySchoolId;
	}
	public void setElementarySchoolId(int elementarySchoolId) {
		getUpdates().put(ELEMENTARY_SCHOOL_ID, String.valueOf(elementarySchoolId));
		info.elementarySchoolId = elementarySchoolId;
	}
	public String getElementarySchoolName() {
		return info.elementarySchoolName;
	}
	public void setElementarySchoolName(String elementarySchoolName) {
		getUpdates().put(ELEMENTARY_SCHOOL_NAME, elementarySchoolName);
		info.elementarySchoolName = elementarySchoolName;
	}
	public int getElementarySchoolYear() {
		return info.elementarySchoolYear;
	}
	public void setElementarySchoolYear(int elementarySchoolYear) {
		getUpdates().put(ELEMENTARY_SCHOOL_YEAR, String.valueOf(elementarySchoolYear));
		info.elementarySchoolYear = elementarySchoolYear;
	}
	
}
