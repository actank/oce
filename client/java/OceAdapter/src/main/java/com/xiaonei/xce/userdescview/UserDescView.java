package com.xiaonei.xce.userdescview;

import com.xiaonei.xce.usercontact.UserContactInfo;
import com.xiaonei.xce.userfav.UserFavInfo;
import com.xiaonei.xce.usertime.UserTimeInfo;

public class UserDescView<T extends UserTimeInfo, C extends UserContactInfo, F extends UserFavInfo> {

	protected T userTimeView;

	protected C userContactView;

	protected F userFavView;


	public T getUserTimeView() {
		return userTimeView;
	}

	public void setUserTimeView(T userTimeView) {
		this.userTimeView = userTimeView;
	}

	public C getUserContactView() {
		return userContactView;
	}

	public void setUserContactView(C userContactView) {
		this.userContactView = userContactView;
	}

	public F getUserFavView() {
		return userFavView;
	}

	public void setUserFavView(F userFavView) {
		this.userFavView = userFavView;
	}
}
