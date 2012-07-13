package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userbasic.DefaultUserBasicFactory;
import com.xiaonei.xce.userbasic.UserBasicAdapter;
import com.xiaonei.xce.userbasic.UserBasicInfo;
import com.xiaonei.xce.userbasic.UserBasicInfoN;
import com.xiaonei.xce.userbasic.UserBasicInfoNFactory;

public class TestUserBasicAdapter {
  public static void main(String args[]) {
    UserBasicAdapter<UserBasicInfo> adapter = 
      new UserBasicAdapter(new DefaultUserBasicFactory());
    try {
      UserBasicInfo basicInfo = adapter.getUserBasic(67913);
      System.out.println(basicInfo.getBirthDay() + " " + basicInfo.getName());
    } catch (Exception e) {
      e.printStackTrace();
    }
    
    UserBasicAdapter<UserBasicInfoN> adapterN = 
      new UserBasicAdapter(new UserBasicInfoNFactory());
    try {
      UserBasicInfoN basicInfoN = adapterN.getUserBasic(67913);
      System.out.println(basicInfoN.getBirthDay() + " " + basicInfoN.getName()
          + " " + basicInfoN.getIsLunarBirth());
    } catch (Exception e) {
      e.printStackTrace();
    }
    System.exit(0);
  }
}
