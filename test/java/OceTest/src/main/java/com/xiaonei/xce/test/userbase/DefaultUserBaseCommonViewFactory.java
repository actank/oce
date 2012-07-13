package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userbaseview.UserBaseCommonViewFactory;


public class DefaultUserBaseCommonViewFactory implements UserBaseCommonViewFactory<DefaultUserBaseCommonView> {
  public DefaultUserBaseCommonView create() {
    return new DefaultUserBaseCommonView();
  }
}
