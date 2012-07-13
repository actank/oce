package com.xiaonei.xce.buddy.relation.impl;

import mop.hi.oce.domain.buddy.BuddyRelation;

import com.xiaonei.xce.buddyadapter.buddyfriends.BuddyFriendsCacheAdapter;
import com.xiaonei.xce.buddyrelationcache.BuddyRelationCacheAdapter;
import com.xiaonei.xce.buddy.relation.BuddyRelationReadContext;

public class BuddyRelationReadContextImpl implements BuddyRelationReadContext {

	@Override
	public BuddyRelation getRelation(int fromId, int toId) {
		// TODO Auto-generated method stub
		BuddyRelation relation = BuddyRelationCacheAdapter.getInstance().getRelation(fromId, toId);
//		if(relation.isNoPath()){
//			Boolean ret = BuddyFriendsCacheAdapter.getInstance().isFriendsFriend(fromId, toId);
//			if(ret){
//				return new BuddyRelation(fromId,toId,BuddyRelation.BuddyDesc.FriendFriend);
//			}
//		}
		return relation;	
	}
}
