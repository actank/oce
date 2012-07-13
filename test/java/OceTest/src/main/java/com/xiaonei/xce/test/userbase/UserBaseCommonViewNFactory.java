package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userbaseview.UserBaseCommonViewFactory;


public class UserBaseCommonViewNFactory implements UserBaseCommonViewFactory<UserBaseCommonViewN> {
  public UserBaseCommonViewN create() {
    return new UserBaseCommonViewN();
  }
}
