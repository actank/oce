#ifndef __BUDDY_GROUP_ICE__
#define __BUDDY_GROUP_ICE__

#include <Util.ice>

module com
{
module xiaonei
{
module model
{

dictionary<int, MyUtil::StrSeq> BuddiedGroupsMap;

dictionary<int, MyUtil::Int2StrMap> BuddyGroupsMap;

class BuddyGroupManager
{
	//write
	void addGroup(int host, string groupName);
	
	void removeGroup(int host, string groupName);	
	void removeGroupWithGid(int host, int groupId);
	
	void cleanGroup(int host, string groupName);
	void cleanGroupWithGid(int host, int groupId);
	
	void renameGroup(int host, string oldName, string newName);
	void renameGroupWithGid(int host, int groupId, string newName);
	
	void addBuddyToGroup(int host, string groupName, int buddy);
	void addBuddyToGroupWithGid(int host, int groupId, int buddy);
	
	void addBuddiesToGroup(int host, string groupName, MyUtil::IntSeq buddies);
	void addBuddiesToGroupWithGid(int host, int groupId, MyUtil::IntSeq buddies);

	void setBuddiesToGroup(int host, string groupName, MyUtil::IntSeq buddies);
	void setBuddiesToGroupWithGid(int host, int groupId, MyUtil::IntSeq buddies);
	
	void removeBuddyFromGroup(int host, string groupName, int buddy);
	void removeBuddyFromGroupWithGid(int host, int groupId, int buddy);
	
	void removeBuddy(int host, int buddy);
	
	int changeBuddyGroups(int host, int buddy, MyUtil::StrSeq nameSeq);
	int changeBuddyGroupsWithGid(int host, int buddy, MyUtil::IntSeq groupIdSeq);

	void changeBuddyGroupsForWeb(int host, int buddy, MyUtil::StrSeq nameSeq);
	void changeBuddyGroupsForWebWithGid(int host, int buddy, MyUtil::IntSeq groupIdSeq);
	
	//read
	BuddiedGroupsMap getBuddiedGroupsMap(int host);
  BuddyGroupsMap getBuddyGroupsMap(int host);
	
	MyUtil::StrSeq getGroupList(int host);
	MyUtil::Int2StrMap getGroupListData(int host);

  MyUtil::IntSeq getBuddyList(int host, string groupName, int begin, int limit);
	MyUtil::IntSeq getBuddyListWithGid(int host, int groupId, int begin, int limit);
  
  int getCountByGroup(int host,string groupName);
  int getCountByGroupId(int host, int groupId);
	
	MyUtil::StrSeq getBuddyGroups(int host, int guest);
  MyUtil::Int2StrMap getBuddyGroupsData(int host, int guest);
	
	MyUtil::Str2IntMap getAllGroupsBuddyCount( int host);
  MyUtil::Int2IntMap getAllGroupsBuddyCountData( int host);

	void load(int host);	
	void saveLoad(int host);
	//for im only
	//int buddyChangeGroups(int host, int buddy, MyUtil::StrSeq nameSeq);
	
	
};

class BuddyGroupReader
{
	//write
	void addGroup(int host, string groupName);
	
	void removeGroup(int host, string groupName);	
	void removeGroupWithGid(int host, int groupId);
	
	void cleanGroup(int host, string groupName);
	void cleanGroupWithGid(int host, int groupId);
	
	void renameGroup(int host, string oldName, string newName);
	void renameGroupWithGid(int host, int groupId, string newName);
	
	void addBuddyToGroup(int host, string groupName, int buddy);
	void addBuddyToGroupWithGid(int host, int groupId, int buddy);
	
	void addBuddiesToGroup(int host, string groupName, MyUtil::IntSeq buddies);
	void addBuddiesToGroupWithGid(int host, int groupId, MyUtil::IntSeq buddies);

	void setBuddiesToGroup(int host, string groupName, MyUtil::IntSeq buddies);
	void setBuddiesToGroupWithGid(int host, int groupId, MyUtil::IntSeq buddies);
	
	void removeBuddyFromGroup(int host, string groupName, int buddy);
	void removeBuddyFromGroupWithGid(int host, int groupId, int buddy);
	
	void removeBuddy(int host, int buddy);
	
	int changeBuddyGroups(int host, int buddy, MyUtil::StrSeq nameSeq);
	int changeBuddyGroupsWithGid(int host, int buddy, MyUtil::IntSeq groupIdSeq);

	void changeBuddyGroupsForWeb(int host, int buddy, MyUtil::StrSeq nameSeq);
	void changeBuddyGroupsForWebWithGid(int host, int buddy, MyUtil::IntSeq groupIdSeq);
	
	//read
	BuddiedGroupsMap getBuddiedGroupsMap(int host);
  BuddyGroupsMap getBuddyGroupsMap(int host);
	
	MyUtil::StrSeq getGroupList(int host);
	MyUtil::Int2StrMap getGroupListData(int host);

  MyUtil::IntSeq getBuddyList(int host, string groupName, int begin, int limit);
	MyUtil::IntSeq getBuddyListWithGid(int host, int groupId, int begin, int limit);
  
  int getCountByGroup(int host,string groupName);
  int getCountByGroupId(int host, int groupId);
	
	MyUtil::StrSeq getBuddyGroups(int host, int guest);
  MyUtil::Int2StrMap getBuddyGroupsData(int host, int guest);
	
	MyUtil::Str2IntMap getAllGroupsBuddyCount( int host);
  MyUtil::Int2IntMap getAllGroupsBuddyCountData( int host);

	void load(int host);	
	void saveLoad(int host);
	//for im only
	//int buddyChangeGroups(int host, int buddy, MyUtil::StrSeq nameSeq);
	
	
};


class BuddyGroupWriter
{
	//write
	void addGroup(int host, string groupName);
	
	void removeGroup(int host, string groupName);	
	void removeGroupWithGid(int host, int groupId);
	
	void cleanGroup(int host, string groupName);
	void cleanGroupWithGid(int host, int groupId);
	
	void renameGroup(int host, string oldName, string newName);
	void renameGroupWithGid(int host, int groupId, string newName);
	
	void addBuddyToGroup(int host, string groupName, int buddy);
	void addBuddyToGroupWithGid(int host, int groupId, int buddy);
	
	void addBuddiesToGroup(int host, string groupName, MyUtil::IntSeq buddies);
	void addBuddiesToGroupWithGid(int host, int groupId, MyUtil::IntSeq buddies);

	void setBuddiesToGroup(int host, string groupName, MyUtil::IntSeq buddies);
	void setBuddiesToGroupWithGid(int host, int groupId, MyUtil::IntSeq buddies);
	
	void removeBuddyFromGroup(int host, string groupName, int buddy);
	void removeBuddyFromGroupWithGid(int host, int groupId, int buddy);
	
	void removeBuddy(int host, int buddy);
	
	int changeBuddyGroups(int host, int buddy, MyUtil::StrSeq nameSeq);
	int changeBuddyGroupsWithGid(int host, int buddy, MyUtil::IntSeq groupIdSeq);

	void changeBuddyGroupsForWeb(int host, int buddy, MyUtil::StrSeq nameSeq);
	void changeBuddyGroupsForWebWithGid(int host, int buddy, MyUtil::IntSeq groupIdSeq);
	
	//read

	void load(int host);	
	void saveLoad(int host);
	//for im only
	//int buddyChangeGroups(int host, int buddy, MyUtil::StrSeq nameSeq);
	
	
};



};
};
};
#endif

