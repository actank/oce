package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userpassport.UserPassportFactory;
import com.xiaonei.xce.userpassport.UserPassportInfo;
import com.xiaonei.xce.userright.UserRightFactory;
import com.xiaonei.xce.userright.UserRightInfo;
import com.xiaonei.xce.userstate.UserStateFactory;
import com.xiaonei.xce.userstate.UserStateInfo;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;
import com.xiaonei.xce.username.UserNameFactory;
import com.xiaonei.xce.username.UserNameInfo;
import com.xiaonei.xce.userborn.UserBornFactory;
import com.xiaonei.xce.userborn.UserBornInfo;
import com.xiaonei.xce.userstage.UserStageFactory;
import com.xiaonei.xce.userstage.UserStageInfo;
import com.xiaonei.xce.userdoing.UserDoingFactory;
import com.xiaonei.xce.userdoing.UserDoingInfo;
import com.xiaonei.xce.userbasic.UserBasicFactory;
import com.xiaonei.xce.userbasic.UserBasicInfo;
import com.xiaonei.xce.userbaseview.UserBaseFullViewFactory;
import com.xiaonei.xce.userbaseview.UserBaseFullView;
import com.xiaonei.xce.userbaseview.UserBaseCommonViewFactory;
import com.xiaonei.xce.userbaseview.UserBaseCommonView;

class MUserPassportInfo extends UserPassportInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "account = " + this.getAccount() + "\n";
		result += "domain = " + this.getDomain() + "\n";
		result += "password_md5 = " + this.getPasswordMd5() + "\n";
		result += "status = " + this.getStatus() + "\n";
		result += "safe_status = " + this.getSafeStatus() + "\n";

		return result;
	}
}

class MUserPassportFactory implements UserPassportFactory<MUserPassportInfo> {
	public MUserPassportInfo create() {
		return new MUserPassportInfo();
	}
}

class MUserStateInfo extends UserStateInfo {
	private static final long serialVersionUID = 8234650290181069888L;
	
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "state = " + this.getState() + "\n";
		result += "level = " + this.getLevel() + "\n";
		result += "star = " + this.getStar() + "\n";

		return result;
	}
}

class MUserStateFactory implements UserStateFactory<MUserStateInfo> {
	public MUserStateInfo create() {
		return new MUserStateInfo();
	}
}

class MUserUrlInfo extends UserUrlInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "largeurl = " + this.getLargeUrl() + "\n";
		result += "mainurl = " + this.getMainUrl() + "\n";
		result += "headurl = " + this.getHeadUrl() + "\n";
		result += "tinyurl = " + this.getTinyUrl() + "\n";

		return result;
	}
}

class MUserUrlFactory implements UserUrlFactory<MUserUrlInfo> {
	public MUserUrlInfo create() {
		return new MUserUrlInfo();
	}
}

class MUserNameInfo extends UserNameInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "name = " + this.getName() + "\n";
		result += "nickname = " + this.getNickname() + "\n";

		return result;
	}
}

class MUserNameFactory implements UserNameFactory<MUserNameInfo> {
	public MUserNameInfo create() {
		return new MUserNameInfo();
	}
}

class MUserBornInfo extends UserBornInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "birth_day = " + this.getBirthDay() + "\n";
		result += "birth_month = " + this.getBirthMonth() + "\n";
		result += "birth_year = " + this.getBirthYear() + "\n";
		result += "astrology = " + this.getAstrology() + "\n";
		result += "gender = " + this.getGender() + "\n";
		result += "home_province = " + this.getHomeProvince() + "\n";
		result += "home_city = " + this.getHomeCity() + "\n";
		result += "city_code = " + this.getCityCode() + "\n";
		result += "is_lunar_birth = " + this.isLunarBirth() + "\n";

		return result;
	}
}

class MUserBornFactory implements UserBornFactory<MUserBornInfo> {
	public MUserBornInfo create() {
		return new MUserBornInfo();
	}
}

class MUserStageInfo extends UserStageInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "univ = " + this.getUniv() + "\n";
		result += "stage = " + this.getStage() + "\n";

		return result;
	}
}

class MUserStageFactory implements UserStageFactory<MUserStageInfo> {
	public MUserStageInfo create() {
		return new MUserStageInfo();
	}
}

class MUserDoingInfo extends UserDoingInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "doing = " + this.getDoing() + "\n";
		result += "doingtime = " + this.getDoingTime() + "\n";

		return result;
	}
}

class MUserDoingFactory implements UserDoingFactory<MUserDoingInfo> {
	public MUserDoingInfo create() {
		return new MUserDoingInfo();
	}
}

class MUserRightInfo extends UserRightInfo {
	public String toString() {
		String result = new String("");

		result += "id = " + this.getId() + "\n";
		result += "auth = " + this.getAuth() + "\n";
		result += "common_rights = " + this.getRights() + "\n";

		return result;
	}
}

class MUserRightFactory implements UserRightFactory<MUserRightInfo> {
	public MUserRightInfo create() {
		return new MUserRightInfo();
	}
}

class MUserBasicInfo extends UserBasicInfo {
	private static final long serialVersionUID = 8234650290181069444L;
}

class MUserBasicFactory implements UserBasicFactory<MUserBasicInfo> {
	public MUserBasicInfo create() {
		return new MUserBasicInfo();
	}
}

class MUserBaseFullView extends UserBaseFullView<MUserPassportInfo, MUserRightInfo, MUserNameInfo, MUserStateInfo, MUserBasicInfo, MUserUrlInfo> {
	public String toString(){
		String result = new String("");

		result += "\nUserNameInfo:\n--------\n" + this.getUserNameInfo().toString();
		result += "\nUserPassportInfo:\n--------\n" + this.getUserPassportInfo().toString();
		result += "\nUserRightInfo:\n--------\n" + this.getUserRightInfo().toString();
		result += "\nUserStateInfo:\n--------\n" + this.getUserState().toString();
		result += "\nUserUrlInfo:\n--------\n" + this.getUserUrl().toString();
		result += "\nUserBasic:\n--------\n" + this.getUserBasic().toString();
		result += "\nUserLoginCountInfo:\n--------\n" + this.getUserLoginCountInfo();
		
		return result;
	}
}

class MUserBaseFullViewFactory implements UserBaseFullViewFactory<MUserBaseFullView> {
	public MUserBaseFullView create() {
		return new MUserBaseFullView();
	}
}

class MUserBaseCommonView extends UserBaseCommonView<MUserPassportInfo, MUserRightInfo, MUserNameInfo, MUserStateInfo, MUserUrlInfo> {
	public String toString(){
		String result = new String("");

		result += "\nUserNameInfo:\n--------\n" + this.getUserNameInfo().toString();
		result += "\nUserPassportInfo:\n--------\n" + this.getUserPassportInfo().toString();
		result += "\nUserRightInfo:\n--------\n" + this.getUserRightInfo().toString();
		result += "\nUserStateInfo:\n--------\n" + this.getUserState().toString();
		result += "\nUserUrlInfo:\n--------\n" + this.getUserUrl().toString();
		result += "\nUserLoginCountInfo:\n--------\n" + this.getUserLoginCountInfo();
		result += "\n\nstage:\n--------\n" + this.getStage();
		result += "\n\nuniv:\n--------\n" + this.getUniv();
		result += "\n\ngender:\n--------\n" + this.getGender();
		
		return result;
	}
}

class MUserBaseCommonViewFactory implements UserBaseCommonViewFactory<MUserBaseCommonView> {
	public MUserBaseCommonView create() {
		return new MUserBaseCommonView();
	}
}