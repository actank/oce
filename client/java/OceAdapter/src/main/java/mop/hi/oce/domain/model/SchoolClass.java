package mop.hi.oce.domain.model;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import xce.util.tools.DateFormatter;


public class SchoolClass {

	private int id;

	private Date foundTime;

	private int founder;

	private String founderName;

	private int memberCount;

	private int schoolFlag;

	private String classYear;

	private String className;

	private String classUrl;

	private int isPublic;

	private int spaceLeft;

	private String beautify;

	private String bulletin;

	private String provinceId;

	private String provinceName;

	private String cityUnivId;

	private String cityUnivName;

	private String schoolDeptId;

	private String schoolDeptName;

	private String founderTinyUrl;
	
	private String classRight;

	public static String ID = "ID";

	public static String FOUNDTIME = "FOUNDTIME";

	public static String FOUNDER = "FOUNDER";

	public static String FOUNDERNAME = "FOUNDER_NAME";

	public static String MEMBERCOUNT = "MEMBER_COUNT";

	public static String SCHOOLFLAG = "SCHOOLFLAG";

	public static String CLASSYEAR = "CLASSYEAR";

	public static String CLASSNAME = "CLASSNAME";

	public static String CLASSURL = "CLASSURL";

	public static String ISPUBLIC = "ISPUBLIC";

	public static String SPACELEFT = "SPACELEFT";

	public static String BEAUTIFY = "BEAUTIFY";

	public static String BULLETIN = "BULLETIN";

	public static String PROVINCEID = "PROVINCE_ID";

	public static String PROVINCENAME = "PROVINCE_NAME";

	public static String CITYUNIVID = "CITY_UNIV_ID";

	public static String CITYUNIVNAME = "CITY_UNIV_NAME";

	public static String SCHOOLDEPTID = "SCHOOL_DEPT_ID";

	public static String SCHOOLDEPTNAME = "SCHOOL_DEPT_NAME";

	public static String FOUNDERTINYURL = "FOUNDER_TINYURL";
	
	public static String CLASSRIGHT = "CLASSRIGHT";

	private Map<String, String> props = new HashMap<String, String>();

	public Map<String, String> getProperties() {
		return props;
	}

	public String getBeautify() {
		return beautify;
	}

	public void setBeautify(String beautify) {
		this.beautify = beautify;
		props.put(this.BEAUTIFY, this.beautify);
	}

	public String getBulletin() {
		return bulletin;
	}

	public void setBulletin(String bulletin) {
		this.bulletin = bulletin;
		props.put(this.BULLETIN, this.bulletin);
	}

	public String getCityUnivId() {
		return cityUnivId;
	}

	public void setCityUnivId(String cityUnivId) {
		this.cityUnivId = cityUnivId;
		props.put(this.CITYUNIVID, this.cityUnivId);
	}

	public String getCityUnivName() {
		return cityUnivName;
	}

	public void setCityUnivName(String cityUnivName) {
		this.cityUnivName = cityUnivName;
		props.put(this.CITYUNIVNAME, this.cityUnivName);
	}

	public String getClassName() {
		return className;
	}

	public void setClassName(String className) {
		this.className = className;
		props.put(this.CLASSNAME, this.className);
	}

	public String getClassUrl() {
		return classUrl;
	}

	public void setClassUrl(String classUrl) {
		this.classUrl = classUrl;
		props.put(this.CLASSURL, this.classUrl);
	}

	public String getClassYear() {
		return classYear;
	}

	public void setClassYear(String classYear) {
		this.classYear = classYear;
		props.put(this.CLASSYEAR, this.classYear);
	}

	public int getFounder() {
		return founder;
	}

	public void setFounder(int founder) {
		this.founder = founder;
		props.put(this.FOUNDER, String.valueOf(this.founder));
	}

	public String getFounderName() {
		return founderName;
	}

	public void setFounderName(String founderName) {
		this.founderName = founderName;
		props.put(this.FOUNDERNAME, this.founderName);
	}

	public Date getFoundTime() {
		return foundTime;
	}

	public void setFoundTime(Date foundTime) {
		this.foundTime = foundTime;
		props.put(this.FOUNDTIME, DateFormatter.format(this.foundTime));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getIsPublic() {
		return isPublic;
	}

	public void setIsPublic(int isPublic) {
		this.isPublic = isPublic;
		props.put(this.ISPUBLIC, String.valueOf(this.isPublic));
	}

	public int getMemberCount() {
		return memberCount;
	}

	public void setMemberCount(int memberCount) {
		this.memberCount = memberCount;
		props.put(this.MEMBERCOUNT, String.valueOf(this.memberCount));
	}

	public String getProvinceId() {
		return provinceId;
	}

	public void setProvinceId(String provinceId) {
		this.provinceId = provinceId;
		props.put(this.PROVINCEID, this.provinceId);
	}

	public String getProvinceName() {
		return provinceName;
	}

	public void setProvinceName(String provinceName) {
		this.provinceName = provinceName;
		props.put(this.PROVINCENAME, this.provinceName);
	}

	public String getSchoolDeptId() {
		return schoolDeptId;
	}

	public void setSchoolDeptId(String schoolDeptId) {
		this.schoolDeptId = schoolDeptId;
		props.put(this.SCHOOLDEPTID, this.schoolDeptId);
	}

	public String getSchoolDeptName() {
		return schoolDeptName;
	}

	public void setSchoolDeptName(String schoolDeptName) {
		this.schoolDeptName = schoolDeptName;
		props.put(this.SCHOOLDEPTNAME, this.schoolDeptName);
	}

	public int getSchoolFlag() {
		return schoolFlag;
	}

	public void setSchoolFlag(int schoolFlag) {
		this.schoolFlag = schoolFlag;
		props.put(this.SCHOOLFLAG, String.valueOf(this.schoolFlag));
	}

	public int getSpaceLeft() {
		return spaceLeft;
	}

	public void setSpaceLeft(int spaceLeft) {
		this.spaceLeft = spaceLeft;
		props.put(this.SPACELEFT, String.valueOf(this.spaceLeft));
	}

	public String getFounderTinyUrl() {
		return founderTinyUrl;
	}

	public void setFounderTinyUrl(String founderTinyUrl) {
		this.founderTinyUrl = founderTinyUrl;
		props.put(this.FOUNDERTINYURL, this.founderTinyUrl);
	}
	
	public String getClassRight() {
		return classRight;
	}

	public void setClassRight(String classRight) {
		this.classRight = classRight;
		props.put(this.CLASSRIGHT, this.classRight);
	}

	public static SchoolClass parse(Map<String, String> properties) {
		SchoolClass result = new SchoolClass();

		result.setBeautify(properties.get(SchoolClass.BEAUTIFY));
		result.setBulletin(properties.get(SchoolClass.BULLETIN));
		result.setCityUnivId(properties.get(SchoolClass.CITYUNIVID));
		result.setCityUnivName(properties.get(SchoolClass.CITYUNIVNAME));
		result.setClassName(properties.get(SchoolClass.CLASSNAME));
		result.setClassUrl(properties.get(SchoolClass.CLASSURL));
		result.setClassYear(properties.get(SchoolClass.CLASSYEAR));
		result
				.setFounder(Integer.parseInt(properties
						.get(SchoolClass.FOUNDER)));
		result.setFounderName(properties.get(SchoolClass.FOUNDERNAME));
		result.setFoundTime(DateFormatter.parse(properties
				.get(SchoolClass.FOUNDTIME)));
		result.setId(Integer.parseInt(properties.get(SchoolClass.ID)));
		result.setIsPublic(Integer.parseInt(properties
				.get(SchoolClass.ISPUBLIC)));
		result.setMemberCount(Integer.parseInt(properties
				.get(SchoolClass.MEMBERCOUNT)));
		result.setProvinceId(properties.get(SchoolClass.PROVINCEID));
		result.setProvinceName(properties.get(SchoolClass.PROVINCENAME));
		result.setSchoolDeptId(properties.get(SchoolClass.SCHOOLDEPTID));
		result.setSchoolDeptName(properties.get(SchoolClass.SCHOOLDEPTNAME));
		result.setSchoolFlag(Integer.parseInt(properties
				.get(SchoolClass.SCHOOLFLAG)));
		result.setSpaceLeft(Integer.parseInt(properties
				.get(SchoolClass.SPACELEFT)));
		result.setFounderTinyUrl(properties.get(SchoolClass.FOUNDERTINYURL));
		result.setClassRight(properties.get(SchoolClass.CLASSRIGHT));
		result.props.clear();
		return result;
	}

}
