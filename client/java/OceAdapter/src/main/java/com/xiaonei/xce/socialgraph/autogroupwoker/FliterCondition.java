package com.xiaonei.xce.socialgraph.autogroupwoker;

/**
 * 条件过滤类，用来过滤相应的分组成员
 * @author zhangnan
 */
public class FliterCondition {
	private String schoolName;		//学校名
	private String academe;			//学院
	private int enrollYear;			//入学年

	public FliterCondition() {
		// TODO Auto-generated constructor stub
		schoolName = "";
		academe = "";
		enrollYear = 0;
	}
	
	public void setSchoolName(String schoolName) {
		this.schoolName = schoolName;
	}
	
	public void setAcademe(String academe) {
		this.academe = academe;
	}
	
	public void setEnrollYear(int enrollYear) {
		this.enrollYear = enrollYear;
	}
	
	public String getSchoolName() {
		return schoolName;
	}
	
	public String getAcademe() {
		return academe;
	}
	
	public int getEnrollYear() {
		return enrollYear;
	}
}
