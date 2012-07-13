package com.xiaonei.xce.test.userbase;

import com.xiaonei.xce.userstate.UserStateAdapter;
import com.xiaonei.xce.userstate.UserStateInfoN;
import com.xiaonei.xce.userstate.UserStateInfoNFactory;

public class TestUserStateAdapter {
  public static void main(String args[]) {
    UserStateAdapter<UserStateInfoN> adapter = 
      new UserStateAdapter<UserStateInfoN>(new UserStateInfoNFactory());
    UserStateInfoN stateInfo = (UserStateInfoN) adapter.getUserState(67913);
    System.out.println("id = " + stateInfo.getId() + ", star = " + stateInfo.getStar()
        + ", state = " + stateInfo.getStateN() + ", level = " + stateInfo.getLevel());
    System.out.println("isGiftMember(before set): " + stateInfo.isGiftMember());
    adapter.setGiftMemberOn(67913);
    stateInfo = (UserStateInfoN) adapter.getUserState(67913);
    System.out.println("isGiftMember(after set): " + stateInfo.isGiftMember());
    
    System.out.println("isRenRenUser(before set) : " + stateInfo.isStatusOn(StateFlag.IS_RENREN_USER));
    adapter.setStatus(67913, StateFlag.IS_RENREN_USER, true);
    stateInfo = (UserStateInfoN) adapter.getUserState(67913);
    System.out.println("isRenRenUser(after set) : " + stateInfo.isStatusOn(StateFlag.IS_RENREN_USER));
    System.exit(0);
  }
}
