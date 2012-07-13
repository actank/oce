#include "GenericSortMember.h"
void GenericSortMember::sortFriends(const int friendsNumber, int **friendsGraph, vector<int>& userFriend){
    int* membersVolum = new int[friendsNumber];      //内存处理

    for(int i=0; i<friendsNumber; ++i){
        membersVolum[i] = i;
    }

    int* accountVolum = new int[friendsNumber];      //内存处理

    for(int i=0; i<friendsNumber; ++i){
        accountVolum[i] = accountNumber(friendsGraph[i], friendsNumber);
    }

    for(int i=0; i<friendsNumber; ++i){
        for(int j=i+1; j<friendsNumber; ++j){
            if(accountVolum[i] < accountVolum[j]){
                int temp = accountVolum[j];
                accountVolum[j] = accountVolum[i];
                accountVolum[i] = temp;

                int temp_mem = membersVolum[j];
                membersVolum[j] = membersVolum[i];
                membersVolum[i] = temp_mem;
            }
        }
    }

    vector<int> friends;

    for(int i=0; i<friendsNumber; ++i){
        int temp = membersVolum[i];
        friends.push_back(userFriend[temp]);
    }

    userFriend = friends;

    int** newGraph = new int *[friendsNumber];                    //要做内存处理
    for(int i=0; i<friendsNumber; ++i){
        newGraph[i] = new int[friendsNumber];
    }

    for(int i=0; i<friendsNumber; ++i){
        newGraph[i][i] = 0;
        for(int j=i+1; j<friendsNumber; ++j){
            newGraph[i][j] = 0;
            newGraph[j][i] = 0;
        }
    }

    for(int i=0; i<friendsNumber; ++i){
        int temp1 = membersVolum[i];

        for(int j=i+1; j<friendsNumber; ++j){
            int temp2 = membersVolum[j];
            if(friendsGraph[temp1][temp2] == 1){
                newGraph[i][j] = 1;
                newGraph[j][i] = 1;
            }
        }
    }

    for(int i=0; i<friendsNumber; ++i)
        for(int j=0; j<friendsNumber; ++j)
            friendsGraph[i][j] = newGraph[i][j];

    for(int i=0; i<friendsNumber; ++i){               //完成内存处理 
        delete[] newGraph[i];
    }

    delete[] newGraph;
    newGraph = NULL;

    delete[] membersVolum;
    membersVolum = NULL;

    delete[] accountVolum;
    accountVolum = NULL;
}

int GenericSortMember::accountNumber(int* members, const int friendsNumber){
    int account = 0;
    int *p = members;

    while((p = find(p, members+friendsNumber, 1)) != members+friendsNumber){
        ++account;
        ++p;
    }
    return account;
}

void GenericSortMember::newSortAfterUniteGroup(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, const int newGroupAccount, int **friendsGraph,
	 vector<int>& userFriend){
    map<int, vector<int> > afterUniteGroup;
    map<int, vector<StringInt> > afterUniteLabel;

    int accountUniteGroup = 0;

	for(map<int, vector<int> >::iterator mIt=finalNewGroup.begin(); mIt!=finalNewGroup.end(); ++mIt){
		++accountUniteGroup;
    	afterUniteGroup.insert(pair<int, vector<int> >(accountUniteGroup, mIt->second));
        afterUniteLabel.insert(pair<int, vector<StringInt> >(accountUniteGroup, groupLabel[mIt->first]));
	}
    //输出好友前的排序
    sortAfterUniteGroup(afterUniteGroup, friendsGraph, userFriend);

    finalNewGroup = afterUniteGroup;
    groupLabel = afterUniteLabel;
}

void GenericSortMember::sortAfterUniteGroup(map<int, vector<int> >& afterUniteGroup, int **friendsGraph, vector<int>& userFriend){
    map<int, vector<int> > tmpVectorGroup;

	for(map<int, vector<int> >::iterator mIt=afterUniteGroup.begin(); mIt!=afterUniteGroup.end(); ++mIt){
		vector<int> tmpVector;
		vector<int> tmpGroup = mIt->second;

		for(vector<int>::iterator sIt=tmpGroup.begin(); sIt!=tmpGroup.end(); ++sIt){
			int friendIndex = find(userFriend.begin(), userFriend.end(), *sIt) - userFriend.begin();
			tmpVector.push_back(friendIndex);
		}
		tmpVectorGroup.insert(pair<int, vector<int> >(mIt->first, tmpVector));
	}

    sortOutputMember(tmpVectorGroup, friendsGraph);

    afterUniteGroup.clear();

	for(map<int, vector<int> >::iterator mIt=tmpVectorGroup.begin(); mIt!=tmpVectorGroup.end(); ++mIt){
		vector<int> finalGroupMember;
		vector<int> groupMember = mIt->second;
		for(vector<int>::iterator sIt=groupMember.begin(); sIt!=groupMember.end(); ++sIt){
			finalGroupMember.push_back(userFriend[*sIt]);
		}
		afterUniteGroup.insert(pair<int, vector<int> >(mIt->first, finalGroupMember));
	}
}

void GenericSortMember::sortOutputMember(map<int, vector<int> >& group, int **friendsGraph){
	for(map<int, vector<int> >::iterator mIt=group.begin(); mIt!=group.end(); ++mIt){
		vector<int>& groupMember = mIt->second;
        vector<int> accountVolume;
		
		for(vector<int>::iterator sIt=groupMember.begin(); sIt!=groupMember.end(); ++sIt){
			int accountRelationship = accountOutputRelationship(*sIt, groupMember, friendsGraph);
			accountVolume.push_back(accountRelationship);
		}

        for(int j=0; j<accountVolume.size(); ++j){
            for(int k=j+1; k<accountVolume.size(); ++k){

                if(accountVolume[j] < accountVolume[k]){
                    int temp = accountVolume[k];
                    accountVolume[k] = accountVolume[j];
                    accountVolume[j] = temp;

                    int tempMem = groupMember[k];
                    groupMember[k] = groupMember[j];
                    groupMember[j] = tempMem;
                }
            }
        }
	}
}

int GenericSortMember::accountOutputRelationship(const int member, vector<int>& groupMember, int **friendsGraph){
    int totalRelationship = 0;
	for(vector<int>::iterator sIt=groupMember.begin(); sIt!=groupMember.end(); ++sIt){
		if(friendsGraph[member][*sIt] == 1){
            ++totalRelationship;
		}
	}
    return totalRelationship;
}

void GenericSortMember::sortGroupBySize(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel){
    map<int, vector<int> > tmpSortFinalGroup;
    map<int, vector<StringInt> > tmpSortGroupLabel;

    int sortAccount = 0;

    int finalGroupAccount = finalNewGroup.size();

	while(finalNewGroup.size()){
        int maxSize = 0;
        int maxMember = 0;
		for(map<int, vector<int> >::iterator mIt=finalNewGroup.begin(); mIt!=finalNewGroup.end(); ++mIt){
			vector<int> tmpFinalMember = mIt->second;
			if(maxSize < tmpFinalMember.size()){
				maxSize = tmpFinalMember.size();
				maxMember = mIt->first;
			}
		}
	
		++sortAccount;
        vector<int> maxFinalMember = finalNewGroup[maxMember];
        tmpSortFinalGroup.insert(pair<int, vector<int> >(sortAccount, maxFinalMember));

        vector<StringInt> maxGroupLabel = groupLabel[maxMember];
        tmpSortGroupLabel.insert(pair<int, vector<StringInt> >(sortAccount, maxGroupLabel));
        finalNewGroup.erase(maxMember);
	}
    finalNewGroup = tmpSortFinalGroup;
    groupLabel = tmpSortGroupLabel;
}
