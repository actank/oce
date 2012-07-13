#ifndef GROUPFRIENDBYINFO_H_ 
#define GROUPFRIENDBYINFO_H_
#include "GroupLabel.h"
class GroupFriendByInfo{
public:
	static void newUniteGroupLabel(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel);
	static void uniteGroupLabel(map<int, vector<int> >& finalGroup, map<int, vector<StringInt> >& groupLabel, const float threshold);
	static void uniteSpecificMember(map<int, vector<int> >& finalGroup, map<int, vector<int> >& uniteGroup, const float threshold);
	static void addMembersByInfo(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, vector<int>& tempUngrouping, const int userId);
	static void addMembersByUniv(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
				vector<int>& tempUngrouping, const int userId);
	static void addMembersByOtherUniv(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
				vector<int>& tempUngrouping, const int userId);
	static void addMembersByHighSchool(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
				vector<int>& tempUngrouping);
	static void addMembersByJuniorHighSchool(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
				vector<int>& tempUngrouping);
	static void addMembersByElementarySchool(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
				vector<int>& tempUngrouping);
	static void addMembersByCompany(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
				vector<int>& tempUngrouping);
	static void addMembersByExistingLabel(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, vector<int>& tempUngrouping, const int userId);
};
#endif
