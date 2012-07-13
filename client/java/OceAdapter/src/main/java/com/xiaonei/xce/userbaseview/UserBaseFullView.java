package com.xiaonei.xce.userbaseview;

import com.xiaonei.xce.userbasic.UserBasicInfo;
import com.xiaonei.xce.username.UserNameInfo;
import com.xiaonei.xce.userpassport.UserPassportInfo;
import com.xiaonei.xce.userright.UserRightInfo;
import com.xiaonei.xce.userstate.UserStateInfo;
import com.xiaonei.xce.userurl.UserUrlInfo;

public abstract class UserBaseFullView<P extends UserPassportInfo, R extends UserRightInfo, N extends UserNameInfo, S extends UserStateInfo, B extends UserBasicInfo, U extends UserUrlInfo> {

	protected P userPassport;
	protected R userRight;
	protected N userName;
	protected S userState;
	protected int userLoginCount;
	protected B userBasic;
	protected U userUrl;
	
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

	public void setUserStateInfo(S usi) {
		this.userState = usi;
	}

	public B getUserBasic() {
		return userBasic;
	}

	public void setUserBasic(B userBasic) {
		this.userBasic = userBasic;
	}

	public U getUserUrl() {
		return userUrl;
	}

	public void setUserUrl(U userUrl) {
		this.userUrl = userUrl;
	}
}
