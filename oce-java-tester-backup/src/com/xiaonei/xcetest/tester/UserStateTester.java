package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;
import com.xiaonei.xce.userstate.UserStateAdapter;
import com.xiaonei.xce.userstate.UserState;

public class UserStateTester extends TestCase {

	private UserStateAdapter _usa = new UserStateAdapter();

	int userId = 202911262;

	public void get() {
		UserState props = _usa.getUserState(userId);
		System.out.println(props);
		System.out.println("isSelected : " + props.isSelected()+ "\tisVipMember : " + props.isVipMember());
	}

	public void setSelected() {
		get();
		_usa.setSelectedOn(userId);
		get();
	}

	public void setVipMember() {
		get();
		_usa.setVipMemberOn(userId);
		get();
	}
}
