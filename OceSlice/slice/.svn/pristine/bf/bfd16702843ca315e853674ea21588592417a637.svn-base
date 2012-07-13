#ifndef _RELATION_ICE_
#define _RELATION_ICE_

#include <Util.ice>
#include <DataWrapper.ice>
module mop
{
module hi
{
module svc
{
module relation
{
	exception RelationException extends MyUtil::MceException
	{
		int hostId;
		int guestId;
	};
	exception RelationGroupException extends MyUtil::MceException
	{
		int hostId;
		int groupId;
	};

	exception RelationNotExistException extends RelationException {};
	exception RelationAlreadyExistException extends RelationException {};

	exception RelationGroupNotExistException extends RelationGroupException {};
	exception RelationGroupAlreadyExistException extends RelationGroupException {};

	dictionary<int, MyUtil::Str2StrMap> Int2Str2StrMap;

	const string CRHOST = "HOST";
	const string CRGUEST = "GUEST";
	const string CRGROUPID = "GROUPID";
	const string CRTIME = "TIME";
	
	class BuddyData
	{
		int ownerId;
		int buddyId;
		int groupId;
		int timestamp;
	};
	sequence<BuddyData> BuddyDataSeq;
	
	const string CBHOST = "HOST";
	const string CBGUEST = "GUEST";
	const string CBTIME = "TIME";
	
	class BlockData
	{	
		int blockerId;
		int blockeeId;
		int timestamp;
	};
	sequence<BlockData> BlockDataSeq;
	
	class BlockedData
	{
		int blockeeId;
		int blockerId;
		int timestamp;
	};
	sequence<BlockedData> BlockedDataSeq;
	
	const string CRQAPPLICANT = "APPLICANT";
	const string CRQACCEPTER = "ACCEPTER";
	const string CRQTIME = "TIME";
	const string CRQAPPLICANTNAME = "APPLICANT_NAME";
	const string CRQENROLLMENTYEAR = "ENROLLMENT_YEAR";
	const string CRQSCHOOLID = "SCHOOLID";
	const string CRQDEPARTMENT = "DEPARTMENT";
	const string CRQDORM = "DORM";
	const string CRQHEADURL = "HEAD_URL";
	const string CRQWHY = "WHY";
	const string CRQSCHOOLNAME = "SCHOOLNAME";
	const string CRQSELECTED = "SELECTED";
	const string CRQSTAR = "STAR";

	class RequestData
	{
		int applicant;
		int accepter;
		int timestamp;
		string applicantName;
		int enrollmentYear;
		string schoolId;
		string department;
		string dorm;
		string headUrl;
		string why;
		string schoolName;
		int selected;
		int star;
	};
	sequence<RequestData> RequestDataSeq;
	
	const string CGHOST = "HOST";
	const string CGGROUP = "GROUPID";
	const string CGNAME = "NAME";
	
	class GroupData
	{
		int ownerId;
		int groupId;
		string name;
	};
	sequence<GroupData> GroupDataSeq;
	
    interface RelationManager
	{
		// Request
        void addRequest(int fromId, int toId, MyUtil::Str2StrMap props);
        void sendRequest(int toId, int fromId, MyUtil::Str2StrMap props);
        
        void confirmRequest(int fromId, int toId);
        void acceptRequest(int toId, int fromId);
        
        void rejectRequest(int fromId,int toId);
        void denyRequest(int toId, int fromId);
        
        void rejectAllRequest(int toId);
		void denyAllRequest(int toId);
		
        int getRequestCount(int toId);
        MyUtil::Str2StrMapSeq getRequestList(int toId, int offset, int limit);
        MyUtil::RowSet getRequestRowSet(int toId,int offset, int limit);
        MyUtil::Str2StrMap getRequest(int fromId, int toId) throws RelationException;
		
		RequestDataSeq getRequestSeq(int toId, int offset, int limit);
		
		// Block
		void addBlock(int sourceId, int targetId);
		void block(int sourceId, int targetId);
		
		void removeBlock(int sourceId, int targetId);
		void unblock(int sourceId, int targetId);
		
		int getBlockCount(int sourceId);
		MyUtil::Str2StrMap getBlock(int sourceId, int targetId) throws RelationException;
		MyUtil::Str2StrMapSeq getBlockList(int sourceId, int begin, int limit);
        MyUtil::RowSet getBlockRowSet(int sourceId, int begin, int limit);
		BlockDataSeq getBlockSeq(int sourceId, int begin, int limit);
		
		int getBlockedCount(int targetId);
	    MyUtil::Str2StrMapSeq getBlockedList(int targetId, int begin, int limit);
        MyUtil::RowSet getBlockedRowSet(int targetId, int begin, int limit);

		// Buddy
		void addBuddy(int hostId, int buddyId);
		void removeBuddy(int hostId, int buddyId);

		int getBuddyCount(int hostId);
		MyUtil::Str2StrMapSeq getBuddyList(int hostId, int begin, int limit);
        MyUtil::RowSet getBuddyRowSet(int hostId, int begin, int limit);
		MyUtil::Str2StrMap getBuddy(int hostId, int buddyId) throws RelationException;
		BuddyDataSeq getBuddySeq(int hostId, int begin, int limit);
		
		// Relation
		int getRelationCount(int userId, int disc);
		MyUtil::Str2StrMap getRelationA(int visitor, int target) throws RelationException;
		MyUtil::Str2StrMap getRelation(int target, int visitor) throws RelationException;
	};

	interface RelationGroupManager
	{
		MyUtil::Str2StrMap addRelationGroup(int hostId, string name) throws RelationGroupException;
		void removeRelationGroup(int hostId, int groupId);
		void renameRelationGroup(int hostId, int groupId, string name);

		int getRelationGroupCount(int hostId);
		MyUtil::Str2StrMapSeq getRelationGroups(int hostId, int begin, int limit) throws RelationGroupException;
        MyUtil::RowSet getRelationGroupRowSet(int hostId, int begin, int limit) throws RelationGroupException ;
        GroupDataSeq getRelationGroupSeq(int hostId, int begin, int limit) throws RelationGroupException ;
        MyUtil::Str2StrMap getRelationGroup(int hostId, int groupId) throws RelationGroupException;

		void moveBuddy(int hostId, int buddyId, int groupId);
	};

	class InternalManager
	{
    	void reloadRequest(int toId);
    	void reloadBlock(int fromId);
    	void reloadBlocked(int toId);
    	void reloadBuddy(int hostId);
		void reloadRelationGroup(int hostId);

		void addRequest(int toId, MyUtil::Str2StrMap props);
		void addBuddy(int hostId, MyUtil::Str2StrMap props);
		void addBlock(int hostId, MyUtil::Str2StrMap props);
		void addBlocked(int toId, MyUtil::Str2StrMap props);

		void removeRequest(int fromId, int toId);
		void removeBuddy(int hostId, int buddyId);
		void removeBlock(int fromId, int toId);
		void removeBlocked(int fromId, int toId);

		bool containsRequest(int fromId, int toId);
		bool containsBuddy(int hostId, int buddyId);
		bool containsBlock(int fromId, int toId);
		bool containsBlocked(int fromId, int toId);
	};
};
};
};
};
#endif

/* vim: set ts=4 softtabstop=4 shiftwidth=4 : */

