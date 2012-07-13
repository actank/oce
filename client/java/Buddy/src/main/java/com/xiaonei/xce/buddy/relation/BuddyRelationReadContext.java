package com.xiaonei.xce.buddy.relation;

import mop.hi.oce.domain.buddy.BuddyRelation;

/**
 * BuddyRelation系列读接口
 * 
 * @author
 * 
 */
public interface BuddyRelationReadContext {


	/**
	 * 获取两者的关系。
	 * 
	 * @param fromId
	 * @param toId
	 * @return
	 */
	BuddyRelation getRelation(int fromId, int toId);
}
