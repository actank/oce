package mop.hi.oce.adapter;

import java.util.List;
import java.util.Vector;

import mop.hi.oce.domain.buddy.BuddyApply;
import mop.hi.oce.domain.buddy.BuddyApplyWithInfo;
import mop.hi.oce.domain.buddy.BuddyBlock;
import mop.hi.oce.domain.buddy.BuddyRelation;
import mop.hi.oce.domain.buddy.CheckedBuddyRelation;

public interface BuddyCoreAdapter {

	/**
	 * 增加一个好友申请。
	 * 如果已经申请或者是被加黑名单，则什么都不做。
	 * 如果曾经是黑名单关系，则删除黑名单，并申请。
	 * 如果曾经是被申请，则接受申请。
	 * @param request
	 */
	void addApply(BuddyApplyWithInfo request, String ref);
	@Deprecated
	void addApply(BuddyApplyWithInfo request);
	

	/**
	 * 增加一批好友申请。
	 * 如果已经申请或者是被加黑名单，则什么都不做。
	 * 如果曾经是黑名单关系，则删除黑名单，并申请。
	 * 如果曾经是被申请，则接受申请。
	 * @param request
	 */
	void addApplyBatch(List<BuddyApplyWithInfo> requests, String ref);
	@Deprecated
	void addApplyBatch(List<BuddyApplyWithInfo> requests);

	/**
	 * 接受好友申请。
	 * 当且仅当曾经是申请关系，才会进行操作。
	 * 其他情况不做操作。
	 * @param request
	 */
	void acceptApply(BuddyApply request);

	/**
	 * 接受所有好友申请。
	 * 
	 * @param accepter
	 */
	void acceptAllApply(int accepter);

	/**
	 * 拒绝好友申请。
	 * 当且仅当曾经是申请关系时，才会进行操作。
	 * 其他情况不操作。
	 * @param request
	 */

	void denyApply(BuddyApply request);

	/**
	 * 拒绝所有好友申请。
	 * 
	 * @param accepter
	 */
	void denyAllApply(int accepter);

	/**
         * deny one applicant's all applies   
         * 
         * @param accepter
         */
	void denyAllApplyByApplicant(int applicant);
	
	// Request getter.
	@Deprecated
	int getAppliedCount(int accepter);

	/**
         * 获取申请过ID序列。
         * @param applicant
         * @param begin
	 * @param limit
         * @return
         */
	List<BuddyRelation> getApplySeq(int applicant, int begin, int limit);
	/**
	 * 删除好友关系。
	 * 如果不是好友，则不操作。
	 * @param relation
	 */
	void removeFriend(BuddyRelation relation);

	// Buddy getter.
	@Deprecated
	int getBuddyCount(int userId);

	/**
	 * 增加黑名单
	 * 无论曾经是什么关系，一律删除改为黑名单。
	 * @param block
	 */
	void addBlock(BuddyBlock block);

	/**
	 * 删除黑名单
	 * 当且仅当曾经是黑名单关系，才进行操作。
	 * @param block
	 */
	void removeBlock(BuddyBlock block);

	// Block getter.
	@Deprecated
	int getBlockCount(int from);

	/**
	 * 获取两者的关系。
	 * @param fromId
	 * @param toId
	 * @return
	 */
	BuddyRelation getRelation(int fromId, int toId);

	/**
         * 获取两者的关系。
         * @param fromId
         * @param toId
         * @return
         */
        BuddyRelation getBuddyCoreRelation(int fromId, int toId);
    	/**
         * 获取两者的关系。
         * @param fromId
         * @param toId
         * @return
         */
   CheckedBuddyRelation getBuddyCoreCheckedRelation(int fromId, int toId);
	@Deprecated
	List<BuddyRelation> getFriendList(int userId, int begin, int limit);

	@Deprecated	
	void addFriend(int fromId, int toId);

	void addFriendDirectly(int fromId, int toId);
}
