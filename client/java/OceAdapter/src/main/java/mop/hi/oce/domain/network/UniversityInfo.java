package mop.hi.oce.domain.network;

import mop.hi.oce.domain.Updatable;

public class UniversityInfo extends Updatable {
	public static final String ID = "ID";

	public static final String USERID = "USERID";

	public static final String UNIVERSITY_ID = "UNIVERSITY_ID";

	public static final String UNIVERSITY_NAME = "UNIVERSITY_NAME";

	public static final String ENROLL_YEAR = "ENROLL_YEAR";

	public static final String DORM = "DORM";

	public static final String DEPARTMENT = "DEPARTMENT";

	public static final String TYPE_OF_COURSE = "TYPE_OF_COURSE";

	protected mop.hi.svc.model.UniversityInfo info = new mop.hi.svc.model.UniversityInfo();

	public String toString() {
		StringBuffer buf = new StringBuffer();
		buf.append("id:").append(getId()).append("\n");
		buf.append("getEnrollYear:").append(getEnrollYear()).append("\n");
		buf.append("getDorm:").append(getDorm()).append("\n");
		buf.append("getDepartment:").append(getDepartment()).append("\n");
		buf.append("getTypeOfCourse:").append(getTypeOfCourse()).append("\n");
		buf.append("getUniversityId:").append(getUniversityId()).append("\n");
		buf.append("getUniversityName:").append(getUniversityName()).append(
				"\n");
		buf.append("getUserId:").append(getUserId()).append("\n");
		// System.out.println(buf.toString());
		return buf.toString();
	}

	public mop.hi.svc.model.UniversityInfo get() {
		return info;
	}

	public void set(mop.hi.svc.model.UniversityInfo info) {
		this.info = info;
	}

	public int getId() {
		return info.id;
	}

	public void setId(int id) {
		this.getUpdates().put(UniversityInfo.ID, String.valueOf(id));
		info.id = id;
	}

	public int getEnrollYear() {
		return info.enrollYear;
	}

	public void setEnrollYear(int enrollYear) {
		this.getUpdates().put(UniversityInfo.ENROLL_YEAR,
				String.valueOf(enrollYear));
		info.enrollYear = enrollYear;
	}

	public String getDorm() {
		return info.dorm;
	}

	public void setDorm(String dorm) {
		this.getUpdates().put(UniversityInfo.DORM, dorm);
		info.dorm = dorm;
	}

	public String getDepartment() {
		return info.department;
	}

	public void setDepartment(String department) {
		this.getUpdates().put(UniversityInfo.DEPARTMENT, department);
		info.department = department;
	}

	public String getTypeOfCourse() {
		return info.typeOfCourse;
	}

	public void setTypeOfCourse(String typeOfCourse) {
		this.getUpdates().put(UniversityInfo.TYPE_OF_COURSE, typeOfCourse);
		info.typeOfCourse = typeOfCourse;
	}

	public int getUniversityId() {
		return info.universityId;
	}

	public void setUniversityId(int universityId) {
		this.getUpdates().put(UniversityInfo.UNIVERSITY_ID,
				String.valueOf(universityId));
		info.universityId = universityId;
	}

	public String getUniversityName() {
		return info.universityName;
	}

	public void setUniversityName(String universityName) {
		this.getUpdates().put(UniversityInfo.UNIVERSITY_NAME, universityName);
		info.universityName = universityName;
	}

	public int getUserId() {
		return info.userId;
	}

	public void setUserId(int userId) {
		this.getUpdates().put(UniversityInfo.USERID, String.valueOf(userId));
		info.userId = userId;
	}

}
