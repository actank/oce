package com.xiaonei.xce.buddyadapter.buddycount;

/**
 * BuddyCount类型
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public interface BuddyCountType {
    /** 发达好友申请 */
    int ADD_FRIEND_APPLY = 1;
    /** 忽略好友申请 */
    int REMOVE_FRIEND_APPLY = 2;
    /** 接受好友申请 */
    int ACCEPT_FRIEND_APPLY = 3;
    /** 加特别好友 */
    int ADD_HOT_FRIEND = 4;
    /** 删除特别好友 */
    int REMOVE_HOT_FRIEND = 5;
}
