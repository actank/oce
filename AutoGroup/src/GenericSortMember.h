#ifndef GENERICSORTMEMBER_H_ 
#define GENERICSORTMEMBER_H_
#include "GroupLabel.h"
class GenericSortMember{
public:
	static void sortFriends(const int friendsNumber, int **friendsGraph, vector<int>& userFriend);
	static int accountNumber(int* members, const int friendsNumber);	
	static void newSortAfterUniteGroup(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, const int newGroupAccount, int **friendsGraph,
		        vector<int>& userFriend);
	static void sortAfterUniteGroup(map<int, vector<int> >& afterUniteGroup, int **friendsGraph, vector<int>& userFriend);
	static void sortOutputMember(map<int, vector<int> >& group, int **friendsGraph);
	static int accountOutputRelationship(const int member, vector<int>& groupMember, int **friendsGraph);
	static void sortGroupBySize(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel);
};
#endif
