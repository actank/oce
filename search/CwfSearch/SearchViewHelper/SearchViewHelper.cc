/*
 * =====================================================================================
 *
 *       Filename:  UserView.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/16/2011 07:10:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "search/CwfSearch/SearchViewHelper/SearchViewHelper.h"
#include "base/logging.h"

int SearchView::getId()  {
  return _id;
}

string SearchView::getName()  {
  return _name;
}

string SearchView::getHeadUrl()  {
  return _headurl;
}

string SearchView::getTinyUrl()  {
  return _tinyurl;
}

IndexType SearchView::getIndexType() {
  return _indextype;
}

void SearchView::setId(int id)  {
  _id = id;
}

void SearchView::setName(string name)  {
  _name = name;
}

void SearchView::setHeadUrl(string headurl)  {
  _headurl = headurl;
}

void SearchView::setTinyUrl(string tinyurl)  {
  _tinyurl = tinyurl;
}

void SearchView::setIndexType(IndexType type) {
  _indextype = type;
}

void SearchUserView::setStatus(int status)  {
  _status = status;
}

void SearchUserView::setState(int state)  {
  _state = state;
}

void SearchUserView::setCommonFriendsNum(string common_friends_num)  {
  _common_friends_num = common_friends_num; 
}

void SearchUserView::setFriendType(string friend_type)  {
  _friend_type = friend_type;
}

void SearchUserView::setIsFriend(string is_friend)  {
  _is_friend = is_friend;
}

void SearchUserView::setGender(string gender)  {
  _gender = gender;
}

bool SearchUserView::isFriend()  {
  if(_is_friend == "1")  return true;
  else return false;
}

bool SearchUserView::isSelected()  {
  return 0 != (_state & (1 << CStateSELECTED));
}

bool SearchUserView::isVipMember()  {
  return 0 != (_state & (1 << CStateVIPMEMBER));
}

bool SearchUserView::isMSNIMME()  {
  return 0 != (_state & (1 << CStateMSNIMME)); 
}

bool SearchUserView::isMobileBind()  {
  return 0 != (_state & (1 << CStateMOBILEBIND));
}

string SearchUserView::getCommonFriendsNum()  {
  return _common_friends_num;
}

string SearchUserView::getFriendType()  {
  return _friend_type; 
}

string SearchUserView::getGender()  {
  return _gender;
}

int SearchUserView::getStatus()  {
  return _status;
}

void SearchUserView::addNetwork(string network_name)  {
  _network.push_back(network_name);
}

void SearchUserView::addEducation(string school_name)  {
  _education.push_back(school_name);
}

bool SearchUserView::isWorkplacesExist(string workplace_name) {
  vector<string>::iterator ret = std::find(_workplaces.begin(), _workplaces.end(),workplace_name);
  if (ret == _workplaces.end()) {
    return false;
  } else {
    return true;
  }
}

bool SearchUserView::isEducationExist(string school_name) {
  vector<string>::iterator ret = std::find(_education.begin(), _education.end(),school_name);
  if (ret == _education.end()) {
    return false;
  } else {
    return true;
  }
}

bool SearchUserView::isRegionExist(string region_name) {
  vector<string>::iterator ret = std::find(_region.begin(), _region.end(),region_name);
  if (ret == _region.end()) {
    return false;
  } else {
    return true;
  }
}

void SearchUserView::addWorkplaces(string workplace_name)  {
  _workplaces.push_back(workplace_name);
}

void SearchUserView::addRegion(string region_name)  {
  _region.push_back(region_name);
}

void SearchUserView::addFinalView(string final_name)  {
  _final_view.push_back(final_name);
}

vector<string> SearchUserView::getFinalView()  {
  return _final_view;
}

string SearchAppView::getDescription()  {
  return _description;
}

string SearchAppView::getCompany()  {
  return _company;
}

string SearchAppView::getSideNavUrl()  {
  return _side_nav_url;
}

string SearchAppView::getLogo() {
  return _logo;
}

void SearchAppView::setDescription(string description)  {
  _description = description;
}

void SearchAppView::setCompany(string company)  {
  _company = company;
}

void SearchAppView::setSideNavUrl(string side_nav_url)  {
  _side_nav_url = side_nav_url;
}

void SearchAppView::setLogo(string logo)  {
  _logo = logo;
}

int SearchPageView::getPType()  {
  return _ptype;
}

int SearchPageView::getFansCount()  {
  return _fans_count;
}

string SearchPageView::getSubType()  {
  return _sub_type;
}

void SearchPageView::setPType(int ptype)  {
  _ptype = ptype;
}

void SearchPageView::setFansCount(int fans_count)  {
  _fans_count = fans_count;
}

void SearchPageView::setSubType(string sub_type)  {
  _sub_type = sub_type;
}

int& SearchResult::getTotalCount()  {
  return _total_count;
}

int SearchResult::getCurrentPosition()  {
  return _current_position;
}

double SearchResult::getTimeCost()  {
  return _time_cost;
}

vector<SearchViewPtr>&  SearchResult::getContents()  {
  return _contents;
}

map<IndexType, int>&  SearchResult::getTypedCount()  {
  return _typed_count;
}

void SearchResult::setTotalCount(int total_count)  {
  _total_count = total_count;
}

void SearchResult::setCurrentPosition(int current_position)  {
  _current_position = current_position;
}

void SearchResult::setTimeCost(double time_cost)  {
  _time_cost = time_cost;
}

void SearchResult::setContents(vector<SearchViewPtr>& contents)  {
  _contents.assign(contents.begin(), contents.end());
}

void SearchResult::setTypedCount(map<IndexType, int>& typed_count)  {
  for(map<IndexType, int>::iterator iter = typed_count.begin(); iter != typed_count.end(); iter++)  {
    _typed_count[iter->first] = iter->second;
  }
}

SearchViewHelper::SearchViewHelper(int userid)  {
  _userid = userid;
  _result_ptr = new SearchResult();
//  xce::dist::searchcache::DistSearchCacheAdapter::instance().initialize();
}

SearchResultPtr SearchViewHelper::searchViewFilter(IndexResultPtr result)  {
  divideView(result); 
  long start = get_now();
  generateUserView();
  long end = get_now();
  LOG(INFO) << "SearchViewHelper TIMECOST getUserCache:" << end - start << "ms";
  long last = end;
  generatePageView();
  end = get_now();
  LOG(INFO) << "SearchViewHelper TIMECOST getPageCache:" << end - last << "ms";
  last = end;
  generateFinalView(result);
  end = get_now();
  LOG(INFO) << "SearchViewHelper TIMECOST generateFinalView:" << end - last << "ms";
  return _result_ptr;
}

bool SearchViewHelper::isAdmin(long userid)  {  // unimplementation
  return false;
}

void SearchViewHelper::divideView(IndexResultPtr result)  {
  _result_ptr->setTotalCount(result->totalCount);
  _result_ptr->setCurrentPosition(result->currentPos);
  _result_ptr->setTimeCost(result->timeCost);
  _result_ptr->setTypedCount(result->typedCount);
  vector<int> use_for_dangers;
  for(IndexResultItemSeq::iterator iter = result->contents.begin(); iter != result->contents.end(); iter++)  {
    if(iter->type == User)  {
      _user_ids.push_back((long)(iter->id));
      use_for_dangers.push_back(iter->id);
      _common_friends.push_back(iter->content[COMMON_FRIENDS_NUM]);
      _friends_type.push_back(iter->content[FRIEND_TYPE]);
      _is_friend.push_back(iter->content[IS_FRIEND]);
      /* if(UserDangerAdapter::instance().isSearchShielding(iter->id))  {//Todo:  batch interface
         _user_danger.push_back(true);
         }  else  {
         _user_danger.push_back(false);
         }*/
    }  else if(iter->type == Page)  {
      _page_ids.push_back((long)(iter->id));
    }
  }

  map<int, bool>  user_danger_map = UserDangerAdapter::instance().isSearchShielding(use_for_dangers);
  for(map<int, bool>::iterator iter = user_danger_map.begin(); iter != user_danger_map.end(); iter++)  {
    _user_danger.push_back(iter->second);
  }
}

void SearchViewHelper::generateUserView()  {//TODO:  站外用户
  if( _user_ids.size() > 0 )  {
    long start = get_now();
    filterPrivacy(_userid, _user_ids);
    long end = get_now();
    LOG(INFO) << "SearchViewHelper TIMECOST Privacy Cost:" << end - start << "ms";
  }
}

void SearchViewHelper::generatePageView()  {
  if(_page_ids.size() > 0)  {
    map<long, SearchCacheMemPageDataPtr> page_results = DistSearchCacheAdapter::instance().getPageData(_page_ids);
    for(vector<long>::iterator it = _page_ids.begin(); it != _page_ids.end(); it++)  {
      map<long, SearchCacheMemPageDataPtr>::iterator iter = page_results.find(*it);
      if(iter == page_results.end() || iter->first == 1){
        continue;
      }
      SearchPageViewPtr page_tmp = new SearchPageView();
      page_tmp->setId(iter->first);
      page_tmp->setName((iter->second)->name());
      page_tmp->setHeadUrl((iter->second)->headurl());
      page_tmp->setTinyUrl((iter->second)->tinyurl());
      page_tmp->setPType((iter->second)->ptype());
      page_tmp->setSubType((iter->second)->subtype());
//      LOG(INFO) << "SUBTYPE:" << (iter->second)->subtype();
      page_tmp->setFansCount((iter->second)->fanscount());
      _page_contents.push_back(page_tmp); 
    }
  }
}


void SearchViewHelper::generateFinalView(IndexResultPtr result)  {
  vector<SearchUserViewPtr>::iterator view_iter = _user_contents.begin();
  vector<SearchPageViewPtr>::iterator page_view_iter = _page_contents.begin();
  vector<bool>::iterator  user_danger_iter = _user_danger.begin();
//  LOG(INFO) << "RESULT.SIZE:" << result->contents.size() << " USER_CONTENTS.SIZE:" << _user_contents.size();

  for(IndexResultItemSeq::iterator iter = result->contents.begin(); iter != result->contents.end(); iter++)  {
    switch((*iter).type)  {
      case User : { if(view_iter == _user_contents.end())  {
                    _result_ptr->getTotalCount()--;
                    (_result_ptr->getTypedCount())[User]--;
                    continue;
                  }
                  (*view_iter)->setIndexType((*iter).type);
                  if((*view_iter)->getStatus() < 3 && !(*user_danger_iter) )  { 
                    _result_ptr->getContents().push_back(*view_iter);
                  }  else  {
//                    if((*view_iter)->getStatus() >= 3)  {
//                      LOG(INFO)<<"STATUS >= 3 ID "<<(*iter).id;
//                    }  else  {
//                      LOG(INFO)<<"UserDanger ID "<<(*iter).id;
//                    }
                    _result_ptr->getTotalCount()--;
                    (_result_ptr->getTypedCount())[User]--;
                  }
                  user_danger_iter++;
                  view_iter++; 
                  break; 
                  }

      case Page : { 
                    if(page_view_iter == _page_contents.end())  {
                      _result_ptr->getTotalCount()--;
                      (_result_ptr->getTypedCount())[Page]--;
                      continue;
                    }
                    (*page_view_iter)->setIndexType((*iter).type);
                    _result_ptr->getContents().push_back(*page_view_iter);
//                    LOG(INFO)<<"Page ID "<<(*iter).id;
                    page_view_iter++; 
                    break; 
                  } 

      case App  : { SearchAppViewPtr app_view = new SearchAppView();
                    app_view->setIndexType((*iter).type);
                    app_view->setId((*iter).id);
                    app_view->setName((*iter).content[APP_NAME]);
                    app_view->setHeadUrl((*iter).content[HEADURL]);
                    app_view->setTinyUrl((*iter).content[TINYURL]);
                    app_view->setDescription((*iter).content[APP_DESCRIPTION]);
                    app_view->setSideNavUrl((*iter).content[APP_SIDE_NAV_URL]);
                    app_view->setLogo((*iter).content[APP_LOGO]);
                    app_view->setCompany((*iter).content[APP_COMPANY]);
                    _result_ptr->getContents().push_back(app_view);
                    break;
                  }
    } 
  }
}

void SearchViewHelper::assembleUserView(SearchUserViewPtr view_ptr, SearchCacheMemDataPtr mem_ptr, string is_friend, string friend_type, string common_friends_num, bool basic_info, bool education, bool workplaces)  {
  view_ptr->setState(mem_ptr->state());
  view_ptr->setStatus(mem_ptr->status());
  if (friend_type == "0") {
    view_ptr->setCommonFriendsNum("");
  } else {
    view_ptr->setCommonFriendsNum(common_friends_num);
  }
  view_ptr->setFriendType(friend_type);
  view_ptr->setIsFriend(is_friend);
  view_ptr->setTinyUrl(mem_ptr->tinyurl()); 
  view_ptr->setGender(mem_ptr->gender());
  set<string> network_names;
  for(int i = 0; i < mem_ptr->networks_size(); i++) { 
    network_names.insert(mem_ptr->networks(i).name());
  }

  if(!basic_info && !education && !workplaces) {
    for(set<string>::iterator iter = network_names.begin(); iter != network_names.end(); iter++)  {
      view_ptr->addNetwork(*iter);
      view_ptr->addFinalView(*iter);
    }
  } else {
    if(workplaces)  {
      for(int i = 0; i < mem_ptr->workplaces_size() && i < WORK_PLACE_LIMIT; i++)  {
        view_ptr->addWorkplaces(mem_ptr->workplaces(i).name());
        view_ptr->addFinalView(mem_ptr->workplaces(i).name());
      }  
    }  else  {
      for(int i = 0; i < mem_ptr->workplaces_size() && i < WORK_PLACE_LIMIT; i++)  {
        if(network_names.find(mem_ptr->workplaces(i).name()) != network_names.end())  {
          view_ptr->addWorkplaces(mem_ptr->workplaces(i).name());
          view_ptr->addFinalView(mem_ptr->workplaces(i).name());
        }
      }
    }

    int edu_count = 0;
    for(int i = 0; i < mem_ptr->universityschools_size() && edu_count < SCHOOL_LIMIT; i++)  {
      if (!view_ptr->isEducationExist(mem_ptr->universityschools(i).name())) {
        if(education)  {
          view_ptr->addEducation(mem_ptr->universityschools(i).name());
          view_ptr->addFinalView(mem_ptr->universityschools(i).name());
        }  else  {
          if(network_names.find(mem_ptr->universityschools(i).name()) != network_names.end())  {
            view_ptr->addEducation(mem_ptr->universityschools(i).name());
            view_ptr->addFinalView(mem_ptr->universityschools(i).name());
          }
        }
        edu_count++;
      }
    }

    for(int i = 0; i < mem_ptr->collegeschools_size() && edu_count < SCHOOL_LIMIT; i++)  {
      if (!view_ptr->isEducationExist(mem_ptr->collegeschools(i).name())) {
        if(education)  {
          view_ptr->addEducation(mem_ptr->collegeschools(i).name());
          view_ptr->addFinalView(mem_ptr->collegeschools(i).name());
        }  else  {
          if(network_names.find(mem_ptr->collegeschools(i).name()) != network_names.end())  {
            view_ptr->addEducation(mem_ptr->collegeschools(i).name());
            view_ptr->addFinalView(mem_ptr->collegeschools(i).name());
          }
        }
        edu_count++;
      }
    }

    for(int i = 0; i < mem_ptr->highschools_size() && edu_count < SCHOOL_LIMIT; i++)  {
      if (!view_ptr->isEducationExist(mem_ptr->highschools(i).name())) {
        if(education)  {
          view_ptr->addEducation(mem_ptr->highschools(i).name());
          view_ptr->addFinalView(mem_ptr->highschools(i).name());
        }  else  {
          if(network_names.find(mem_ptr->highschools(i).name()) != network_names.end())  {
            view_ptr->addEducation(mem_ptr->highschools(i).name());
            view_ptr->addFinalView(mem_ptr->highschools(i).name());
          }
        }
        edu_count++;
      }
    }

    for(int i = 0; i < mem_ptr->juniorschools_size() && edu_count < SCHOOL_LIMIT; i++)  {
      if (!view_ptr->isEducationExist(mem_ptr->juniorschools(i).name())) {
        if(education)  {
          view_ptr->addEducation(mem_ptr->juniorschools(i).name());
          view_ptr->addFinalView(mem_ptr->juniorschools(i).name());
        }  else  {
          if(network_names.find(mem_ptr->juniorschools(i).name()) != network_names.end())  {
            view_ptr->addEducation(mem_ptr->juniorschools(i).name());
            view_ptr->addFinalView(mem_ptr->juniorschools(i).name());
          }
        }
        edu_count++;
      }
    }

    for(int i = 0; i < mem_ptr->elementaryschools_size() && edu_count < SCHOOL_LIMIT; i++)  {
      if (!view_ptr->isEducationExist(mem_ptr->elementaryschools(i).name())) {
        if(education)  { 
          view_ptr->addEducation(mem_ptr->elementaryschools(i).name());
          view_ptr->addFinalView(mem_ptr->elementaryschools(i).name());
        }  else  {
          if(network_names.find(mem_ptr->elementaryschools(i).name()) != network_names.end())  {
            view_ptr->addEducation(mem_ptr->elementaryschools(i).name());
            view_ptr->addFinalView(mem_ptr->elementaryschools(i).name());
          }
        }
        edu_count++;
      }
    }  
    if(basic_info)  {
      for(int i = 0; i < mem_ptr->regions_size() && i < REGION_LIMIT; i++)  {
        view_ptr->addRegion(mem_ptr->regions(i).city());
        view_ptr->addFinalView(mem_ptr->regions(i).city());
      }  
    }  else  {
      for(int i = 0; i < mem_ptr->regions_size() && i < REGION_LIMIT; i++)  {
        if(network_names.find(mem_ptr->regions(i).city()) != network_names.end())  {
          view_ptr->addRegion(mem_ptr->regions(i).city());
          view_ptr->addFinalView(mem_ptr->regions(i).city());
        }
      }
    }
  }
}

int SearchViewHelper::workplacePrivacy(string profile_privacy)  {
  string::size_type indexW= profile_privacy.find("w");
  string::size_type indexWait;
  int value; 
  if(indexW != string::npos)  {
    indexWait = profile_privacy.find(",", indexW);
    if(indexWait == string::npos)  {
      indexWait =profile_privacy.find("}",indexW);
    }
    value = atoi(profile_privacy.substr(indexW+2, indexWait - indexW - 2).c_str());
    return value; 
  }
  return 3;
}

int SearchViewHelper::regionPrivacy(string profile_privacy)  {
  string::size_type indexB= profile_privacy.find("b");
  string::size_type indexWait;
  int value; 
  if(indexB != string::npos)  {
    indexWait = profile_privacy.find(",", indexB);
    if(indexWait == string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }
    value = atoi(profile_privacy.substr(indexB+2, indexWait - indexB - 2).c_str());
    return value; 
  }
  return 3;
}

int SearchViewHelper::educationPrivacy(string profile_privacy)  {
  string::size_type indexB= profile_privacy.find("e");
  string::size_type indexWait;
  int value; 
  if(indexB != string::npos)  {
    indexWait = profile_privacy.find(",", indexB);
    if(indexWait == string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }
    value = atoi(profile_privacy.substr(indexB+2, indexWait - indexB - 2).c_str());
    return value; 
  }
  return 3;
}

bool SearchViewHelper::isSameNetwork(SearchCacheMemDataPtr user, SearchCacheMemDataPtr searched_result)  {
  set<int> user_networks;
  vector<int> result_networks;
  bool user_stage = false, result_stage = false;
  for(int i = 0; i < user->networks_size(); i++)  {
    if((user->networks(i)).stage() == 10)  {
      user_stage = true;
    }
    user_networks.insert((user->networks(i)).id());
  }
  for(int i = 0; i < searched_result->networks_size(); i++)  {
    if((searched_result->networks(i)).stage() == 10)  {
      result_stage = true;
    }
    result_networks.push_back((searched_result->networks(i)).id());
  }
  if(result_stage && user_stage)  {
    return true;
  }
  for(vector<int>::iterator iter = result_networks.begin(); iter != result_networks.end(); iter++)  {
    if(user_networks.find(*iter) != user_networks.end())  {
      return true;
    }
  }
  return false;
}

void SearchViewHelper::filterPrivacy(long userid, vector<long>& searchedIds)  {
  if( userid == 0 )  {//external search
    map<long, SearchCacheMemDataPtr> results = DistSearchCacheAdapter::instance().getData(searchedIds);
    for(vector<long>::iterator iter_id = searchedIds.begin(); iter_id != searchedIds.end(); iter_id++)  {
      map<long, SearchCacheMemDataPtr>::iterator iter_ptr = results.find(*iter_id);
      if(iter_ptr == results.end() || iter_ptr->first == 1)  {
        continue;
      } 
      SearchUserViewPtr user_view = new SearchUserView();
      user_view->setId((int)iter_ptr->first);
      user_view->setName((string)(iter_ptr->second)->name());
      user_view->setHeadUrl((string)(iter_ptr->second)->headurl());
      user_view->setTinyUrl((string)(iter_ptr->second)->tinyurl());
      assembleUserView(user_view, iter_ptr->second, "0", "0", "", false, false, false);
      _user_contents.push_back(user_view);
    } 
  }  else  { //internal search
    searchedIds.push_back(userid);
//    LOG(INFO) << "searchedIds.size:" << searchedIds.size();
    map<long, SearchCacheMemDataPtr> results;
    try {
      results = DistSearchCacheAdapter::instance().getData(searchedIds);
    } catch (...) {
      LOG(WARNING) << "EXCEPTION searchedIds.size:" << searchedIds.size() << "; ";
    }
    searchedIds.erase(searchedIds.end()-1);
    SearchCacheMemDataPtr search_user= results[userid];
    int result_seq = 0;
    vector<string>::iterator iter_common, iter_friend_type, iter_is_friend;
    vector<long>::iterator it;
    for(it = searchedIds.begin(), iter_common = _common_friends.begin(), iter_friend_type = _friends_type.begin(), iter_is_friend = _is_friend.begin(); it != searchedIds.end(); it++, iter_common++, iter_friend_type++, iter_is_friend++)  {
      map<long,SearchCacheMemDataPtr>::iterator iter = results.find(*it);
//      LOG(INFO) << "results.SIZE:" << results.size() << " LONG:" << iter->first;
      if(iter == results.end() || iter->first == 1){
        continue;
      }
      if(isAdmin(*it) || userid == *it)  {//TODO : is admin
        SearchUserViewPtr user_view = new SearchUserView();
        user_view->setId((int)iter->first);
        user_view->setName((string)(iter->second)->name());
        user_view->setHeadUrl((string)(iter->second)->headurl());
        user_view->setTinyUrl((string)(iter->second)->tinyurl());
        assembleUserView(user_view, iter->second, *iter_is_friend, *iter_friend_type, *iter_common, true, true, true);
        _user_contents.push_back(user_view);
      }  else  {
        bool basic_privacy = false, workplaces_privacy = false, edu_privacy = false;
        string profileprivacy = (iter->second)->profileprivacy();
        int basic_value = regionPrivacy(profileprivacy); 
        int workplaces_value = workplacePrivacy(profileprivacy);
        int edu_value = educationPrivacy(profileprivacy);
        if(workplaces_value == VIEW_PERMIT_LEVEL4)  {
          workplaces_privacy = true;
        } else if(workplaces_value == VIEW_PERMIT_LEVEL3) {
          if(_is_friend.at(result_seq) == "1"  || isSameNetwork( search_user, iter->second))  {
            workplaces_privacy = true;
          } else  {
            workplaces_privacy = false;
          }
        } else if(workplaces_value == VIEW_PERMIT_LEVEL2)  {
          if(isSameNetwork( search_user, iter->second))  {
            workplaces_privacy = true;
          } else  {
            workplaces_privacy = false;
          } 
        }  else if(workplaces_value == VIEW_PERMIT_LEVEL1)  {
          if(_is_friend.at(result_seq) == "1")  {
            workplaces_privacy = true;
          }  else  {
            workplaces_privacy = false;
          }
        } 
        if(edu_value == VIEW_PERMIT_LEVEL4)  {
          edu_privacy = true;
        } else if (edu_value == VIEW_PERMIT_LEVEL3) {
          if(_is_friend.at(result_seq) == "1"  || isSameNetwork( search_user, iter->second))  {
            edu_privacy = true;
          } else  {
            edu_privacy = false;
          }
        } else if(edu_value == VIEW_PERMIT_LEVEL2)  {
          if(isSameNetwork( search_user, iter->second))  {
            edu_privacy = true;
          } else  {
            edu_privacy = false;
          } 
        }  else if(edu_value == VIEW_PERMIT_LEVEL1)  {
          if(_is_friend.at(result_seq) == "1")  {
            edu_privacy = true;
          }  else  {
            edu_privacy = false;
          }
        }

        if(basic_value == VIEW_PERMIT_LEVEL4)  {
          basic_privacy = true;
        } else if (basic_value == VIEW_PERMIT_LEVEL3) {
          if(_is_friend.at(result_seq) == "1"  || isSameNetwork( search_user, iter->second))  {
            basic_privacy = true;
          } else  {
            basic_privacy = false;
          }
        } else if(basic_value == VIEW_PERMIT_LEVEL2)  {
          if(isSameNetwork( search_user, iter->second))  {
            basic_privacy = true;
          } else  {
            basic_privacy = false;
          } 
        }  else if(basic_value == VIEW_PERMIT_LEVEL1)  {
          if(_is_friend.at(result_seq) == "1")  {
            basic_privacy = true;
          }  else  {
            basic_privacy = false;
          }
        }

        SearchUserViewPtr user_view = new SearchUserView();
        user_view->setId((int)iter->first);
        user_view->setName((string)(iter->second)->name());
        user_view->setHeadUrl((string)(iter->second)->headurl());
        assembleUserView(user_view, iter->second, *iter_is_friend, *iter_friend_type, *iter_common, basic_privacy, edu_privacy, workplaces_privacy);
        _user_contents.push_back(user_view);      
      }
      result_seq++;
    }      
  }
}


