package mop.hi.oce.domain.user;

import java.io.Serializable;
import java.util.Date;
import java.util.Map;

import mop.hi.oce.domain.Markable;
import xce.util.tools.DateFormatter;


/**
 * 
 * UserBasic.java
 * 
 * @author drizzt drizzt.wang@gmail.com 2006-10-5 涓嬪崍04:05:51
 */
public class UserBasic extends Markable implements Serializable {
	private static final long serialVersionUID = 8234650290181069541L;

	public static final String ID = "ID";

	public static final String UNIV = "UNIV";

	public static final String SELECTED = "SELECTED";

	public static final String NAME = "NAME";

	public static final String STATUS = "STATUS";

	public static final String ONLINESTATUS = "ONLINESTATUS";

	public static final String GENDER = "GENDER";

	public static final String UNIVYEAR = "UNIV_YEAR";

	public static final String DEPARTMENT = "DEPARTMENT";

	public static final String DORM = "DORM";

	public static final String BIRTHYEAR = "BIRTH_YEAR";

	public static final String BIRTHMONTH = "BIRTH_MONTH";

	public static final String BIRTHDAY = "BIRTH_DAY";

	public static final String HOMEPROVINCE = "HOME_PROVINCE";

	public static final String HOMECITY = "HOME_CITY";

	public static final String CITYCODE = "CITY_CODE";

	public static final String HIGHSCHOOL = "HIGH_SCHOOL";

	public static final String HIGHSCHOOLCODE = "HIGH_SCHOOL_CODE";

	public static final String ASTROLOGY = "ASTROLOGY";

	public static final String HEADURL = "HEADURL";

	public static final String TINYURL = "TINYURL";

	public static final String BASICCONTROL = "BASICCONTROL";

	public static final String STAR = "STAR";

	public static final String DOING = "DOING";

	public static final String DOINGTIME = "DOINGTIME";

	public static final String STAGE = "STAGE";

	// public static final String CURNETWORKID = "CUR_NETWORK_ID";
	//	
	// public static final String CURNETWORKNAME = "CUR_NETWORK_NAME";

	// public static final String JOINYEAR = "JOIN_YEAR";

	public static final String ROOM = "ROOM";

	protected int id;

	protected String name;

	protected int selected;// = User.usrNotSelected;

	protected int univ;

	protected int status;// = com.dodoyo.model.User.statusActivateVerified;

	// protected int onlineStatus;

	protected int univYear;

	protected String department;

	protected String dorm;

	protected String gender;// = User.genderUnknown;

	protected String homeProvince;

	protected String homeCity;

	protected String cityCode;

	protected String highSchool;

	protected String highSchoolCode;

	protected int birthYear;

	protected int birthMonth;

	protected int birthDay;

	protected String astrology;

	protected int star;

	protected int basicControl;// = Relation1.uuNoPath;

	protected String headUrl;// = Globals.userHeadUrl;

	protected String tinyUrl;// = Globals.userTinyUrl;

	protected String doing;

	protected Date doingTime;

	protected int stage;

	// protected int curNetworkId;
	//	
	// protected String curNetworkName;

	// protected int joinYear;

	protected String room;

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append("UserBasic:\n");
		str.append("ID = ").append(this.getId()).append("\n");
		str.append("UNIV = ").append(getUniv()).append("\n");
		str.append("SELECTED = ").append(getSelected()).append("\n");
		str.append("NAME = ").append(getName()).append("\n");
		str.append("STATUS = ").append(getStatus()).append("\n");
		// str.append("ONLINESTATUS = ").append(getOnlineStatus()).append("\n");
		str.append("GENDER = ").append(getGender()).append("\n");
		str.append("UNIVYEAR = ").append(getUnivYear()).append("\n");
		str.append("DEPARTMENT = ").append(getDepartment()).append("\n");
		str.append("DORM = ").append(getDorm()).append("\n");
		str.append("BIRTHYEAR = ").append(getBirthYear()).append("\n");
		str.append("BIRTHMONTH = ").append(getBirthMonth()).append("\n");
		str.append("BIRTHDAY = ").append(getBirthDay()).append("\n");
		str.append("HOMEPROVINCE = ").append(getHomeProvince()).append("\n");
		str.append("HOMECITY = ").append(getHomeCity()).append("\n");
		str.append("HIGHSCHOOL = ").append(getHighSchool()).append("\n");
		str.append("ASTROLOGY = ").append(getAstrology()).append("\n");
		str.append("HEADURL = ").append(getHeadUrl()).append("\n");
		str.append("TINYURL = ").append(getTinyUrl()).append("\n");
		str.append("BASICCONTROL = ").append(getBasicControl()).append("\n");
		str.append("STAR = ").append(getStar()).append("\n");
		str.append("DOING = ").append(getDoing()).append("\n");
		str.append("STAGE = ").append(getStage()).append("\n");
		// str.append("CUR_NETWORK_ID =
		// ").append(getCurNetworkId()).append("\n");
		// str.append("CUR_NETWORK_NAME =
		// ").append(getCurNetworkName()).append("\n");
		// str.append("JOIN_YEAR = ").append(getJoinYear()).append("\n");
		str.append("ROOM = ").append(getRoom()).append("\n");
		return (str.toString());
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		// id will not update.
		// this.properties.put(this.ID,String.valueOf(this.id));
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
		mark(this.NAME, this.name);
	}

	public int getSelected() {
		return selected;
	}

	public void setSelected(int selected) {
		this.selected = selected;
		mark(this.SELECTED, String.valueOf(this.selected));
	}

	public String getAstrology() {
		return astrology;
	}

	public void setAstrology(String astrology) {
		this.astrology = astrology;
		mark(this.ASTROLOGY, this.astrology);
	}

	public int getBasicControl() {
		return basicControl;
	}

	public void setBasicControl(int basicControl) {
		this.basicControl = basicControl;
		mark(this.BASICCONTROL,
				String.valueOf(this.basicControl));
	}

	public String getDepartment() {
		return department;
	}

	public void setDepartment(String department) {
		this.department = department;
		mark(this.DEPARTMENT, this.department);
	}

	public String getDorm() {
		return dorm;
	}

	public void setDorm(String dorm) {
		this.dorm = dorm;
		mark(this.DORM, this.dorm);
	}

	public String getGender() {
		return gender;
	}

	public void setGender(String gender) {
		this.gender = gender;
		mark(this.GENDER, this.gender);
	}

	public String getHighSchool() {
		return highSchool;
	}

	public void setHighSchool(String highSchool) {
		this.highSchool = highSchool;
		mark(this.HIGHSCHOOL, this.highSchool);
	}

	public String getHomeProvince() {
		return homeProvince;
	}

	public void setHomeProvince(String homeProvince) {
		this.homeProvince = homeProvince;
		mark(this.HOMEPROVINCE, this.homeProvince);
	}

	public int getStatus() {
		return status;
	}

	public void setStatus(int status) {
		this.status = status;
		mark(this.STATUS, String.valueOf(this.status));
	}

	public String getHeadUrl() {
		return headUrl;
	}

	public void setHeadUrl(String headUrl) {
		this.headUrl = headUrl;
		mark(this.HEADURL, this.headUrl);
	}

	public String getHomeCity() {
		return homeCity;
	}

	public void setHomeCity(String homeCity) {
		this.homeCity = homeCity;
		mark(this.HOMECITY, this.homeCity);
	}

	public String getTinyUrl() {
		return tinyUrl;
	}

	public void setTinyUrl(String tinyUrl) {
		this.tinyUrl = tinyUrl;
		mark(this.TINYURL, this.tinyUrl);
	}

	public int getUniv() {
		return univ;
	}

	public void setUniv(int univ) {
		this.univ = univ;
		mark(UserBasic.UNIV, String.valueOf(this.univ));
	}

	public int getStar() {
		return star;
	}

	public void setStar(int star) {
		this.star = star;
		mark(UserBasic.STAR, String.valueOf(this.star));
	}

	public int getBirthDay() {
		return birthDay;
	}

	public void setBirthDay(int birthDay) {
		this.birthDay = birthDay;
		mark(UserBasic.BIRTHDAY, String.valueOf(this.birthDay));
	}

	public int getBirthMonth() {
		return birthMonth;
	}

	public void setBirthMonth(int birthMonth) {
		this.birthMonth = birthMonth;
		mark(UserBasic.BIRTHMONTH,
				String.valueOf(this.birthMonth));
	}

	public int getBirthYear() {
		return birthYear;
	}

	public void setBirthYear(int birthYear) {
		this.birthYear = birthYear;
		mark(UserBasic.BIRTHYEAR,
				String.valueOf(this.birthYear));
	}

	public int getUnivYear() {
		return univYear;
	}

	public void setUnivYear(int univYear) {
		this.univYear = univYear;
		mark(UserBasic.UNIVYEAR, String.valueOf(this.univYear));
	}

	public UserBasic parse(Map<String, String> props) {
		this.id = Integer.parseInt(props.get(UserBasic.ID));
		
		this.astrology = props.get(UserBasic.ASTROLOGY);
		this.basicControl = Integer.parseInt(props.get(UserBasic.BASICCONTROL));
		try {
			this.birthDay = Integer.parseInt(props.get(UserBasic.BIRTHDAY));
		} catch (NumberFormatException e) {
			this.birthDay = 1;
		}
		try {
			this.birthMonth = Integer.parseInt(props.get(UserBasic.BIRTHMONTH));
		} catch (NumberFormatException e) {
			this.birthMonth = 1;
		}
		try {
			this.birthYear = Integer.parseInt(props.get(UserBasic.BIRTHYEAR));
		} catch (NumberFormatException e) {
			this.birthYear = 1980;
		}
		this.department = props.get(UserBasic.DEPARTMENT);
		this.dorm = props.get(UserBasic.DORM);
		this.gender = props.get(UserBasic.GENDER);
		this.headUrl = props.get(UserBasic.HEADURL);
		this.highSchool = props.get(UserBasic.HIGHSCHOOL);
		this.homeCity = props.get(UserBasic.HOMECITY);
		this.homeProvince = props.get(UserBasic.HOMEPROVINCE);

		this.name = props.get(UserBasic.NAME);
		try {
			this.selected = Integer.parseInt(props.get(UserBasic.SELECTED));
		} catch (NumberFormatException e) {

		}
		try {
			this.star = Integer.parseInt(props.get(UserBasic.STAR));
		} catch (NumberFormatException e) {
			this.star = 1;
		}
		try {
			this.status = Integer.parseInt(props.get(UserBasic.STATUS));
		} catch (NumberFormatException e) {
			this.status = 2;
		}
		this.tinyUrl = props.get(UserBasic.TINYURL);
		try {
			this.univ = Integer.parseInt(props.get(UserBasic.UNIV));
		} catch (NumberFormatException e) {

		}
		try {
			this.univYear = Integer.parseInt(props.get(UserBasic.UNIVYEAR));
		} catch (NumberFormatException e) {
			this.univYear = 2004;
		}
		this.doing = props.get(UserBasic.DOING);
		this.highSchoolCode = props.get(UserBasic.HIGHSCHOOLCODE);
		this.cityCode = props.get(UserBasic.CITYCODE);
		this.doingTime = DateFormatter.parse(props.get(UserBasic.DOINGTIME));
		this.stage = Integer.parseInt(props.get(UserBasic.STAGE));
		this.room = props.get(UserBasic.ROOM);
		
		return this;
	}

	public String getDoing() {
		return this.doing;
	}

	public void setDoing(String doing) {
		this.doing = doing;
		mark(UserBasic.DOING, doing);
	}

	public String getCityCode() {
		return cityCode;
	}

	public void setCityCode(String cityCode) {
		this.cityCode = cityCode;
		mark(UserBasic.CITYCODE, String.valueOf(cityCode));
	}

	public Date getDoingTime() {
		return doingTime;
	}

	public void setDoingTime(Date doingTime) {
		this.doingTime = doingTime;
		if (doingTime == null) {
			return;
		}
		mark(UserBasic.DOINGTIME,
				DateFormatter.format(doingTime));
	}

	public String getHighSchoolCode() {
		return highSchoolCode;
	}

	public void setHighSchoolCode(String highSchoolCode) {
		this.highSchoolCode = highSchoolCode;
		mark(UserBasic.HIGHSCHOOLCODE,
				String.valueOf(highSchoolCode));
	}

	public String getRoom() {
		return room;

	}

	public void setRoom(String room) {
		this.room = room;
		mark(UserBasic.ROOM, room);
	}

	public int getStage() {
		return stage;
	}

	public void setStage(int stage) {
		this.stage = stage;
		mark(UserBasic.STAGE, String.valueOf(stage));
	}

}
