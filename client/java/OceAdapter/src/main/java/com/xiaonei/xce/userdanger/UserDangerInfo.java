package com.xiaonei.xce.userdanger;


public class UserDangerInfo {
    protected int _id;
    public UserDangerInfo(int id){
	_id=id;
    }
    public boolean isNotFeed() {
	return UserDangerAdapter.getInstance().isNotFeed(_id);
    }
    public boolean isNotAcceptBuddyApplication() {
	return UserDangerAdapter.getInstance().isNotAcceptBuddyApplication(_id);
    }
    public boolean isOnlySeenByFriend() {
	return UserDangerAdapter.getInstance().isOnlySeenByFriend(_id);
    }
    public boolean isOnlySeenBySelf() {
	return UserDangerAdapter.getInstance().isOnlySeenBySelf(_id);
    }
    public boolean isUGCOnlySeenBySelf() {
	return UserDangerAdapter.getInstance().isUGCOnlySeenBySelf(_id);
    }
    public boolean isUGCAllCheck() {
	return UserDangerAdapter.getInstance().isUGCAllCheck(_id);
    }
    public boolean isSearchShielding() {
	return UserDangerAdapter.getInstance().isSearchShielding(_id);
    }
    public boolean isMailSubscriptionShielding() {
	return UserDangerAdapter.getInstance().isMailSubscriptionShielding(_id);
    }
    public boolean isUgcAuditFirst() {
  return UserDangerAdapter.getInstance().isUgcAuditFirst(_id);
    }
    public boolean isMiniGroupHide() {
  return UserDangerAdapter.getInstance().isMiniGroupHide(_id);
    }
}
