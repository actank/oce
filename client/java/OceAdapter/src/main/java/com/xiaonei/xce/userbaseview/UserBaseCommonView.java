package com.xiaonei.xce.userbaseview;

import com.xiaonei.xce.userdanger.UserDangerInfo;
import com.xiaonei.xce.username.UserNameInfo;
import com.xiaonei.xce.userpassport.UserPassportInfo;
import com.xiaonei.xce.userright.UserRightInfo;
import com.xiaonei.xce.userstate.UserStateInfo;
import com.xiaonei.xce.userurl.UserUrlInfo;

public abstract class UserBaseCommonView<P extends UserPassportInfo, R extends UserRightInfo, N extends UserNameInfo, S extends UserStateInfo, U extends UserUrlInfo> {

	protected P userPassport;
	protected R userRight;
	protected N userName;
	protected S userState;
	protected int userLoginCount;
	protected U userUrl;
	protected int stage;
	protected int univ;
	protected String gender;
	protected UserDangerInfo userDanger;
	protected int realUserId;
	
  public int getRealUserId() {
		return realUserId;
	}

	public void setRealUserId(int realUserId) {
		this.realUserId = realUserId;
	}

	public String getGender() {
		return gender;
	}

	public void setGender(String gender) {
		this.gender = gender;
	}

	public int getStage() {
		return stage;
	}

	public void setStage(int stage) {
		this.stage = stage;
	}

	public int getUniv() {
		return univ;
	}

	public void setUniv(int univ) {
		this.univ = univ;
	}

	public void setUserPassportInfo(P upi) {
		this.userPassport = upi;
	}

	public void setUserRightInfo(R uri) {
		this.userRight = uri;
	}

	public void setUserNameInfo(N uni) {
		this.userName = uni;
	}

	public P getUserPassportInfo() {
		return userPassport;
	}

	public R getUserRightInfo() {
		return userRight;
	}

	public N getUserNameInfo() {
		return userName;
	}

	public int getUserLoginCountInfo() {
		return userLoginCount;
	}

	public void setUserLoginCountInfo(int ulci) {
		this.userLoginCount = ulci;
	}

	public S getUserState() {
		return userState;
	}
	
	public UserDangerInfo getUserDangerInfo(){
	    return userDanger;
	}
	
	public void setUserDangerInfo(UserDangerInfo udi){
	    this.userDanger=udi;
	}

	public void setUserStateInfo(S usi) {
		this.userState = usi;
	}

	public U getUserUrl() {
		return userUrl;
	}

	public void setUserUrl(U userUrl) {
		this.userUrl = userUrl;
	}
}
