package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userbaseview.UserBaseCommonViewAdapter;
import com.xiaonei.xce.username.DefaultUserNameFactory;
import com.xiaonei.xce.username.DefaultUserNameInfo;
import com.xiaonei.xce.userpassport.DefaultUserPassportFactory;
import com.xiaonei.xce.userpassport.DefaultUserPassportInfo;
import com.xiaonei.xce.userright.DefaultUserRightFactory;
import com.xiaonei.xce.userright.DefaultUserRightInfo;
import com.xiaonei.xce.userstate.DefaultUserStateFactory;
import com.xiaonei.xce.userstate.DefaultUserStateInfo;
import com.xiaonei.xce.userstate.UserStateInfoN;
import com.xiaonei.xce.userstate.UserStateInfoNFactory;
import com.xiaonei.xce.userurl.DefaultUserUrlFactory;
import com.xiaonei.xce.userurl.DefaultUserUrlInfo;

public class TestUserBaseCommonViewAdapter {
  public static void main(String args[]) {
    UserBaseCommonViewAdapter<DefaultUserBaseCommonView, DefaultUserPassportInfo, DefaultUserRightInfo, 
        DefaultUserNameInfo, DefaultUserStateInfo, DefaultUserUrlInfo> adapter = 
        new UserBaseCommonViewAdapter<DefaultUserBaseCommonView, DefaultUserPassportInfo, DefaultUserRightInfo, 
        DefaultUserNameInfo, DefaultUserStateInfo, DefaultUserUrlInfo>(new DefaultUserBaseCommonViewFactory(),
            new DefaultUserPassportFactory(),
            new DefaultUserRightFactory(), new DefaultUserNameFactory(), new DefaultUserStateFactory(),
            new DefaultUserUrlFactory());
    DefaultUserBaseCommonView userBaseCommonView = adapter.getUserBaseView(67913);
    System.out.println("-----------------------------------------------------------------------");
    System.out.println("isApplySpammer : " + userBaseCommonView.getUserState().isApplySpammer());
    UserBaseCommonViewAdapter<UserBaseCommonViewN, DefaultUserPassportInfo, DefaultUserRightInfo, 
    DefaultUserNameInfo, UserStateInfoN, DefaultUserUrlInfo> adapterN = 
    new UserBaseCommonViewAdapter<UserBaseCommonViewN, DefaultUserPassportInfo, DefaultUserRightInfo, 
    DefaultUserNameInfo, UserStateInfoN, DefaultUserUrlInfo>(new UserBaseCommonViewNFactory(),
        new DefaultUserPassportFactory(),
        new DefaultUserRightFactory(), new DefaultUserNameFactory(), new UserStateInfoNFactory(),
        new DefaultUserUrlFactory());
    UserBaseCommonViewN userBaseCommonViewN = adapterN.getUserBaseView(67913);
    System.out.println("isRenRenUser : " + 
        userBaseCommonViewN.getUserState().isStatusOn(StateFlag.IS_RENREN_USER));
    System.exit(0);
  }
}
