#ifndef __BUDDY_RELATION_CACHE_ICE__
#define __BUDDY_RELATION_CACHE_ICE__

#include <BuddyCommon.ice>
#include <ObjectCache.ice>

module xce {
module buddy {

	/**
	 *BuddyRelationData only for BuddyRelationCache.
	 **/

	struct RelationEntry {
		int id;
		BuddyDesc desc;
	};
	sequence<RelationEntry> RelationEntryList;

	class BuddyRelationData {
		RelationEntryList list;
	};

	struct RelationshipEntry {
		Relationship relation;
		BuddyDesc desc;
	};
	sequence<RelationshipEntry> RelationshipEntryList;

	interface BuddyRelationCacheManager {

		BuddyDesc getRelation( Relationship relation);
		RelationshipEntryList getRelationBatch(MyUtil::Int2IntMap ids);

		/** 
		 * BuddyRelationCachePreloader invoke this.
		 **/
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult data);
		/** 
		 * BuddyRelationCacheLoader invoke this. 
		 **/
		void setRelation(Relationship relation, BuddyDesc desc);

		void addBuddyRelationData(int id, BuddyRelationData data);
 	};

	interface BuddyRelationTTManager {

		BuddyDesc getRelation( Relationship relation);

		/** 
		 * BuddyRelationCachePreloader invoke this.
		 **/
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult data);
		/** 
		 * BuddyRelationCacheLoader invoke this. 
		 **/
		void setRelation(Relationship relation, BuddyDesc desc);

		void addBuddyRelationData(int id, BuddyRelationData data);
 	};

	interface BuddyRelationLoader {
		/** 
		 * BuddyRelationCache invoke this. 
		 **/
		void reload( int id );

		/** 
		 * BuddyCoreLogic invoke this. 
		 **/
		void setRelation( Relationship relation, BuddyDesc desc);
 	};
};
};

#endif
