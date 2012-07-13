package com.xiaonei.xce.buddy;

import com.xiaonei.xce.buddy.relation.BuddyRelationContext;
import com.xiaonei.xce.buddy.friend.FriendContext;

/**
 * Buddy入口
 * 
 * @author 
 * 
 */
public interface BuddyAdapter {

    public BuddyRelationContext getRelationContext(); 

    public FriendContext getFriendContext();       
}
