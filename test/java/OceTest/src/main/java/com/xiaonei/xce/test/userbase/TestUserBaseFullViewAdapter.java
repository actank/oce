package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userbaseview.DefaultUserBaseFullView;
import com.xiaonei.xce.userbaseview.DefaultUserBaseFullViewFactory;
import com.xiaonei.xce.userbaseview.UserBaseFullViewAdapter;
import com.xiaonei.xce.userbaseview.UserBaseFullViewN;
import com.xiaonei.xce.userbaseview.UserBaseFullViewNFactory;
import com.xiaonei.xce.userbasic.DefaultUserBasicFactory;
import com.xiaonei.xce.userbasic.DefaultUserBasicInfo;
import com.xiaonei.xce.userbasic.UserBasicInfoN;
import com.xiaonei.xce.userbasic.UserBasicInfoNFactory;
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

public class TestUserBaseFullViewAdapter {
  public static void main(String args[]) {
    UserBaseFullViewAdapter<DefaultUserBaseFullView, DefaultUserPassportInfo, DefaultUserRightInfo, 
    DefaultUserNameInfo, DefaultUserStateInfo, DefaultUserBasicInfo, DefaultUserUrlInfo> adapter = 
    new UserBaseFullViewAdapter<DefaultUserBaseFullView, DefaultUserPassportInfo, DefaultUserRightInfo, 
    DefaultUserNameInfo, DefaultUserStateInfo, DefaultUserBasicInfo, 
        DefaultUserUrlInfo>(new DefaultUserBaseFullViewFactory(),
        new DefaultUserPassportFactory(),
        new DefaultUserRightFactory(), new DefaultUserNameFactory(), new DefaultUserStateFactory(),
        new DefaultUserBasicFactory(),
        new DefaultUserUrlFactory());
    DefaultUserBaseFullView defaulBaseFullView = null;
    try {
      System.out.println("-------------------------------------------------");
      defaulBaseFullView = adapter.getUserBaseView(67913);
      System.out.println(defaulBaseFullView.getUserState().isGiftMember());
    } catch (Exception ex) {
      System.out.println(ex);
    }
    
    UserBaseFullViewAdapter<UserBaseFullViewN, DefaultUserPassportInfo, DefaultUserRightInfo, 
    DefaultUserNameInfo, UserStateInfoN, UserBasicInfoN, DefaultUserUrlInfo> adapterN = 
    new UserBaseFullViewAdapter<UserBaseFullViewN, DefaultUserPassportInfo, DefaultUserRightInfo, 
    DefaultUserNameInfo, UserStateInfoN, UserBasicInfoN, 
        DefaultUserUrlInfo>(new UserBaseFullViewNFactory(),
        new DefaultUserPassportFactory(),
        new DefaultUserRightFactory(), new DefaultUserNameFactory(), new UserStateInfoNFactory(),
        new UserBasicInfoNFactory(),
        new DefaultUserUrlFactory());
    UserBaseFullViewN baseFullView = null;
    try {
      System.out.println("-------------------------------------------------");
      baseFullView = adapterN.getUserBaseView(67913);
      System.out.println(baseFullView.getUserState().isStatusOn(StateFlag.IS_RENREN_USER));
    } catch (Exception ex) {
      System.out.println(ex);
    }
    System.exit(0);
  }
}
