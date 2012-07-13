#include "GroupFriendByInfo.h"
void GroupFriendByInfo::newUniteGroupLabel(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel){
    //合并分组
    float threshold = 0;
    uniteGroupLabel(finalNewGroup, groupLabel, threshold);
}

void GroupFriendByInfo::uniteGroupLabel(map<int, vector<int> >& finalGroup, map<int, vector<StringInt> >& groupLabel, const float threshold){
    map<int, vector<int> > uniteGroup;
    vector<int> groupedId;
    int uniteGroupAccount = 0;

    for(int i=1; i<=groupLabel.size(); ++i){
        if(groupLabel[i].at(0).name == "待命名"){
            continue;
        }

        if(find(groupedId.begin(), groupedId.end(), i) != groupedId.end()){
            continue;
        }

        vector<int> tmpUnitedGroup;
        tmpUnitedGroup.push_back(i);
        groupedId.push_back(i);
        vector<StringInt> tag = groupLabel[i];
        vector<string> tmpTagName;

		for(vector<StringInt>::iterator iter=tag.begin(); iter!=tag.end(); ++iter){
			tmpTagName.push_back((*iter).name);
		}

        for(int j=i+1; j<=groupLabel.size(); ++j){
            if(groupLabel[j].at(0).name == "待命名"){
                continue;
            }

            if(find(groupedId.begin(), groupedId.end(), j) != groupedId.end()){
                continue;
            }

            vector<StringInt> subTag = groupLabel[j];

			for(vector<StringInt>::iterator iter=subTag.begin(); iter!=subTag.end(); ++iter){
				if(find(tmpTagName.begin(), tmpTagName.end(), (*iter).name) != tmpTagName.end()){
					tmpUnitedGroup.push_back(j);
                    groupedId.push_back(j);
                    break;
				}
			}
        }
        ++uniteGroupAccount;
        uniteGroup.insert(pair<int, vector<int> >(uniteGroupAccount, tmpUnitedGroup));
    }
    uniteSpecificMember(finalGroup, uniteGroup, threshold);
}

void GroupFriendByInfo::uniteSpecificMember(map<int, vector<int> >& finalGroup, map<int, vector<int> >& uniteGroup, const float threshold){
	if(threshold != 0){
		return;
	}
	for(map<int, vector<int> >::iterator iter=uniteGroup.begin(); iter!=uniteGroup.end(); ++iter)
	{
		vector<int> groupNumber = iter->second;
		if(groupNumber.size() == 1){
  	       continue;
      	}

		int maxGroup = 0;
        int maxSize = 0;
		for(vector<int>::iterator sIt=groupNumber.begin(); sIt!=groupNumber.end(); ++sIt){
			if(finalGroup[*sIt].size() > maxSize){
    	        maxGroup = *sIt;
    		    maxSize = finalGroup[*sIt].size();
          	}
		}

		map<int, vector<int> >::iterator mIt = finalGroup.find(maxGroup);
    	vector<int>& groupMember1 = mIt->second;
		vector<int> groupMember2;
		for(vector<int>::iterator sIt2=groupNumber.begin(); sIt2!=groupNumber.end(); ++sIt2){
			if(maxGroup != *sIt2){
				groupMember2 = finalGroup[*sIt2];
				for(vector<int>::iterator sIt3=groupMember2.begin(); sIt3!=groupMember2.end(); ++sIt3){
					if(find(groupMember1.begin(), groupMember1.end(), *sIt3) == groupMember1.end()){
             	       groupMember1.push_back(*sIt3);
                    }
				}
				finalGroup.erase(*sIt2);
			}
		}
	}
}

void GroupFriendByInfo::addMembersByInfo(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, vector<int>& tempUngrouping, const int userId){
    map<int, NetworkBigObjectNPtr> networkObjectMap;

	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
        NetworkBigObjectNPtr p = UserNetworkAdapter::instance().getBigObjectN(*sIt);
        networkObjectMap.insert(pair<int, NetworkBigObjectNPtr>(*sIt, p));
	}
    //cout<<"********************************************************"<<endl;
    addMembersByUniv(finalNewGroup, groupLabel, networkObjectMap, tempUngrouping, userId);
    addMembersByOtherUniv(finalNewGroup, groupLabel, networkObjectMap, tempUngrouping, userId);
    addMembersByHighSchool(finalNewGroup, groupLabel, networkObjectMap, tempUngrouping);
    addMembersByJuniorHighSchool(finalNewGroup, groupLabel, networkObjectMap, tempUngrouping);
    addMembersByElementarySchool(finalNewGroup, groupLabel, networkObjectMap, tempUngrouping);
    addMembersByCompany(finalNewGroup, groupLabel, networkObjectMap, tempUngrouping);
    addMembersByExistingLabel(finalNewGroup, groupLabel, tempUngrouping, userId);
    //cout<<"********************************************************"<<endl;
}

void GroupFriendByInfo::addMembersByUniv(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
	 vector<int>& tempUngrouping, const int userId)
{
    vector<string> hostUnivName;
    NetworkBigObjectNPtr hostInfo = UserNetworkAdapter::instance().getBigObjectN(userId);
    UniversityInfoSeq &hostUnivList = hostInfo->universitys;

	for(UniversityInfoSeq::iterator iter=hostUnivList.begin(); iter!=hostUnivList.end(); ++iter){
		hostUnivName.push_back((*iter)->universityName);
	}

    vector<string> groupLabelName;

	for(map<int, vector<StringInt> >::iterator mIt=groupLabel.begin(); mIt!=groupLabel.end(); ++mIt){
		groupLabelName.push_back(mIt->second.at(0).name);
	}

    map<int, NetworkBigObjectNPtr> tmpNetworkObjectMap(networkObjectMap);

    //按同学校＋同学院＋同入学年添加进对应的组
	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
        int insertSignal = 0;
        map<int, NetworkBigObjectNPtr>::iterator it = tmpNetworkObjectMap.find(*sIt);
		if(it != tmpNetworkObjectMap.end()){
			UniversityInfoSeq &univList = it->second->universitys;
			for(UniversityInfoSeq::iterator iter=univList.begin(); iter!=univList.end(); ++iter){
				string univLabelName = (*iter)->universityName;
				if((*iter)->universityName == ""){
					continue;
				}

				univLabelName += (*iter)->department;
				if((*iter)->department == ""){
					continue;
				}

				char tmpstr[25];
				sprintf(tmpstr, "%d", (*iter)->enrollYear);
				string enrollYear(tmpstr);
				if(enrollYear == ""){
					continue;
				}

				if(enrollYear.size() == 4){
					univLabelName += enrollYear.substr(2,2);
					vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), univLabelName);

					if(labelIter != groupLabelName.end()){
						int index = labelIter - groupLabelName.begin();
						vector<int>& finalGroupMember = finalNewGroup[index + 1];
						finalGroupMember.insert(finalGroupMember.begin(), *sIt);
						insertSignal = 1;
					}
				}
			}

			if(insertSignal){
				tmpNetworkObjectMap.erase(*sIt);
			}
		}
	}

    //按同学校＋同学院添加到对应的组中
	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = tmpNetworkObjectMap.find(*sIt)) == tmpNetworkObjectMap.end()){
			continue;
		}

		UniversityInfoSeq &univList = it->second->universitys;
		for(UniversityInfoSeq::iterator iter=univList.begin(); iter!=univList.end(); ++iter){
            string univLabelName = (*iter)->universityName;
			if((*iter)->universityName == ""){
                continue;
			}

			univLabelName += (*iter)->department;
			if((*iter)->department == ""){
                continue;
			}
		
			vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), univLabelName);

			if(labelIter != groupLabelName.end()){
				int index = labelIter - groupLabelName.begin();
				vector<int>& finalGroupMember = finalNewGroup[index + 1];

				if(find(finalGroupMember.begin(), finalGroupMember.end(), *sIt) == finalGroupMember.end()){
					finalGroupMember.insert(finalGroupMember.begin(), *sIt);
					if(tmpNetworkObjectMap.find(*sIt) != tmpNetworkObjectMap.end()){
						tmpNetworkObjectMap.erase(*sIt);
					}
				}
			}	
		}
	}

    //按同学校添加到对应的组中  
	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = tmpNetworkObjectMap.find(*sIt)) == tmpNetworkObjectMap.end()){
            continue;
        }

		UniversityInfoSeq &univList = it->second->universitys;
		for(UniversityInfoSeq::iterator iter=univList.begin(); iter!=univList.end(); ++iter){
			string univLabelName = (*iter)->universityName;	
			if(find(hostUnivName.begin(), hostUnivName.end(), univLabelName) == hostUnivName.end()){
				continue;
			}

			vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), univLabelName);
			if(labelIter != groupLabelName.end()){
				int index = labelIter - groupLabelName.begin();
				vector<int>& finalGroupMember = finalNewGroup[index + 1];
				if(find(finalGroupMember.begin(), finalGroupMember.end(), *sIt) == finalGroupMember.end()){
					finalGroupMember.insert(finalGroupMember.begin(), *sIt);
				}
			}
		}
	}
}

void GroupFriendByInfo::addMembersByOtherUniv(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 	  vector<int>& tempUngrouping, const int userId){
    vector<string> hostUnivName;
    NetworkBigObjectNPtr hostInfo = UserNetworkAdapter::instance().getBigObjectN(userId);
    UniversityInfoSeq &hostUnivList = hostInfo->universitys;

	for(UniversityInfoSeq::iterator iter=hostUnivList.begin(); iter!=hostUnivList.end(); ++iter){
        hostUnivName.push_back((*iter)->universityName);
    }

    vector<string> groupLabelName;

	for(map<int, vector<StringInt> >::iterator mIt=groupLabel.begin(); mIt!=groupLabel.end(); ++mIt){
        groupLabelName.push_back(mIt->second.at(0).name);
    }

	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = networkObjectMap.find(*sIt)) == networkObjectMap.end()){
            continue;
        }

		UniversityInfoSeq &univList = it->second->universitys;

		for(UniversityInfoSeq::iterator iter=univList.begin(); iter!=univList.end(); ++iter){
			string univLabelName = (*iter)->universityName;
			if((*iter)->universityName == ""){
                continue;
            }

			if(find(hostUnivName.begin(), hostUnivName.end(), univLabelName) != hostUnivName.end()){
                continue;
            }

			vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), univLabelName);
			if(labelIter != groupLabelName.end()){
				int index = labelIter - groupLabelName.begin();
				vector<int>& finalGroupMember = finalNewGroup[index + 1];
				if(find(finalGroupMember.begin(), finalGroupMember.end(), *sIt) == finalGroupMember.end()){
					finalGroupMember.insert(finalGroupMember.begin(), *sIt);
				}
			}
		}
	}	
}

void GroupFriendByInfo::addMembersByHighSchool(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 	  vector<int>& tempUngrouping){
    vector<string> groupLabelName;

	for(map<int, vector<StringInt> >::iterator iter=groupLabel.begin(); iter!=groupLabel.end(); ++iter){
		groupLabelName.push_back(iter->second.at(0).name);
	}

	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = networkObjectMap.find(*sIt)) == networkObjectMap.end()){
			continue;
		}

		HighSchoolInfoSeq &highList = it->second->highschools;
		for(HighSchoolInfoSeq::iterator iter=highList.begin(); iter!=highList.end(); ++iter){
			string highSchoolLabelName = (*iter)->HighSchoolName;	
			if(highSchoolLabelName == ""){
				continue;
			}
			
			vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), highSchoolLabelName);

			if(labelIter != groupLabelName.end()){
				int index = labelIter - groupLabelName.begin();
				vector<int>& finalGroupMember = finalNewGroup[index + 1];
				if(find(finalGroupMember.begin(), finalGroupMember.end(), *sIt) == finalGroupMember.end()){
                    finalGroupMember.insert(finalGroupMember.begin(), *sIt);
                }
			}
		}
	}	
}

void GroupFriendByInfo::addMembersByJuniorHighSchool(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, 
	 map<int, NetworkBigObjectNPtr>& networkObjectMap, vector<int>& tempUngrouping){

    vector<string> groupLabelName;

	for(map<int, vector<StringInt> >::iterator iter=groupLabel.begin(); iter!=groupLabel.end(); ++iter){
		groupLabelName.push_back(iter->second.at(0).name);
	}
	
	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = networkObjectMap.find(*sIt)) == networkObjectMap.end()){
			continue;
		}

		JuniorHighSchoolInfoPtr &juniorPtr = it->second->juniorhighschool;
		if(!juniorPtr){
            continue;
        }

		string juniorHighSchoolLabelName = juniorPtr->juniorHighSchoolName;
		if(juniorHighSchoolLabelName == ""){
            continue;
        }

		vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), juniorHighSchoolLabelName);
		if(labelIter != groupLabelName.end()){
            int index = labelIter - groupLabelName.begin();
            vector<int>& finalGroupMember = finalNewGroup[index + 1];
            finalGroupMember.insert(finalGroupMember.begin(), *sIt);
        }
	}
}

void GroupFriendByInfo::addMembersByElementarySchool(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, 
	 map<int, NetworkBigObjectNPtr>& networkObjectMap, vector<int>& tempUngrouping){

    vector<string> groupLabelName;
	for(map<int, vector<StringInt> >::iterator iter=groupLabel.begin(); iter!=groupLabel.end(); ++iter){
		groupLabelName.push_back(iter->second.at(0).name);
	}

	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = networkObjectMap.find(*sIt)) == networkObjectMap.end()){
            continue;
        }

		ElementarySchoolInfoPtr &elePtr = it->second->elementary;
		if(!elePtr){
            continue;
        }

		string elementarySchoolLabelName = elePtr->elementarySchoolName;
		if(elementarySchoolLabelName == ""){
            continue;
        }

		vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), elementarySchoolLabelName);
		if(labelIter != groupLabelName.end()){
            int index = labelIter - groupLabelName.begin();
            vector<int>& finalGroupMember = finalNewGroup[index + 1];
            finalGroupMember.insert(finalGroupMember.begin(), *sIt);
        }
	}
}

void GroupFriendByInfo::addMembersByCompany(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, map<int, NetworkBigObjectNPtr>& networkObjectMap, 
	 vector<int>& tempUngrouping){

    vector<string> groupLabelName;
	for(map<int, vector<StringInt> >::iterator iter=groupLabel.begin(); iter!=groupLabel.end(); ++iter){
		groupLabelName.push_back(iter->second.at(0).name);
	}

	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, NetworkBigObjectNPtr>::iterator it;
		if((it = networkObjectMap.find(*sIt)) == networkObjectMap.end()){
            continue;
        }

		WorkspaceInfoSeq &workList = it->second->workspaces;
		for(WorkspaceInfoSeq::iterator iter=workList.begin(); iter!=workList.end(); ++iter){
			string companyLabelName = (*iter)->workplaceName;
			if(companyLabelName == ""){
                continue;
            }

			vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), companyLabelName);
			if(labelIter != groupLabelName.end()){
                int index = labelIter - groupLabelName.begin();
                vector<int>& finalGroupMember = finalNewGroup[index + 1];

                if(find(finalGroupMember.begin(), finalGroupMember.end(), *sIt) == finalGroupMember.end()){
                    finalGroupMember.insert(finalGroupMember.begin(), *sIt);
                }
            }
		}
	}
}

void GroupFriendByInfo::addMembersByExistingLabel(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, vector<int>& tempUngrouping, const int userId)
{
    vector<string> groupLabelName;

	for(map<int, vector<StringInt> >::iterator iter=groupLabel.begin(); iter!=groupLabel.end(); ++iter){
		groupLabelName.push_back(iter->second.at(0).name);
	}

    map<int, vector<string> > existingLabelMap = BuddyGroupAdapter::instance().getBuddiedGroupsMap(userId);

	for(vector<int>::iterator sIt=tempUngrouping.begin(); sIt!=tempUngrouping.end(); ++sIt){
		map<int, vector<string> >::iterator it = existingLabelMap.find(*sIt);
		if(it == existingLabelMap.end()){
			continue;
		}
		
		string existingLabelName = it->second.at(0);
		if(existingLabelName == ""){
            continue;
        }

		vector<string>::iterator labelIter = find(groupLabelName.begin(), groupLabelName.end(), existingLabelName);
		if(labelIter != groupLabelName.end()){
            int index = labelIter - groupLabelName.begin();
            vector<int>& finalGroupMember = finalNewGroup[index + 1];
            finalGroupMember.insert(finalGroupMember.begin(), *sIt);
        }
	}
}
