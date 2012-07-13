#ifndef _BUDDYCORE_ICE_
#define _BUDDYCORE_ICE_

#include <Util.ice>

module com {
module xiaonei {
module service {
module buddy {
    enum BuddyDesc 
    {
		NoPath,
		Self,
		Friend,
		Apply,
		Applied,
		Block,
		Blocked,
		BiBlock //BilateralBlock
    };
    
    struct OpState
    {
		BuddyDesc first;
		BuddyDesc second;
    };

    struct ApplyInfo
    {
		MyUtil::Str2StrMap props;
    };
    struct Applyship
    {
		int applicant;
		int accepter;
    };
    struct Blockship
    {
		int from;
		int to;
    };
    struct Friendship
    {
		int from;
		int to;
    };
    struct Relationship
    {
		int from;
		int to;
    };
    
    dictionary<int, Applyship> ApplyshipMap;
    dictionary<int, ApplyInfo> ApplyInfoMap;
    dictionary<int, OpState> OpStateMap;

    interface BuddyCoreManager // Every void returned interface in BuddyManager should have a syncXXX interface in SyncManager
    {
		// Apply
		void addApply(Applyship apply, ApplyInfo info);
		OpState addApplyWithState(Applyship apply, ApplyInfo info);
		void addApplyBatch(ApplyshipMap applies, ApplyInfoMap infos);
		OpStateMap addApplyWithStateBatch(ApplyshipMap applies, ApplyInfoMap infos);
	
		void acceptApply(Applyship apply);
		OpState acceptApplyWithState(Applyship apply);
		void acceptAllApplied(int userId);
	
		void denyApply(Applyship apply);
		OpState denyApplyWithState(Applyship apply);
		void denyAllApplied(int accepter);
		void denyAllApply(int applicant);
	
		int getAppliedCount(int userId);
		MyUtil::IntSeq getAppliedSeq(int userId, int begin, int limit);
	
		int getApplyCount(int userId);
		MyUtil::IntSeq getApplySeq(int userId, int begin, int limit);
		MyUtil::Int2IntSeqMap getApplySeqBatch(MyUtil::IntSeq userIds);
	
		// Buddy
		void removeFriend(Friendship buddy);
		OpState removeFriendWithState(Friendship buddy);
	
		int getFriendCount(int userId);
		MyUtil::IntSeq getFriendSeq(int userId, int begin, int limit);
		MyUtil::Int2IntSeqMap getFriendSeqBatch(MyUtil::IntSeq userIds);
	
		// Block
		void addBlock(Blockship block);
		OpState addBlockWithState(Blockship block);
	
		void removeBlock(Blockship block);
		OpState removeBlockWithState(Blockship block);
	
		int getBlockCount(int userId);
		MyUtil::IntSeq getBlockSeq(int userId, int begin, int limit);
		MyUtil::Int2IntSeqMap getBlockSeqBatch(MyUtil::IntSeq userIds);

		//AddFriend
                void addFriend(int from, int to);
                OpState addFriendWithState(int from, int to);		
	
		// Relation
		BuddyDesc getRelation(Relationship relation);
	
		// reload
		void reload(int id);
    };

    interface BuddyCoreSyncManager // All interfaces in SyncManager must returns void.
    {
		void reload(int id);
		void syncAddFriend(int from, int to); // This is a backdoor.
	
		void syncAddApply(Applyship apply);
		void syncAcceptApply(Applyship apply);
		void syncDenyApply(Applyship apply); 
		// syncDenyAllApply use syncDenyApply with BatchedOneWay 
	
		void syncRemoveFriend(Friendship buddy);
	
		void syncAddBlock(Blockship block);
		void syncRemoveBlock(Blockship block);
    };
    
    // Snapshot
    enum SnapshotStatus {
    	INITIALIZING, INITIALIZED
    };
    
    struct SnapshotEntry{
    	int toId;
    	BuddyDesc desc;
    };
    sequence<SnapshotEntry> SnapshotEntrySeq;
    dictionary<int, SnapshotEntrySeq> SnapshotEntrySeqMap;
    
    interface BuddyCoreSnapshot{
    	SnapshotStatus getStatus();
    	void setStatus(SnapshotStatus newStatus);
    	void patch(int timestamp, SnapshotEntrySeqMap data);
    	SnapshotEntrySeqMap getEntry4CommonFriend(int userId);
    	SnapshotEntrySeqMap getEntry4ShareFriend(MyUtil::IntSeq ids);
    	SnapshotEntrySeqMap getFriends(MyUtil::IntSeq ids);
    };
    sequence<BuddyCoreSnapshot*> BuddyCoreSnapshotPrxSeq;

	// Logic
    interface BuddyCoreLogic
    {
		void addApply(Applyship apply);
		void acceptApply(Applyship apply);
		void denyApply(Applyship apply);
		void removeFriend(Friendship buddy);
		
		void addBlock(Blockship block);
		void removeBlock(Blockship block);
		void addFriend(int from, int to);	
    		void reload(int userId);
    };

};
};
};
};
#endif

