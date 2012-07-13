#ifndef __BuddyI_h__
#define __BuddyI_h__

#include "BuddyCommon.h"
#include "Buddy.h"
#include "ServiceI.h"
#include "../../../BuddyEntry/src/BuddyEntry.h"
#include "../../../BuddyEntry/src/BuddyRelation.h"

namespace xce {
namespace buddy {

static const int BUDDY_RELATION = 0;
static const int TASK_LEVEL_CACHEUSERCOUNT = 1;
static const int TASK_LEVEL_BUDDY_FILL = 2;

/**
  * @brief class BuddyManagerI
  *  实现 Buddy 关系管理
  */
class BuddyManagerI: virtual public BuddyManager,virtual public MyUtil::Singleton<BuddyManagerI> {
public:

	/**
          * 异步增加一个好友申请。
          * 如果已经申请或者是被加黑名单，则什么都不做。
          * 如果曾经是黑名单关系，则删除黑名单，并申请。
          * 如果曾经是被申请，则接受申请。
          * @param apply 申请关系 
          */
	virtual void addApply(const Relationship&, const ApplyInfo& info,
			const Ice::Current& = Ice::Current());

	/**
          * 同步增加一个好友申请。
          * 如果已经申请或者是被加黑名单，则什么都不做。
          * 如果曾经是黑名单关系，则删除黑名单，并申请。
          * 如果曾经是被申请，则接受申请。
          * @param apply 申请关系 
          * @return OpState 记录上次与本次操作Buddy关系 
          */
	virtual OpState addApplyWithState(const Relationship&, const ApplyInfo& info,
			const Ice::Current& = Ice::Current());

	/**
          * 异步增加一批好友申请。
          * 如果已经申请或者是被加黑名单，则什么都不做。
          * 如果曾经是黑名单关系，则删除黑名单，并申请。
          * 如果曾经是被申请，则接受申请。
          * @param applies 多个申请关系 
          */
	virtual void addApplyBatch(const ApplyshipMap&, const ApplyInfoMap&,
			const Ice::Current& = Ice::Current());

	/**
          * 同步增加一批好友申请。
          * 如果已经申请或者是被加黑名单，则什么都不做。
          * 如果曾经是黑名单关系，则删除黑名单，并申请。
          * 如果曾经是被申请，则接受申请。
          * @param applies 多个申请关系
          * @return OpStateMap 多个OpState与Buddy的映射 
          */
	virtual OpStateMap addApplyWithStateBatch(const ApplyshipMap&,
			const ApplyInfoMap&, const Ice::Current& = Ice::Current());

	/**
          * 异步接受好友申请。
          * 当且仅当曾经是申请关系，才会进行操作。
          * 其他情况不做操作。
          * @param apply 申请关系 
          */
	virtual void acceptApply(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 同步接受好友申请。
          * 当且仅当曾经是申请关系，才会进行操作。
          * 其他情况不做操作。
          * @param apply 申请关系 
          */
	virtual OpState acceptApplyWithState(const Relationship&, const Ice::Current& =
			Ice::Current());
	
	/**
          * 接受所有好友申请。
          * 
          * @param userId 被操作id 
          */
	virtual void acceptAllApplied(::Ice::Int, const Ice::Current& =
			Ice::Current());

	/**
          * 异步拒绝好友申请。
          * 当且仅当曾经是申请关系时，才会进行操作。
          * 其他情况不操作。
          * @param apply 申请关系
          */
	virtual void denyApply(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 同步拒绝好友申请。
          * 当且仅当曾经是申请关系时，才会进行操作。
          * 其他情况不操作。
          * @param apply 申请关系
          * @return OpState 记录上次与本次操作Buddy关系 
          */
	virtual OpState denyApplyWithState(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 拒绝所有好友申请。
          * 
          * @param userId 被操作Id
          */
	virtual void denyAllApplied(::Ice::Int, const Ice::Current& =
			Ice::Current());

	/**     
          * 获取被申请计数。
          * 
          * @param userId 被操作Id
          * @return int 计数值
          */
	virtual ::Ice::Int getAppliedCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	/**     
          * 获取所有被申请id。
          * 
          * @param userId 被操作Id
          * @param begin 起始id
          * @param limit 操作id个数
          * @return IntSeq 多个被申请id
          */
	virtual ::MyUtil::IntSeq getAppliedSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	/**     
          * 获取主动申请计数。
          * 
          * @param userId 被操作Id
          * @return int 计数值
          */
	virtual ::Ice::Int getApplyCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	/**     
          * 获取所有主动申请id。
          * 
          * @param userId 被操作Id
          * @param begin 起始id
          * @param limit 操作id个数
          * @return IntSeq 多个主动申请id
          */
	virtual ::MyUtil::IntSeq getApplySeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());
	
	/**     
          * 批量获取所有主动申请id。
          * 
          * @param userIds 多个被操作Id
          * @return Int2IntSeqMap 多个id与多个主动申请id映射
          */	
	virtual ::MyUtil::Int2IntSeqMap getApplySeqBatch(const ::MyUtil::IntSeq&,
			const Ice::Current& = Ice::Current());

	/**
          * 异步删除好友关系。
          * 如果不是好友，则不操作。
          * @param buddy 好友关系 
          */
	virtual void removeFriend(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 同步删除好友关系。
          * 如果不是好友，则不操作。
          * @param buddy 好友关系 
          */
	virtual OpState removeFriendWithState(const Relationship&,
			const Ice::Current& = Ice::Current());

	/**
          * 获取好友计数。
          * 
          * @param userId 被操作id 
          */
	virtual ::Ice::Int getFriendCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	/**     
          * 获取所有好友id。
          * 
          * @param userId 被操作Id
          * @param begin 起始id
          * @param limit 操作id个数
          * @return IntSeq 多个好友id
          */
	virtual ::MyUtil::IntSeq getFriendSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());
	/**     
          * 批量获取好友id。
          * 
          * @param userIds 多个被操作Id
          * @return Int2IntSeqMap 多个id与多个好友id映射
          */
	virtual ::MyUtil::Int2IntSeqMap getFriendSeqBatch(const ::MyUtil::IntSeq&,
			const Ice::Current& = Ice::Current());
	
	/**
          * 异步增加黑名单
          * 无论曾经是什么关系，一律删除改为黑名单。
          * @param block 加黑关系
          */
	virtual void addBlock(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 同步增加黑名单
          * 无论曾经是什么关系，一律删除改为黑名单。
          * @param block 加黑关系
          * @return OpState 记录上次与本次操作Buddy关系
          */
	virtual OpState addBlockWithState(const Relationship&, const Ice::Current& =
			Ice::Current());
	
	/**
          * 异步删除黑名单
          * 当且仅当曾经是黑名单关系，才进行操作。
          * @param block 加黑关系
          */
	virtual void removeBlock(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 同步删除黑名单
          * 当且仅当曾经是黑名单关系，才进行操作。
          * @param block 加黑关系
          * @return OpState 记录上次与本次操作Buddy关系
          */
	virtual OpState removeBlockWithState(const Relationship&, const Ice::Current& =
			Ice::Current());

	/**
          * 获取黑名单计数。
          * 
          * @param userId 被操作id 
          */
	virtual ::Ice::Int getBlockCount(::Ice::Int, const Ice::Current& =
			Ice::Current());
	
	/**     
          * 获取所有加黑id。
          * 
          * @param userId 被操作Id
          * @param begin 起始id
          * @param limit 操作id个数
          * @return IntSeq 多个加黑id
          */
	virtual ::MyUtil::IntSeq getBlockSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	/**     
          * 批量获取加黑id。
          * 
          * @param userIds 多个被操作Id
          * @return Int2IntSeqMap 多个id与多个加黑id映射
          */	
	virtual ::MyUtil::Int2IntSeqMap getBlockSeqBatch(const ::MyUtil::IntSeq&,
			const Ice::Current& = Ice::Current());

	//virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());

	///以下4个接口为 cacheloader所需
	virtual bool isValid(const Ice::Current& = Ice::Current());

        virtual void setValid(bool valid,const Ice::Current& = Ice::Current());

        //批量
	virtual void setData(const ::MyUtil::ObjectResultPtr& data, const Ice::Current& = 
			Ice::Current());
	//单个
	virtual void load( ::Ice::Int, const ::Ice::ObjectPtr&, const Ice::Current& =
			Ice::Current());
	//!以下4个接口为 cacheloader所需

	//后门
	/*virtual void addFriend(const Relationship&, const Ice::Current& =
                        Ice::Current());
*/
	/**     
          * 添加申请，完成对应ID内存修改
          * 
          * @param Relationship 两者关系
	  * @param StateRecorder  状态记录
          * @return BuddyRelationHoldersPtr 返回一个BuddyRelationHolders，以便于对操作状态作判断
          */
        virtual BuddyRelationHoldersPtr cacheAddApply(const Relationship&,  const StateRecorder& , const Ice::Current& =
                        Ice::Current());
	
	/**     
          * 接收申请，完成对应ID内存修改
          * 
          * @param Relationship 两者关系
          * @param StateRecorder  状态记录
          * @return BuddyRelationHoldersPtr 返回一个BuddyRelationHolders，以便于对操作状态作判断
          */
        virtual BuddyRelationHoldersPtr cacheAcceptApply(const Relationship&,  const StateRecorder& , const Ice::Current& =
                        Ice::Current());

	/**     
          * 拒绝申请，完成对应ID内存修改
          * 
          * @param Relationship 两者关系
          * @param StateRecorder  状态记录
          * @return BuddyRelationHoldersPtr 返回一个BuddyRelationHolders，以便于对操作状态作判断
          */
        virtual BuddyRelationHoldersPtr cacheDenyApply(const Relationship&,  const StateRecorder& , const Ice::Current& =
                        Ice::Current());

	/**     
          * 移除好友，完成对应ID内存修改
          * 
          * @param Relationship 两者关系
          * @param StateRecorder  状态记录
          * @return BuddyRelationHoldersPtr 返回一个BuddyRelationHolders，以便于对操作状态作判断
          */
        virtual BuddyRelationHoldersPtr cacheRemoveFriend(const Relationship&, const StateRecorder& , const Ice::Current& =
                        Ice::Current());

	/**     
          * 加黑，完成对应ID内存修改
          * 
          * @param Relationship 两者关系
          * @param StateRecorder  状态记录
          * @return BuddyRelationHoldersPtr 返回一个BuddyRelationHolders，以便于对操作状态作判断
          */
        virtual BuddyRelationHoldersPtr cacheAddBlock(const Relationship&, const StateRecorder& , const Ice::Current& =
                        Ice::Current());

	/**     
          * 移除黑名单，完成对应ID内存修改
          * 
          * @param Relationship 两者关系
          * @param StateRecorder  状态记录
          * @return BuddyRelationHoldersPtr 返回一个BuddyRelationHolders，以便于对操作状态作判断
          */
        virtual BuddyRelationHoldersPtr cacheRemoveBlock(const Relationship&, const StateRecorder& , const Ice::Current& =
                        Ice::Current());

	/**     
          * 移除该ID所有关系，完成对应ID内存修改
          * 
          * @param Ice::Int 被操作ID 
          */
	virtual void removeBuddyRelation(const Ice::Int ,const Ice::Current& current = Ice::Current());
	
	/**     
          * 根据状态作相关处理
          * 1. 自身判断
	  * 2. 获取被操作Id对应关系BuddyRelationHolder
	  * 3. 对应个别状态，特殊处理
	  * 4. 获取两个ID关系
	  * 5. 对关系作判零处理
	  * 6. 对关系作溢出处理
	  * 7. 进入processHandleWithState， 不同状态不同处理 
          * @param state 状态
	  * @param relation 被操作关系
	  * @param applyInfo 申请信息
	  * @return OpState
          */
	OpState processHandler(const BuddyState& state, const Relationship& relation,
                const ApplyInfo* applyInfo = NULL, const Ice::Current& =  Ice::Current());

	/**     
          * 获取ID所有关系BuddyRelation
          * 
          * @param Ice::Int 被操作ID 
	  * @return BuddyRelationPtr 
          */
	BuddyRelationPtr getBuddyRelation(const ::Ice::Int, const Ice::Current& =
                        Ice::Current());
	
	/**     
          * 获取批量ID所有关系Int2BuddyRelationMap
          * 
          * @param Ice::IntSeq 被操作批量ID 
          * @return Int2BuddyRelationMap
          */
	Int2BuddyRelationMap getBuddyRelationBatch(const ::MyUtil::IntSeq&, const Ice::Current& =
                        Ice::Current());

	//兼容接口，已移至BuddyRelationCache
	virtual BuddyDesc getRelation(const Relationship&, const Ice::Current& =
                        Ice::Current());

private:
	friend class MyUtil::Singleton<BuddyManagerI>;	
	BuddyManagerI(){
	}	
	
	OpState itemEmptyCheck(const BuddyState& state, const Relationship& relation, OpState& result,
                        const ApplyInfo* applyInfo = NULL,  const Ice::Current& current = Ice::Current() );
        
	OpState itemOverflowCheck(const BuddyState& state ,  const Relationship& relation , 
			OpState& result, const Ice::Current& current = Ice::Current());
        
	OpState addApplyHandle( const Relationship& apply,const ApplyInfo& applyInfo,
                        OpState& result, const Ice::Current& current = Ice::Current());
        
	OpState acceptApplyHandle( const Relationship& apply,OpState& result,
                        const Ice::Current& current = Ice::Current());
        
	OpState denyApplyHandle( const Relationship& apply,OpState& result,
                        const Ice::Current& current = Ice::Current());
        
	OpState removeFriendHandle( const Relationship& buddy, OpState& result,
                        const Ice::Current& current = Ice::Current());
        
	OpState addBlockHandle( const Relationship& block, OpState& result,
                        const Ice::Current& current = Ice::Current());
        
	OpState removeBlockHandle( const Relationship& block, OpState& result,
                        const Ice::Current& current = Ice::Current());

        OpState processHandleWithState(const BuddyState& state, const Relationship& relation, OpState& result,
                	const ApplyInfo* applyInfo = NULL, const Ice::Current& current = Ice::Current());
       
	void specialHandle(const BuddyState& state, const Relationship& relation, 
                        const Ice::Current& current = Ice::Current());

	void assignStateRecorder(BuddyRelationHoldersIPtr&,const Relationship&,
                        const BuddyState, const bool, /*StateRecorder&,*/
                        const StateRecorder&, const Ice::Current& current = Ice::Current());

	bool isLocalCache(const Ice::Current& current);
        
	bool isBiblock(const Ice::Current& current);
	
	bool isMainCache(const Ice::Current& current);
	
	BuddyRelationHoldersIPtr locateBuddyRelation(uint32_t from);
 	
	BuddyRelationHoldersIPtr findBuddyRelation(uint32_t from);
       
	void cacheUserCountGuestRequest(uint32_t userId);
        
	string translateBuddyState(const BuddyState& state) const;
	
	BuddyDesc translate(uint32_t desc) const;

private:	
	bool _isValid;
        
	IceUtil::RWRecMutex _validMutex;
};

class BuddyDatabaseHelper: public MyUtil::Singleton<BuddyDatabaseHelper> {
public:
	bool insertBuddyApply(int applicant, int accepter,
			const MyUtil::Str2StrMap& props);
	
	bool deleteBuddyApply(int applicant, int accepter);
	
	bool insertRelationBi(int from, int to);
	
	bool deleteRelationBi(int from, int to);
	
	bool insertBlock(int from, int to);
	
	bool deleteBlock(int from, int to);
private:
	friend class MyUtil::Singleton<BuddyDatabaseHelper>;
	string getRelationTable(int from);
};

class AddApplyTask: virtual public MyUtil::Task {
public:
	AddApplyTask(const Relationship& apply,
			const ApplyInfo& info,
			const Ice::Current& current) :
		 _apply(apply), _info(info), _current(current) {
	}
	
	virtual void handle();
private:
	Relationship _apply;
	ApplyInfo _info;
	Ice::Current _current;
};

class AcceptApplyTask: virtual public MyUtil::Task {
public:
	AcceptApplyTask(const Relationship& apply,
			const Ice::Current& current) :
		 _apply(apply), _current(current) {
	}
	
	virtual void handle();
private:
	Relationship _apply;
	Ice::Current _current;
};

class DenyApplyTask: virtual public MyUtil::Task {
public:
	DenyApplyTask(const Relationship& apply,
			const Ice::Current& current) :
		 _apply(apply), _current(current) {
	}
	
	virtual void handle();
private:
	Relationship _apply;
	Ice::Current _current;
};

class RemoveFriendTask: virtual public MyUtil::Task {
public:
	RemoveFriendTask(const Relationship& buddy,
			const Ice::Current& current) :
		_buddy(buddy), _current(current) {
	}
	
	virtual void handle();
private:
	Relationship _buddy;
	Ice::Current _current;
};

class AddBlockTask: virtual public MyUtil::Task {
public:
	AddBlockTask(const Relationship& block,
			const Ice::Current& current) :
		 _block(block), _current(current) {
	}
	
	virtual void handle();
private:
	Relationship _block;
	Ice::Current _current;
};

class RemoveBlockTask: virtual public MyUtil::Task {
public:
	RemoveBlockTask(const Relationship& block,
			const Ice::Current& current) :
		 _block(block), _current(current) {
	}
	
	virtual void handle();
private:
	Relationship _block;
	Ice::Current _current;
};

class AddFriendTask: virtual public MyUtil::Task {
public:
        AddFriendTask(const Relationship& relation,
                        const Ice::Current& current) :
                 _relation(relation), _current(current) {
        }

        virtual void handle();
private:
        Relationship _relation;
        Ice::Current _current;
};

class CacheUserCountGuestRequestTask: virtual public MyUtil::Task {
public:
	CacheUserCountGuestRequestTask(uint32_t& userId, uint32_t& count) :
		Task(TASK_LEVEL_CACHEUSERCOUNT), _userId(userId), _count(count) {
	}
	
	virtual void handle();
private:
	uint32_t _userId;
	uint32_t _count;
};

class FillTask: virtual public MyUtil::Task {
public:
        FillTask(const MyUtil::ObjectResultPtr& data,::Ice::Int level=TASK_LEVEL_BUDDY_FILL):
                MyUtil::Task(level), _buddyData(data) {
        }
        
	virtual void handle();
private:
        MyUtil::ObjectResultPtr _buddyData;
};

}
}

#endif
