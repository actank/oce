package com.xiaonei.xce.userborn;

import mop.hi.oce.domain.Markable;
import xce.userbase.UserBornData;

public class UserBornInfo extends Markable {

	public static final String ID = "ID";
	public static final String GENDER = "GENDER";
	public static final String BIRTHYEAR = "BIRTH_YEAR";
	public static final String BIRTHMONTH = "BIRTH_MONTH";
	public static final String BIRTHDAY = "BIRTH_DAY";
	public static final String HOMEPROVINCE = "HOME_PROVINCE";
	public static final String HOMECITY = "HOME_CITY";
	public static final String CITYCODE = "CITY_CODE";
	public static final String ASTROLOGY = "ASTROLOGY";

	protected int _id;
	protected int _birthMonth;
	protected int _birthDay;
	protected int _birthYear;
	protected String _astrology;
	protected String _cityCode;
	protected String _gender;
	protected String _homeCity;
	protected String _homeProvince;

	public UserBornInfo parse(UserBornData source) {
		_astrology=source.astrology;
		_birthDay=source.birthDay;
		_birthMonth=source.birthMonth;
		_birthYear=source.birthYear;
		_cityCode=source.cityCode;
		_gender=source.gender;
		_homeCity=source.homeCity;
		_homeProvince=source.homeProvince;
		_id = source.id;
		return this;
	}

	public int getId() {
		return _id;
	}

	public void setId(int id) {
		_id = id;
		mark(UserBornInfo.ID,String.valueOf(id));
	}

	public int getBirthMonth() {
		return _birthMonth;
	}

	public void setBirthMonth(int birthMonth) {
		_birthMonth = birthMonth;
		mark(UserBornInfo.BIRTHMONTH,String.valueOf(birthMonth));
	}

	public int getBirthDay() {
		return _birthDay;
	}

	public void setBirthDay(int birthDay) {
		_birthDay = birthDay;
		mark(UserBornInfo.BIRTHDAY,String.valueOf(birthDay));
	}

	@Deprecated
	public int get_birthYear() {
		return _birthYear;
	}

	public int getBirthYear() {
		return _birthYear;
	}

	public void setBirthYear(int birthYear) {
		_birthYear = birthYear;
		mark(UserBornInfo.BIRTHYEAR,String.valueOf(birthYear));
	}

	public String getAstrology() {
		return _astrology;
	}

	public void setAstrology(String astrology) {
		_astrology = astrology;
		mark(UserBornInfo.ASTROLOGY,astrology);
	}

	public String getCityCode() {
		return _cityCode;
	}

	public void setCityCode(String cityCode) {
		_cityCode = cityCode;
		mark(UserBornInfo.CITYCODE,cityCode);
	}

	public String getGender() {
		return _gender;
	}

	public void setGender(String gender) {
		_gender = gender;
		mark(UserBornInfo.GENDER,gender);
	}

	public String getHomeCity() {
		return _homeCity;
	}

	public void setHomeCity(String homeCity) {
		_homeCity = homeCity;
		mark(UserBornInfo.HOMECITY,homeCity);
	}

	public String getHomeProvince() {
		return _homeProvince;
	}

	public void setHomeProvince(String homeProvince) {
		_homeProvince = homeProvince;
		mark(UserBornInfo.HOMEPROVINCE,homeProvince);
	}

	
}
