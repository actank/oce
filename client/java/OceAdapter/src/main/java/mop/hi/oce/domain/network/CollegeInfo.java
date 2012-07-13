package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;
import mop.hi.svc.model.CollegeColCOLLEGEID;
import mop.hi.svc.model.CollegeColCOLLEGENAME;
import mop.hi.svc.model.CollegeColDEPARTMENT;
import mop.hi.svc.model.CollegeColENROLLYEAR;
import mop.hi.svc.model.CollegeColUSERID;

public class CollegeInfo extends Updatable {

	public static final String ID = CollegeColCOLLEGEID.value;
	public static final String USERID = CollegeColUSERID.value;
	public static final String COLLEGE_ID = CollegeColCOLLEGEID.value;
	public static final String COLLEGE_NAME = CollegeColCOLLEGENAME.value;
	public static final String DEPARTMENT = CollegeColDEPARTMENT.value;
	public static final String ENROLL_YEAR = CollegeColENROLLYEAR.value;

	protected mop.hi.svc.model.CollegeInfo info = new mop.hi.svc.model.CollegeInfo();

	public String toString() {
		StringBuffer buf = new StringBuffer();
		buf.append("id:").append(getId()).append("\n");
		buf.append("getEnrollYear:").append(getEnrollYear()).append("\n");
		buf.append("getDepartment:").append(getDepartment()).append("\n");
		buf.append("getCollegeId:").append(getCollegeId()).append("\n");
		buf.append("getCollegeName:").append(getCollegeName()).append("\n");
		buf.append("getUserId:").append(getUserId()).append("\n");
		// System.out.println(buf.toString());
		return buf.toString();
	}

	public mop.hi.svc.model.CollegeInfo getInfo() {
		return info;
	}

	public void setInfo(mop.hi.svc.model.CollegeInfo info) {
		this.info = info;
	}

	public int getId() {
		return info.id;
	}

	public void setId(int id) {
		getUpdates().put(ID, String.valueOf(id));
		info.id = id;
	}

	public int getUserId() {
		return info.userId;
	}

	public void setUserId(int userId) {
		getUpdates().put(USERID, String.valueOf(userId));
		info.userId = userId;
	}

	public int getCollegeId() {
		return info.collegeId;
	}

	public void setCollegeId(int collegeId) {
		getUpdates().put(COLLEGE_ID, String.valueOf(collegeId));
		info.collegeId = collegeId;
	}

	public String getCollegeName() {
		return info.collegeName;
	}

	public void setCollegeName(String collegeName) {
		getUpdates().put(COLLEGE_NAME, collegeName);
		info.collegeName = collegeName;
	}

	public String getDepartment() {
		return info.department;
	}

	public void setDepartment(String department) {
		getUpdates().put(DEPARTMENT, department);
		info.department = department;
	}

	public int getEnrollYear() {
		return info.enrollYear;
	}

	public void setEnrollYear(int enrollYear) {
		getUpdates().put(ENROLL_YEAR, String.valueOf(enrollYear));
		info.enrollYear = enrollYear;
	}

}
