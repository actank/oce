package com.xiaonei.xce.buddy.impl;

import com.xiaonei.xce.buddy.BuddyAdapter;
import com.xiaonei.xce.buddy.relation.BuddyRelationContext;
import com.xiaonei.xce.buddy.relation.impl.BuddyRelationContextImpl;
import com.xiaonei.xce.buddy.friend.FriendContext;
import com.xiaonei.xce.buddy.friend.impl.FriendContextImpl;

/**
 * Buddy老大! 一切都是从这里开始
 * 
 * @author
 * 
 */
public class BuddyAdapterImpl implements BuddyAdapter {

	private BuddyRelationContext relationContext = null;
	private FriendContext friendContext = null;

	public BuddyRelationContext getRelationContext() {
		if (relationContext == null) {
			synchronized (this) {
				if (relationContext == null) {
					relationContext = new BuddyRelationContextImpl();
				}
			}
		}
		return relationContext;
	}

	@Override
	public FriendContext getFriendContext() {
		// TODO Auto-generated method stub
		if (friendContext == null) {
			synchronized (this) {
				if (friendContext == null) {
					friendContext = new FriendContextImpl();
				}
			}
		}
		return friendContext;
	}

	public static void main(String[] args) {

		BuddyAdapter adapter = new BuddyAdapterImpl();
		// adapter.getRelationContext().write().acceptAllApply(accepter);

	}

}
