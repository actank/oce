package com.xiaonei.xce.buddy.relation;

import com.xiaonei.xce.buddy.BuddyContext;

/**
 * BuddyRelation系列的接口 <br/>
 * 接口分为读接口和写接口
 * 
 * @author
 * 
 */
public interface BuddyRelationContext extends BuddyContext {

	/**
	 * 返回读接口
	 * 
	 * @return
	 */
	BuddyRelationReadContext read();

	/**
	 * 返回写接口
	 * 
	 * @return
	 */
	BuddyRelationWriteContext write();

}
