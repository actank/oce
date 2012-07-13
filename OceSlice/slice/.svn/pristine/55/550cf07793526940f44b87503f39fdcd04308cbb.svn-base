#ifndef _BUDDY_ICE_
#define _BUDDY_ICE_

#include <Util.ice>
#include <BuddyCommon.ice>
#include <ObjectCache.ice>

module xce {
module buddy {

    /**
      *       负责Buddy所有关系逻辑处理
      **/
    interface BuddyManager // Every void returned interface in BuddyManager should have a syncXXX interface in SyncManager
    {
		// Apply
		/**
         	  * 异步增加一个好友申请。
         	  * 如果已经申请或者是被加黑名单，则什么都不做。
         	  * 如果曾经是黑名单关系，则删除黑名单，并申请。
         	  * 如果曾经是被申请，则接受申请。
         	  * @param apply 申请关系 
          	  **/
		void addApply(Relationship apply, ApplyInfo info);
		/**
                  * 同步增加一个好友申请。
                  * 如果已经申请或者是被加黑名单，则什么都不做。
                  * 如果曾经是黑名单关系，则删除黑名单，并申请。
                  * 如果曾经是被申请，则接受申请。
                  * @param apply 申请关系 
		  * @return OpState 记录上次与本次操作Buddy关系 
                  **/
		OpState addApplyWithState(Relationship apply, ApplyInfo info);
		/**
         	  * 异步增加一批好友申请。
         	  * 如果已经申请或者是被加黑名单，则什么都不做。
         	  * 如果曾经是黑名单关系，则删除黑名单，并申请。
         	  * 如果曾经是被申请，则接受申请。
         	  * @param applies 多个申请关系 
          	  **/
		void addApplyBatch(ApplyshipMap applies, ApplyInfoMap infos);
		/**
                  * 同步增加一批好友申请。
                  * 如果已经申请或者是被加黑名单，则什么都不做。
                  * 如果曾经是黑名单关系，则删除黑名单，并申请。
                  * 如果曾经是被申请，则接受申请。
                  * @param applies 多个申请关系
		  * @return OpStateMap 多个OpState与Buddy的映射 
                  **/
		OpStateMap addApplyWithStateBatch(ApplyshipMap applies, ApplyInfoMap infos);

		/**
         	  * 异步接受好友申请。
         	  * 当且仅当曾经是申请关系，才会进行操作。
         	  * 其他情况不做操作。
         	  * @param apply 申请关系 
         	  **/	
		void acceptApply(Relationship apply);
		/**
                  * 同步接受好友申请。
                  * 当且仅当曾经是申请关系，才会进行操作。
                  * 其他情况不做操作。
                  * @param apply 申请关系 
                  **/
		OpState acceptApplyWithState(Relationship apply);
		/**
         	  * 接受所有好友申请。
         	  * 
         	  * @param userId 被操作id 
            	  **/
		void acceptAllApplied(int userId);

		/**
         	  * 异步拒绝好友申请。
         	  * 当且仅当曾经是申请关系时，才会进行操作。
         	  * 其他情况不操作。
         	  * @param apply 申请关系
         	  **/	
		void denyApply(Relationship apply);
		/**
                  * 同步拒绝好友申请。
                  * 当且仅当曾经是申请关系时，才会进行操作。
                  * 其他情况不操作。
                  * @param apply 申请关系
		  * @return OpState 记录上次与本次操作Buddy关系 
                  **/
		OpState denyApplyWithState(Relationship apply);

		/**
	          * 拒绝所有好友申请。
        	  * 
         	  * @param userId 被操作Id
         	  **/
		void denyAllApplied(int userId);
		/**     
                  * 获取被申请计数。
                  * 
                  * @param userId 被操作Id
		  * @return int 计数值
                  **/	
		int getAppliedCount(int userId);
		/**     
                  * 获取所有被申请id。
                  * 
                  * @param userId 被操作Id
		  * @param begin 起始id
		  * @param limit 操作id个数
		  * @return IntSeq 多个被申请id
                  **/
		MyUtil::IntSeq getAppliedSeq(int userId, int begin, int limit);

		/**     
                  * 获取主动申请计数。
                  * 
                  * @param userId 被操作Id
		  * @return int 计数值
                  **/	
		int getApplyCount(int userId);
		/**     
                  * 获取所有主动申请id。
                  * 
                  * @param userId 被操作Id
                  * @param begin 起始id
                  * @param limit 操作id个数
		  * @return IntSeq 多个主动申请id
                  **/	
		MyUtil::IntSeq getApplySeq(int userId, int begin, int limit);
		/**     
                  * 批量获取所有主动申请id。
                  * 
                  * @param userIds 多个被操作Id
                  * @return Int2IntSeqMap 多个id与多个主动申请id映射
                  **/
		MyUtil::Int2IntSeqMap getApplySeqBatch(MyUtil::IntSeq userIds);
	
		// Buddy
		/**
         	  * 异步删除好友关系。
         	  * 如果不是好友，则不操作。
         	  * @param buddy 好友关系 
         	  **/
		void removeFriend(Relationship buddy);
		/**
                  * 同步删除好友关系。
                  * 如果不是好友，则不操作。
                  * @param buddy 好友关系 
                  **/
		OpState removeFriendWithState(Relationship buddy);
		/**
                  * 获取好友计数。
                  * 
                  * @param userId 被操作id 
                  **/	
		int getFriendCount(int userId);
		/**     
                  * 获取所有好友id。
                  * 
                  * @param userId 被操作Id
                  * @param begin 起始id
                  * @param limit 操作id个数
                  * @return IntSeq 多个好友id
                  **/
		MyUtil::IntSeq getFriendSeq(int userId, int begin, int limit);
		/**     
                  * 批量获取好友id。
                  * 
                  * @param userIds 多个被操作Id
                  * @return Int2IntSeqMap 多个id与多个好友id映射
                  **/
		MyUtil::Int2IntSeqMap getFriendSeqBatch(MyUtil::IntSeq userIds);
	
		// Block
		/**
         	  * 异步增加黑名单
         	  * 无论曾经是什么关系，一律删除改为黑名单。
         	  * @param block 加黑关系
         	  **/
		void addBlock(Relationship block);
		/**
                  * 同步增加黑名单
                  * 无论曾经是什么关系，一律删除改为黑名单。
                  * @param block 加黑关系
		  * @return OpState 记录上次与本次操作Buddy关系
                  **/
		OpState addBlockWithState(Relationship block);

		/**
         	  * 异步删除黑名单
         	  * 当且仅当曾经是黑名单关系，才进行操作。
         	  * @param block 加黑关系
         	  **/	
		void removeBlock(Relationship block);
		/**
                  * 同步删除黑名单
                  * 当且仅当曾经是黑名单关系，才进行操作。
		  * @param block 加黑关系
                  * @return OpState 记录上次与本次操作Buddy关系
                  **/
		OpState removeBlockWithState(Relationship block);
		/**
                  * 获取黑名单计数。
                  * 
                  * @param userId 被操作id 
                  **/	
		int getBlockCount(int userId);
		/**     
                  * 获取所有加黑id。
                  * 
                  * @param userId 被操作Id
                  * @param begin 起始id
                  * @param limit 操作id个数
                  * @return IntSeq 多个加黑id
                  **/
		MyUtil::IntSeq getBlockSeq(int userId, int begin, int limit);
		/**     
                  * 批量获取加黑id。
                  * 
                  * @param userIds 多个被操作Id
                  * @return Int2IntSeqMap 多个id与多个加黑id映射
                  **/
		MyUtil::Int2IntSeqMap getBlockSeqBatch(MyUtil::IntSeq userIds);

		/**     
                  * 获取单个id所有关系列表。
                  * 
                  * @param userId 被操作Id
                  * @return BuddyRelation 单个id所有关系列表
                  **/
		BuddyRelation getBuddyRelation(int userId);
		/**     
                  * 批量获取id所有关系列表。
                  * 
                  * @param userIds 多个被操作Id
                  * @return Int2BuddyRelationMap 多个id与其相应所有关系列表映射
                  **/
		Int2BuddyRelationMap getBuddyRelationBatch(::MyUtil::IntSeq userIds);
		/**     
                  * 删除单个id所有关系列表。
                  * 
                  * @param userIds 多个被操作Id
                  **/
		void removeBuddyRelation(int userId);	
		// reload
		//void reload(int id);
		
		//void addFriend(Relationship relation); // This is a backdoor.

		/**     
                  * 对cache进行添加申请修改。
                  * 
                  * @param apply 申请关系
		  * @param sr 操作状态记录
		  * @return BuddyRelationHolders 单个Id所有相关内存映射 
                  **/
                BuddyRelationHolders cacheAddApply(Relationship apply,  StateRecorder sr);
		/**     
                  * 对cache进行接受申请修改。
                  * 
                  * @param apply 申请关系
                  * @param sr 操作状态记录
                  * @return BuddyRelationHolders 单个Id所有相关内存映射 
                  **/
                BuddyRelationHolders cacheAcceptApply(Relationship apply, StateRecorder sr);
                
		/**     
                  * 对cache进行拒绝申请修改。
                  * 
                  * @param apply 申请关系
                  * @param sr 操作状态记录
                  * @return BuddyRelationHolders 单个Id所有相关内存映射 
                  **/
		BuddyRelationHolders cacheDenyApply(Relationship apply, StateRecorder sr);
                // syncDenyAllApply use syncDenyApply with BatchedOneWay 
		/**     
                  * 对cache进行删除好友修改。
                  * 
                  * @param apply 申请关系
                  * @param sr 操作状态记录
                  * @return BuddyRelationHolders 单个Id所有相关内存映射 
                  **/
                BuddyRelationHolders cacheRemoveFriend(Relationship buddy, StateRecorder sr);
		/**     
                  * 对cache进行加黑修改。
                  * 
                  * @param apply 申请关系
                  * @param sr 操作状态记录
                  * @return BuddyRelationHolders 单个Id所有相关内存映射 
                  **/
                BuddyRelationHolders cacheAddBlock(Relationship block, StateRecorder sr);
                /**     
                  * 对cache进行删除黑名单修改。
                  * 
                  * @param apply 申请关系
                  * @param sr 操作状态记录
                  * @return BuddyRelationHolders 单个Id所有相关内存映射 
                  **/
		BuddyRelationHolders cacheRemoveBlock(Relationship block, StateRecorder sr);

    	  	/**     
                  * 获取当前服务是否可用。
                  * 
                  * @return bool 服务是否当前可用
                  **/
		bool isValid();
		/**     
                  * 设置当前服务是否可用。
                  * 
                  * @param valid 服务是否当前可用
                  **/
                void setValid(bool valid);
		/**     
                  * 批量设置多个id的内存对象
                  * 
                  * @param datas 多个id与内存对象映射
                  **/
                void setData(MyUtil::ObjectResult datas);
		/**     
                  * 利用Object对userId关系进行重新构造
                  * 
		  * @param userId 被操作id
		  * @param o 对应id的内存对象
                  **/
		void load(int userId,Object o);

		// Relation
		/**     
                  * 获取好友关系
                  * 兼容使用
		  * TODO: 由BuddyRelationCache提供该接口服务
                  * @param relation 要查询的两者id 
                  * @return BuddyDesc 两者关系描述
                  **/
                BuddyDesc getRelation(Relationship relation);
    };

    
    // Logic
    /**
      *       负责Buddy关系变更通知
      **/
    interface BuddyLogic
    {
		/**     
                  * 添加申请通知
                  * 
                  * @param apply 申请关系
                  * @param localbrp 对应申请者BuddyRelation对象
                  * @param remotebrp 对应被申请者BuddyRelation对象
                  **/
                void addApply(Relationship apply, BuddyRelation localbrp, BuddyRelation remotebrp, ApplyInfo info);
		/**     
                  * 接受申请通知
                  * 
                  * @param apply 申请关系
                  * @param localbrp 对应被申请者BuddyRelation对象
                  * @param remotebrp 对应申请者BuddyRelation对象
                  **/
                void acceptApply(Relationship apply, BuddyRelation localbrp, BuddyRelation remotebrp);
                
		/**     
                  * 拒绝申请通知
                  * 
                  * @param apply 申请关系
                  * @param localbrp 对应被申请者BuddyRelation对象
                  * @param remotebrp 对应申请者BuddyRelation对象
                  **/
		void denyApply(Relationship apply, BuddyRelation localbrp, BuddyRelation remotebrp);
                
		/**     
                  * 删除好友通知
                  * 
                  * @param buddy 好友关系
                  * @param localbrp 对应本地BuddyRelation对象
                  * @param remotebrp 对应另一个人的BuddyRelation对象
                  **/
		void removeFriend(Relationship buddy, BuddyRelation localbrp, BuddyRelation remotebrp);

		/**     
                  * 加黑通知
                  * 
                  * @param block 加黑关系
                  * @param localbrp 对应加黑者BuddyRelation对象
                  * @param remotebrp 对应被加黑者BuddyRelation对象
                  **/
                void addBlock(Relationship block, BuddyRelation localbrp, BuddyRelation remotebrp);
                /**     
                  * 去黑通知
                  *  
                  * @param block 加黑关系
                  * @param localbrp 对应加黑者BuddyRelation对象
                  * @param remotebrp 对应被加黑者BuddyRelation对象
                  **/
		void removeBlock(Relationship block, BuddyRelation localbrp, BuddyRelation remotebrp);

    }; 
    /**
      *       负责Buddy数据生成，通知各Cache加载
      **/		
    interface BuddyLoader
    {
		/**     
                  * 从数据库产生数据，并通知相关cache加载
                  *  
                  * @param userId 被操作Id
                  **/
		void load(int userId);
    };
};
};
#endif

