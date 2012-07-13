package com.xiaonei.xce.userbasic;

import java.io.Serializable;
import java.util.Date;
import java.util.Map;

import mop.hi.oce.domain.Markable;
import xce.userbase.UserBasicData;
import xce.util.tools.DateFormatter;

public class UserBasicInfo extends Markable implements Serializable {
	private static final long serialVersionUID = 8234650290181069541L;

	public static final String ID = "ID";

	public static final String UNIV = "UNIV";

	public static final String NAME = "NAME";

	public static final String STATUS = "STATUS";

	public static final String ONLINESTATUS = "ONLINESTATUS";

	public static final String GENDER = "GENDER";

	public static final String BIRTHYEAR = "BIRTH_YEAR";

	public static final String BIRTHMONTH = "BIRTH_MONTH";

	public static final String BIRTHDAY = "BIRTH_DAY";

	public static final String HOMEPROVINCE = "HOME_PROVINCE";

	public static final String HOMECITY = "HOME_CITY";

	public static final String CITYCODE = "CITY_CODE";

	public static final String ASTROLOGY = "ASTROLOGY";

	public static final String HEADURL = "HEADURL";

	public static final String TINYURL = "TINYURL";

	public static final String STAR = "STAR";

	public static final String DOING = "DOING";

	public static final String DOINGTIME = "DOINGTIME";

	public static final String STAGE = "STAGE";

	protected int id;

	protected String name;

	protected int univ;

	protected int status;

	protected String gender;

	protected String homeProvince;

	protected String homeCity;

	protected String cityCode;

	protected int birthYear;

	protected int birthMonth;

	protected int birthDay;

	protected String astrology;

	protected int star;

	protected String headUrl;

	protected String tinyUrl;

	protected String doing;

	protected Date doingTime;

	protected int stage;

	public String toString() {
		StringBuffer str = new StringBuffer();
		str.append("UserBasicInfo:\n");
		str.append("ID = ").append(this.getId()).append("\n");
		str.append("UNIV = ").append(getUniv()).append("\n");
		str.append("NAME = ").append(getName()).append("\n");
		str.append("STATUS = ").append(getStatus()).append("\n");
		str.append("GENDER = ").append(getGender()).append("\n");
		str.append("BIRTHYEAR = ").append(getBirthYear()).append("\n");
		str.append("BIRTHMONTH = ").append(getBirthMonth()).append("\n");
		str.append("BIRTHDAY = ").append(getBirthDay()).append("\n");
		str.append("HOMEPROVINCE = ").append(getHomeProvince()).append("\n");
		str.append("HOMECITY = ").append(getHomeCity()).append("\n");
		str.append("ASTROLOGY = ").append(getAstrology()).append("\n");
		str.append("HEADURL = ").append(getHeadUrl()).append("\n");
		str.append("TINYURL = ").append(getTinyUrl()).append("\n");
		str.append("STAR = ").append(getStar()).append("\n");
		str.append("DOING = ").append(getDoing()).append("\n");
		str.append("STAGE = ").append(getStage()).append("\n");
		return (str.toString());
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		mark(ID, String.valueOf(this.id));
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
		mark(NAME, this.name);
	}

	public String getAstrology() {
		return astrology;
	}

	public void setAstrology(String astrology) {
		this.astrology = astrology;
		mark(ASTROLOGY, this.astrology);
	}

	public String getGender() {
		return gender;
	}

	public void setGender(String gender) {
		this.gender = gender;
		mark(GENDER, this.gender);
	}

	public String getHomeProvince() {
		return homeProvince;
	}

	public void setHomeProvince(String homeProvince) {
		this.homeProvince = homeProvince;
		mark(HOMEPROVINCE, this.homeProvince);
	}

	public int getStatus() {
		return status;
	}

	public void setStatus(int status) {
		this.status = status;
		mark(STATUS, String.valueOf(this.status));
	}

	public String getHeadUrl() {
		return headUrl;
	}

	public void setHeadUrl(String headUrl) {
		this.headUrl = headUrl;
		mark(HEADURL, this.headUrl);
	}

	public String getHomeCity() {
		return homeCity;
	}

	public void setHomeCity(String homeCity) {
		this.homeCity = homeCity;
		mark(HOMECITY, this.homeCity);
	}

	public String getTinyUrl() {
		return tinyUrl;
	}

	public void setTinyUrl(String tinyUrl) {
		this.tinyUrl = tinyUrl;
		mark(TINYURL, this.tinyUrl);
	}

	public int getUniv() {
		return univ;
	}

	public void setUniv(int univ) {
		this.univ = univ;
		mark(UserBasicInfo.UNIV, String.valueOf(this.univ));
	}

	public int getStar() {
		return star;
	}

	public void setStar(int star) {
		this.star = star;
		mark(UserBasicInfo.STAR, String.valueOf(this.star));
	}

	public int getBirthDay() {
		return birthDay;
	}

	public void setBirthDay(int birthDay) {
		this.birthDay = birthDay;
		mark(UserBasicInfo.BIRTHDAY, String.valueOf(this.birthDay));
	}

	public int getBirthMonth() {
		return birthMonth;
	}

	public void setBirthMonth(int birthMonth) {
		this.birthMonth = birthMonth;
		mark(UserBasicInfo.BIRTHMONTH, String.valueOf(this.birthMonth));
	}

	public int getBirthYear() {
		return birthYear;
	}

	public void setBirthYear(int birthYear) {
		this.birthYear = birthYear;
		mark(UserBasicInfo.BIRTHYEAR, String.valueOf(this.birthYear));
	}

	public UserBasicInfo parse(Map<String, String> props) {
		this.id = Integer.parseInt(props.get(UserBasicInfo.ID));

		this.astrology = props.get(UserBasicInfo.ASTROLOGY);
		try {
			this.birthDay = Integer.parseInt(props.get(UserBasicInfo.BIRTHDAY));
		} catch (NumberFormatException e) {
			this.birthDay = 1;
		}
		try {
			this.birthMonth = Integer.parseInt(props
					.get(UserBasicInfo.BIRTHMONTH));
		} catch (NumberFormatException e) {
			this.birthMonth = 1;
		}
		try {
			this.birthYear = Integer.parseInt(props
					.get(UserBasicInfo.BIRTHYEAR));
		} catch (NumberFormatException e) {
			this.birthYear = 1980;
		}
		this.gender = props.get(UserBasicInfo.GENDER);
		this.headUrl = props.get(UserBasicInfo.HEADURL);
		this.homeCity = props.get(UserBasicInfo.HOMECITY);
		this.homeProvince = props.get(UserBasicInfo.HOMEPROVINCE);

		this.name = props.get(UserBasicInfo.NAME);
		try {
			this.star = Integer.parseInt(props.get(UserBasicInfo.STAR));
		} catch (NumberFormatException e) {
			this.star = 1;
		}
		try {
			this.status = Integer.parseInt(props.get(UserBasicInfo.STATUS));
		} catch (NumberFormatException e) {
			this.status = 2;
		}
		this.tinyUrl = props.get(UserBasicInfo.TINYURL);
		try {
			this.univ = Integer.parseInt(props.get(UserBasicInfo.UNIV));
		} catch (NumberFormatException e) {

		}
		this.doing = props.get(UserBasicInfo.DOING);
		this.cityCode = props.get(UserBasicInfo.CITYCODE);
		this.doingTime = DateFormatter
				.parse(props.get(UserBasicInfo.DOINGTIME));
		this.stage = Integer.parseInt(props.get(UserBasicInfo.STAGE));

		return this;
	}

	public String getDoing() {
		return this.doing;
	}

	public void setDoing(String doing) {
		this.doing = doing;
		mark(UserBasicInfo.DOING, doing);
	}

	public String getCityCode() {
		return cityCode;
	}

	public void setCityCode(String cityCode) {
		this.cityCode = cityCode;
		mark(UserBasicInfo.CITYCODE, String.valueOf(cityCode));
	}

	public Date getDoingTime() {
		return doingTime;
	}

	public void setDoingTime(Date doingTime) {
		this.doingTime = doingTime;
		if (doingTime == null||doingTime.getTime() == 0) {
			return;
		}
		mark(UserBasicInfo.DOINGTIME, DateFormatter.format(doingTime));
	}

	public int getStage() {
		return stage;
	}

	public void setStage(int stage) {
		this.stage = stage;
		mark(UserBasicInfo.STAGE, String.valueOf(stage));
	}

	public UserBasicInfo parse(UserBasicData source) {
		id = source.id;

		name = source.name;

		univ = source.univ;

		status = source.status;

		gender = source.gender;

		homeProvince = source.homeProvince;

		homeCity = source.homeCity;

		cityCode = source.cityCode;

		birthYear = source.birthYear;

		birthMonth = source.birthMonth;

		birthDay = source.birthDay;

		astrology = source.astrology;

		star = source.star;

		headUrl = source.headUrl;

		tinyUrl = source.tinyUrl;

		doing = source.doing;

		// doingTime = DateFormatter.parse(source.doingTime);
		doingTime = new Date(source.doingTime * 1000l);
		stage = source.stage;

		return this;
	}

}
