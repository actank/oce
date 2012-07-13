#include "action.h"
#include "base/logging.h"
#include <boost/lexical_cast.hpp>
#include "UtilCxx/src/Allocator.h"

using namespace xce::search;
using namespace std;
using namespace com::xiaonei::search2::model;
using namespace xce::searchcache::dboperation;
using namespace com::renren::search2::friendoffriend;
using namespace com::xiaonei::platform::storage;
using namespace passport;
using namespace xce::parseparam;
using namespace login;
using namespace com::xiaonei::ozein::content::generation;
using namespace MyUtil;

std::string ResultAction::SetNoResult(int host, const string& query, const string& param) {
  string out;
  string tpl_name;
  if (host > 0) {
    tpl_name = "tpl/noresult.tpl";
  } else {
    tpl_name = "tpl/noresultexternal.tpl";
  }
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
  if (!tpl) {
    LOG(LERROR) << "can not load tpl:" << tpl_name;
    return out;
  }
  ctemplate::TemplateDictionary dict("noresult");

//  ctemplate::TemplateDictionary* tabdict = dict.AddIncludeDictionary("TAB");
//  tabdict->SetFilename("tpl/tab.tpl");
//  tabdict->SetValue("SELECT_ALL", "select");
  dict.SetIntValue("HOST", host);
  dict.SetValue("SEARCH_EX_Q", query);
  dict.SetValue("ANTISPAM_RST", "empty");
  if (param == "") {
    dict.SetValue("SEARCH_EX_P", "[]");
  } else {
    dict.SetValue("SEARCH_EX_P", param);
  }
  tpl->Expand(&out, &dict);
  return out;
}

std::string ResultAction::SetAntispamResult(int host, const string& query, const string& param) {
  string out;
  string tpl_name;
  if (host > 0) {
    tpl_name = "tpl/antispamresult.tpl";
  } else {
    tpl_name = "tpl/noresultexternal.tpl";
  }
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
  if (!tpl) {
    LOG(LERROR) << "can not load tpl:" << tpl_name;
    return out;
  }
  ctemplate::TemplateDictionary dict("antispamresult");

//  ctemplate::TemplateDictionary* tabdict = dict.AddIncludeDictionary("TAB");
//  tabdict->SetFilename("tpl/tab.tpl");
//  tabdict->SetValue("SELECT_ALL", "select");
  dict.SetValue("SEARCH_EX_Q", query);
  dict.SetValue("ANTISPAM_RST", "spam");
  if (param == "") {
    dict.SetValue("SEARCH_EX_P", "[]");
  } else {
    dict.SetValue("SEARCH_EX_P", param);
  }
  tpl->Expand(&out, &dict);
  return out;
}

void ResultAction::SetUserResult(int hostId, ctemplate::TemplateDictionary& dict, const SearchUserViewPtr searchView, int totalCount, int index, const string& query, const string& param){
  ctemplate::TemplateDictionary* userdict = dict.AddIncludeDictionary("USER_RESULT");
  userdict->SetFilename("tpl/userresult.tpl");
  int UserId = searchView->getId();
  string friendType = "", commonNum = "" ;
  friendType = searchView->getFriendType();
  commonNum = searchView->getCommonFriendsNum();
 
  userdict->SetIntValue("ID", UserId);
  userdict->SetIntValue("INDEX", index);
  userdict->SetValue("HEAD_IMG", HeadUrlUtil::instance().getHeadFullUrl(searchView->getHeadUrl()));
  userdict->SetValue("FRIEND_TYPE", friendType);
  userdict->SetValue("COMMON_NUM", commonNum);
  userdict->SetIntValue("TOTAL_COUNT", totalCount);
  userdict->SetValue("NAME", searchView->getName());
  userdict->SetValue("QUERY", query);
  if (searchView->isSelected()) {
    ctemplate::TemplateDictionary* stardict = userdict->AddIncludeDictionary("STAR");
    stardict->SetFilename("tpl/star.tpl");
  }
  if (searchView->isVipMember()) {
    ctemplate::TemplateDictionary* vipdict = userdict->AddIncludeDictionary("VIP");
    vipdict->SetFilename("tpl/vip.tpl");
  }

  vector<string> userInfo = searchView->getFinalView();
  int userInfoSize = userInfo.size();
  if (userInfoSize > 0) {
    ctemplate::TemplateDictionary* userinfodict = userdict->AddIncludeDictionary("USER_INFO");
    userinfodict->SetFilename("tpl/userinfo.tpl");
//    for (set<string>::iterator it = userInfo.begin(); it != userInfo.end(); it++) {
//      ctemplate::TemplateDictionary* usernetdict = userinfodict->AddSectionDictionary("USER_NET");
//      usernetdict->ShowSection("USER_NET");
//      usernetdict->SetValue("SHOW_NET", *it);
//    }
    for (int i = 0; i < userInfoSize; i++) {
      ctemplate::TemplateDictionary* usernetdict = userinfodict->AddSectionDictionary("USER_NET");
      usernetdict->ShowSection("USER_NET");
      usernetdict->SetValue("SHOW_NET", userInfo[i]);
    }
  }

  if (commonNum != "") {
    ctemplate::TemplateDictionary* commonfriendsdict = userdict->AddIncludeDictionary("COMMON_FRIENDS");
    commonfriendsdict->SetFilename("tpl/commonfriends.tpl");
    commonfriendsdict->SetValue("COMMON_NUM", commonNum);
  }

  if (hostId == 0) {//站外用户显示
    ctemplate::TemplateDictionary* senddict = userdict->AddIncludeDictionary("SEND_MSG");
    senddict->SetFilename("tpl/sendmsg.tpl");
    senddict->SetIntValue("ID", UserId);
    senddict->SetValue("QUERY", query);
    senddict->SetValue("FRIEND_TYPE", friendType);
    senddict->SetValue("COMMON_NUM", commonNum);
    senddict->SetIntValue("TOTAL_COUNT", totalCount);
    senddict->SetIntValue("INDEX", index);
  } else {
    if (friendType == "1") { 
      ctemplate::TemplateDictionary* writecommentdict = userdict->AddIncludeDictionary("WRITE_COMMENT");
      writecommentdict->SetFilename("tpl/writecomment.tpl");
      writecommentdict->SetIntValue("ID", UserId);
      writecommentdict->SetValue("QUERY", query);
      writecommentdict->SetValue("PARAM", param);
      writecommentdict->SetValue("FRIEND_TYPE", friendType);
      writecommentdict->SetValue("COMMON_NUM", commonNum);
      writecommentdict->SetIntValue("TOTAL_COUNT", totalCount);
      writecommentdict->SetIntValue("INDEX", index);
      if (searchView->getGender() == "女生") {
        writecommentdict->SetValue("TA", "她");
      } else {
        writecommentdict->SetValue("TA", "他");
      }
    } else {
      ctemplate::TemplateDictionary* addsenddict = userdict->AddIncludeDictionary("ADD_SEND");
      addsenddict->SetFilename("tpl/addsend.tpl");
      addsenddict->SetIntValue("ID", UserId);
      addsenddict->SetValue("QUERY", query);
      addsenddict->SetValue("TINY_IMG", HeadUrlUtil::instance().getHeadFullUrl(searchView->getTinyUrl()));
      addsenddict->SetValue("FRIEND_TYPE", friendType);
      addsenddict->SetValue("COMMON_NUM", commonNum);
      addsenddict->SetIntValue("TOTAL_COUNT", totalCount);
      addsenddict->SetIntValue("INDEX", index);
    }
  }
}

void ResultAction::SetPageResult(int hostId, ctemplate::TemplateDictionary& dict, const SearchPageViewPtr searchView, int totalCount, int index, const string& query){
  string pageName = "", subType = "", friendType = "", commonNum = "", headUrl = "";
  ctemplate::TemplateDictionary* pagedict;
  int ptype = searchView->getPType();

  if (ptype == 20){//俱乐部
    pagedict = dict.AddIncludeDictionary("CLUB_RESULT");
    pagedict->SetFilename("tpl/clubresult.tpl");
  } else {
    pagedict = dict.AddIncludeDictionary("PAGE_RESULT");
    pagedict->SetFilename("tpl/pageresult.tpl");
  }

  int pageId = searchView->getId();
  pageName = searchView->getName();
  subType = searchView->getSubType();
  int fansCount = searchView->getFansCount();
  headUrl = searchView->getHeadUrl();

  pagedict->SetIntValue("ID", pageId);
  pagedict->SetIntValue("INDEX", index);
  pagedict->SetValue("HEAD_IMG", HeadUrlUtil::instance().getHeadFullUrl(headUrl));
  pagedict->SetValue("FRIEND_TYPE", friendType);
  pagedict->SetValue("COMMON_NUM", commonNum);
  pagedict->SetIntValue("TOTAL_COUNT", totalCount);
  pagedict->SetValue("NAME", pageName);
  pagedict->SetValue("QUERY", query);
  pagedict->SetValue("PAGE_TYPE", subType);
  pagedict->SetIntValue("FANS_COUNT", fansCount);
  ctemplate::TemplateDictionary* pagehomedict = pagedict->AddIncludeDictionary("PAGE_HOME");
  pagehomedict->SetFilename("tpl/pagehome.tpl");
  pagehomedict->SetIntValue("ID", pageId);
  pagehomedict->SetIntValue("INDEX", index);
  if (ptype == 20) {
    pagehomedict->SetValue("PTYPE", "org");
    pagehomedict->SetValue("HOME_WORD", "查看俱乐部");
  } else {
    pagehomedict->SetValue("PTYPE", "page");
    pagehomedict->SetValue("HOME_WORD", "查看主页");
  }
  if (hostId > 0) {
    ctemplate::TemplateDictionary* joinfansdict = pagedict->AddIncludeDictionary("JOIN_FANS");
    joinfansdict->SetFilename("tpl/joinfans.tpl");
    joinfansdict->SetIntValue("ID", pageId);
    if (ptype == 20) {
      joinfansdict->SetValue("FANS_WORD", "申请加入");
    } else {
      joinfansdict->SetValue("FANS_WORD", "成为粉丝");
    }
  }
}

void ResultAction::SetAppResult(ctemplate::TemplateDictionary& dict, const SearchAppViewPtr searchView, int totalCount, int index, const string& query){
  ctemplate::TemplateDictionary* appdict = dict.AddIncludeDictionary("APP_RESULT");
  appdict->SetFilename("tpl/appresult.tpl");

  string appName = "", company = "", description = "", friendType = "", commonNum = "", logo = "", sideNavUrl = "";
  stringstream sideNavUrlFixed;
  int appId = searchView->getId();
  appName = searchView->getName();
  company = searchView->getCompany();
  description = searchView->getDescription();
  logo = searchView->getLogo();
  sideNavUrl = searchView->getSideNavUrl();
  if (strstr(sideNavUrl.c_str(), QuestionMark.c_str())) {
    sideNavUrlFixed << sideNavUrl << "&";
//    LOG(INFO) << "sideNavUrl:" << sideNavUrl << " contains " << QuestionMark << "; after fixed " << sideNavUrlFixed.str();
  } else {
    sideNavUrlFixed << sideNavUrl << "?";
//    LOG(INFO) << "sideNavUrl:" << sideNavUrl << " does not contains " << QuestionMark << "; after fixed " << sideNavUrlFixed.str();
  }
  
  appdict->SetIntValue("ID", appId);
  appdict->SetIntValue("INDEX", index);
  appdict->SetValue("HEAD_IMG", HeadUrlUtil::instance().getHeadFullUrl(logo));
  appdict->SetValue("FRIEND_TYPE", friendType);
  appdict->SetValue("COMMON_NUM", commonNum);
  appdict->SetIntValue("TOTAL_COUNT", totalCount);
  appdict->SetValue("NAME", appName);
  appdict->SetValue("QUERY", query);
  appdict->SetValue("COMPANY", company);
  appdict->SetValue("DESCRIPTION", description);
  appdict->SetValue("SIDE_NAV_URL", sideNavUrlFixed.str());
}

map<string, string> ResultAction::getOpUser(SearchCacheMemDataPtr data) {
  map<string, string> opUsers;
  opUsers["GENERAL|name"] = data->name();
  opUsers["GENERAL|gender"] = data->gender();
  opUsers["GENERAL|user_basic.stage"] = data->stage();

  int univSize = data->universityschools_size();
  if (univSize > 0) {
    for (int i = 0; i < univSize; i++) {
      oce::searchcache::UniversityInfo* info = data->mutable_universityschools(i);
      if (info == NULL) {
        continue;
      }
      stringstream key_year, key_dep, key_name, key_id;
      key_year << "UNIVERSITY_INFO|" << i << "|enroll_year";
      key_dep << "UNIVERSITY_INFO|" << i << "|department";
      key_name << "UNIVERSITY_INFO|" << i << "|university_name";
      key_id << "UNIVERSITY_INFO|" << i << "|university_id";
      opUsers[key_year.str()] = int2String(info->year());
      opUsers[key_dep.str()] = info->department();
      opUsers[key_name.str()] = info->name();
      opUsers[key_id.str()] = int2String(info->id());
    }
  }
  int highSize = data->highschools_size();
  if (highSize > 0) {
    for (int i = 0; i < highSize; i++) {
      oce::searchcache::HighSchoolInfo* info = data->mutable_highschools(i);
      if (info == NULL) {
        continue;
      }
      stringstream key_year, key_name, key_id, key_c1, key_c2, key_c3;
      key_year << "HIGH_SCHOOL_INFO|" << i << "|enroll_year";
      key_name << "HIGH_SCHOOL_INFO|" << i << "|high_school_name";
      key_id << "HIGH_SCHOOL_INFO|" << i << "|high_school_id";
      key_c1 << "HIGH_SCHOOL_INFO|" << i << "|h_class1";
      key_c2 << "HIGH_SCHOOL_INFO|" << i << "|h_class2";
      key_c3 << "HIGH_SCHOOL_INFO|" << i << "|h_class3";
      opUsers[key_year.str()] = int2String(info->year());
      opUsers[key_name.str()] = info->name();
      opUsers[key_id.str()] = int2String(info->id());
      opUsers[key_c1.str()] = info->class1();
      opUsers[key_c2.str()] = info->class2();
      opUsers[key_c3.str()] = info->class3();
    }
  }
  
  int workSize = data->workplaces_size();
  if (workSize > 0) {
    for (int i = 0; i < workSize; i++) {
      oce::searchcache::WorkplaceInfo* info = data->mutable_workplaces(i);
      if (info == NULL) {
        continue;
      }
      stringstream key_name, key_id;
      key_name << "WORKPLACE_INFO|" << i << "|workplace_name";
      key_id << "WORKPLACE_INFO|" << i << "|workplace_id";
      opUsers[key_name.str()] = info->name();
      opUsers[key_id.str()] = int2String(info->id());
    }
  }
  
  int regionSize = data->regions_size();
  if (regionSize > 0) {
    for (int i = 0; i < regionSize; i++) {
      oce::searchcache::RegionInfo* info = data->mutable_regions(i);
      if (info == NULL) {
        continue;
      }
      stringstream key_id, key_prov, key_city;
      key_id << "REGION_INFO|" << i << "|region_id";
      key_prov << "REGION_INFO|" << i << "|province_name";
      key_city << "REGION_INFO|" << i << "|city_name";
      opUsers[key_id.str()] = int2String(info->id());
      opUsers[key_prov.str()] = info->province();
      opUsers[key_city.str()] = info->city();
    }
  }
  int esSize = data->elementaryschools_size();
  if (esSize > 0) {
    for (int i = 0; i < esSize; i++) {
      oce::searchcache::ElementarySchoolInfo* info = data->mutable_elementaryschools(i);
      if (info == NULL) {
        continue;
      }
      stringstream key_year, key_name, key_id;
      key_year << "ES_INFO|" << i << "|elementary_school_info.elementary_school_year";
      key_name << "ES_INFO|" << i << "|elementary_school_info.elementary_school_name";
      key_id << "ES_INFO|" << i << "|elementary_school_info.elementary_school_id";
      opUsers[key_year.str()] = int2String(info->year());
      opUsers[key_name.str()] = info->name();
      opUsers[key_id.str()] = int2String(info->id());
    }
  }
  int jsSize = data->juniorschools_size();
  if (jsSize > 0) {
    for (int i = 0; i < jsSize; i++) {
      oce::searchcache::JuniorSchoolInfo* info = data->mutable_juniorschools(i);
      if (info == NULL) {
        continue;
      }
      stringstream key_year, key_name, key_id;
      key_year << "JS_INFO|" << i << "|junior_high_school_info.junior_high_school_year";
      key_name << "JS_INFO|" << i << "|junior_high_school_info.junior_high_school_name";
      key_id << "JS_INFO|" << i << "|junior_high_school_info.junior_high_school_id";
      opUsers[key_year.str()] = int2String(info->year());
      opUsers[key_name.str()] = info->name();
      opUsers[key_id.str()] = int2String(info->id());
    }
  }
  return opUsers;
}

IndexResultPtr ResultAction::searchIndex(int userId, int offset, const string& query, const string& param, const string& scope, SearchCacheMemDataPtr data) {
  IndexCondition condition;
  IndexResultPtr result = new IndexResult;
  condition.type = OpenPeopleSearch;
  condition.queryCondition["query"] = query;
  if (param != "") {
    ParseParam parseParamAdapter;
    map<string, string> paramMap = parseParamAdapter.json2map(param);
    for (map<string, string>::iterator it = paramMap.begin(); it != paramMap.end(); it++) {
      condition.queryCondition[it->first] = it->second;
    }
  }

  condition.opUserCondition["GENERAL|user_basic.id"] = int2String(userId);
  if (userId > 0) {
    map<string, string> opUsers = getOpUser(data);
    for (map<string, string>::iterator it = opUsers.begin(); it != opUsers.end(); it++) {
      condition.opUserCondition[it->first] = it->second;
    }
  }

  try {
    if (scope == "") {
      long openSearchStart = get_now();
      if (userId == 0) {
        LOG(INFO) << "USERID:" << userId << " call ExternalSearchManager!";
        result = ExternalSearchManagerAdapter::instance().SearchIndex(condition, offset, ItemPerPage);
      } else {
        LOG(INFO) << "USERID:" << userId << " call SearchManager!";
        result = SearchManagerAdapter::instance().SearchIndex(condition, offset, ItemPerPage);
      }
      long openSearchEnd = get_now();
      LOG(INFO) << "TIMECOST scope:" << scope << ", openSearchCost:" << openSearchEnd - openSearchStart << "ms";
    } else if (scope == "0") {
      long peopleSearchStart = get_now();
      if (userId == 0) {
        LOG(INFO) << "USERID:" << userId << " call ExternalPeopleSearchManager!";
        result = ExternalPeopleSearchManagerAdapter::instance().SearchIndex(condition, offset, ItemPerPage);
      } else {
        LOG(INFO) << "USERID:" << userId << " call PeopleSearchManager!";
        result = PeopleSearchManagerAdapter::instance().SearchIndex(condition, offset, ItemPerPage);
      }
      long peopleSearchEnd = get_now();
      LOG(INFO) << "TIMECOST scope:" << scope << ", peopleSearchCost:" << peopleSearchEnd - peopleSearchStart << "ms";
    } else if (scope == "2") {
      long pageSearchStart = get_now();
      result = PageSearchManagerAdapter::instance().SearchIndex(condition, offset, ItemPerPage);
      long pageSearchEnd = get_now();
      LOG(INFO) << "TIMECOST scope:" << scope << ", pageSearchCost:" << pageSearchEnd - pageSearchStart << "ms";
    } else if (scope == "3") {
      long appSearchStart = get_now();
      result = AppSearchManagerAdapter::instance().SearchIndex(condition, offset, ItemPerPage);
      long appSearchEnd = get_now();
      LOG(INFO) << "TIMECOST scope:" << scope << ", appSearchCost:" << appSearchEnd - appSearchStart << "ms";
    }
  } catch (Ice::Exception& ex) {
    LOG(LERROR) << "Ice::Exception->" << ex.what();
    result->totalCount = 0;
  } catch (...) {
    LOG(LERROR) << "Something wrong in search";
    result->totalCount = 0;
  }
  return result;
}

void ResultAction::searchId(const string& query, IndexResultPtr result) {
  long queryId = boost::lexical_cast<long>(query);
  if (queryId > INT_MAX) {
    result->totalCount = 0;
  } else {
    IndexResultItem item;
    item.id = queryId;
    result->totalCount = 1;
    Type2IntMap typeCount;
    if (queryId >= 600000000 && queryId < 700000000) {
      item.type = Page;
      typeCount[Page] = 1;
    } else if (queryId > 0 && (queryId < 600000000 || queryId > 700000000)) {
      item.type = User;
      typeCount[User] = 1;
    }
    result->typedCount = typeCount;
    (result->contents).push_back(item);
  }
}

void ResultAction::searchEmail(const string& query, IndexResultPtr result) {
  vector<AccountPtr> account;
  try {
    account = WebLoginLogicAdapter::instance().getAccount(query);
  } catch (Ice::Exception& e) {
    LOG(LERROR) << "Ice::Exception->" << e.what();
  } catch (...) {
    LOG(LERROR) << "Something wrong in calling WebLoginLogicAdapter";
  }
  size_t accountSize = account.size();
  if (accountSize > 0) {
    Type2IntMap typeCount;
    int acountNum = 0;
    for (size_t i = 0; i < accountSize; i++) {
      int acountId = account[i]->id();
      if (acountId > 0) {
        IndexResultItem item;
        item.id = acountId;
        item.type = User;
        (result->contents).push_back(item);
        acountNum++;
      }
    }
    typeCount[User] = acountNum;
    result->totalCount = acountNum;
    result->typedCount = typeCount;
  } else {
    result->totalCount = 0;
  }
}

bool ResultAction::passAntispam(int userId, const string& query, const string& param, const string& remoteAddr) {
  int antispamId;
  vector<string> checkContent;
  if (param != "") {
    checkContent.push_back(param);
  }
  if (query != "") {
    checkContent.push_back(query);
  }
  if (userId == 0) {
    antispamId = hashCode(remoteAddr);
  } else {
    antispamId = userId;
  }
  LOG(INFO) << "AAAAAAAAAAAAAAntispamID:" << antispamId;
  try {
   CheckResultMore checkResult = OzeinFilterClient::instance().checkerAndFilter(antispamId, 0, AntispamType, checkContent, remoteAddr);
   if (checkResult.getFlag() != 0) {
     LOG(INFO) << "AAAAAAAAAAAAAntispam FLAG:" << checkResult.getFlag();
     return false;
   }
  }catch (Ice::Exception& e) {
    LOG(LERROR) << "Ice::Exception->" << e.what();
    return false;
  } catch (...)  {
    LOG(LERROR) << "Something wrong in calling checkerAndFilter";
    return false;
  }
  return true;
}

cwf::HttpStatusCode ResultAction::Process(cwf::Request* request, cwf::Response* response) { 
  LOG(INFO) << "ResultAction Call";
  long start, end, last;
  start = get_now();
  string out, tinyUrl, headUrl, userName, userGender, tpl_name;
  int userId, offset, count = 0, totalCount, antispamId;
  bool enableBlogSearch = false;
  IndexResultPtr result = new IndexResult;

  MyUtil::IntSeq friendLists;
  map<long, SearchCacheMemDataPtr> showInfos;

  string remoteAddr = request->header("REMOTE_ADDR");
  if (remoteAddr == "") {
    remoteAddr = "0";
  }
  LOG(INFO) << "REMOTE_ADDR:" << remoteAddr; 
  string query = request->form("q");
//  LOG(INFO) << "query:" << query;
  string scope = request->form("s");
//  LOG(INFO) << "scope:" << scope;
  string param = request->form("p");
//  LOG(INFO) << "param:" << param;
  string offset_str = request->form("offset");
  if (offset_str == "") {
    offset_str = "0";
  }

  string ajax = request->form("ajax");
//  LOG(INFO) << "ajax:" << ajax;

  string ticket = request->cookie().Get("t");
//  LOG(INFO) << "Ticket:" << ticket;
  if (ticket == "") {
    userId = 0;
  } else {
    try {
      userId = TicketAdapter::instance().verifyTicket(ticket, "");
    } catch (Ice::Exception& e) {
      LOG(LERROR) << "Ice::Exception->" << e.what();
      userId = 0;
    } catch (...)  {
      LOG(LERROR) << "Something wrong in calling TicketAdapter";
      userId = 0;
    }
  }
    
  if (userId == -1) {
    userId = 0;
  }

  offset= boost::lexical_cast<int>(offset_str);

  LOG(INFO) << "userId:" << userId << " offset:" << offset << " scope:" << scope << " query:" << query;
  end = get_now();
  long getParamCost = end - start;
  last = end;

  if (!passAntispam(userId, query, param, remoteAddr)) {
    LOG(INFO) << "Antispam pass";
    out = SetAntispamResult(userId, query, param);
    SetHeader(response);
    response->WriteRaw(out);
    return cwf::HC_OK;
  }
  
  end = get_now();
  long passAntispamCost = end - last;
  last = end;

  SearchCacheMemDataPtr data = new SearchCacheMemData;
  if (userId > 0) {
    try {
      data = xce::dist::searchcache::DistSearchCacheAdapter::instance().getData((long)userId);
    } catch (exception& ex) {
      LOG(WARNING) << "get showInfo wrong: id:" << userId << " " << ex.what();
    }
  }

  if (checkId(query)) { //query是id
    searchId(query,result);
  } else if (checkEmail(query)) {//query是email
    searchEmail(query,result);
  } else {
    result = searchIndex(userId,offset,query,param,scope,data);
  }

//  vector<IndexResultItem> items = result->contents;
//  stringstream ids_str;
//  for (size_t i = 0; i < items.size(); i++) {
//    ids_str << i << ":"<< (items[i]).id << ";";
//  }
//  LOG(INFO) << "ACTION:ids.size:" << items.size() << " ; ids:" << ids_str.str();

  end = get_now();
  long searchCost = end - last;
  last = end;

  SearchViewHelper viewHelper(userId);
  SearchResultPtr searchResult = viewHelper.searchViewFilter(result);
  end = get_now();
  long getSearchResultCost = end - last;
  last = end;

  try {
    totalCount = searchResult->getTotalCount();
    LOG(INFO) << "TOTALCOUNT:" << totalCount;
    if (totalCount > 0) {
      long dictCommonStart = get_now();
      tpl_name = "tpl/results.tpl";
      ctemplate::Template* tpl = ctemplate::Template::GetTemplate(tpl_name, ctemplate::STRIP_WHITESPACE);
      if (!tpl) {
        LOG(LERROR) << "can not load tpl:" << tpl_name;
        SetHeader(response);
        response->WriteRaw(out);
        return cwf::HC_OK;
      }
      ctemplate::TemplateDictionary dict("result");

      Type2IntMap typedCount = searchResult->getTypedCount();
      for (Type2IntMap::iterator itTC = typedCount.begin(); itTC != typedCount.end(); itTC++) {
        int countNum = itTC->second;
        if (itTC->first == User && countNum > 0) {
          dict.SetIntValue("USER_NUM", countNum > 500 ? 500 : countNum);
        } else if (itTC->first == Page && countNum > 0) {
          dict.SetIntValue("PAGE_NUM", countNum > 500 ? 500 : countNum);
        } else if (itTC->first == App && countNum > 0) {
          dict.SetIntValue("APP_NUM", countNum > 500 ? 500 : countNum);
        }
      }
      if (userId == 0) {
        dict.SetValue("OUTER", "1");
      } else {
        dict.SetValue("OUTER", "");
      }
      dict.SetIntValue("RESULT_NUM", totalCount > 500 ? 500 : totalCount);
      dict.SetValue("SEARCH_EX_S", scope);
      dict.SetValue("ANTISPAM_RST", "ok");
      if (param == "") {
        dict.SetValue("SEARCH_EX_P", "[]");
      } else {
        dict.SetValue("SEARCH_EX_P", param);
      }
      dict.SetValue("SEARCH_EX_Q", query);
      long dictCommonEnd = get_now();
      LOG(INFO) << "TIMECOST dictCommonCost:" << dictCommonEnd - dictCommonStart << "ms";
      vector<SearchViewPtr> contents = searchResult->getContents();
      SearchViewPtr searchView;
      for (size_t i = 0; i < contents.size(); i++) {
        searchView = contents[i];
        IndexType type = searchView->getIndexType();
        if (type == User) {
          long setUserResultStart = get_now();
          SetUserResult(userId, dict, SearchUserViewPtr::dynamicCast(searchView), totalCount, offset + count, query, param);
          long setuserResultEnd = get_now();
          //          LOG(INFO) << count << " TIMECOST setUserResultCost:" << setuserResultEnd - setUserResultStart << "ms";
        } else if (type == Page) {
          long setPageResultStart = get_now();
          SetPageResult(userId, dict, SearchPageViewPtr::dynamicCast(searchView), totalCount, offset + count, query);
          long setPageResultEnd = get_now();
          //          LOG(INFO) << count << " TIMECOST setPageResultCost:" << setPageResultEnd - setPageResultStart << "ms";
        } else if (type == App) {
          long setAppResultStart = get_now();
          SetAppResult(dict, SearchAppViewPtr::dynamicCast(searchView), totalCount, offset + count, query);
          long setAppResultEnd = get_now();
          //          LOG(INFO) << count << " TIMECOST setAppResultCost:" << setAppResultEnd - setAppResultStart << "ms";
        } 
        count++;
      }
      long dictEnd = get_now();
      LOG(INFO) << "TIMECOST dictUPACost:" << dictEnd - dictCommonEnd << "ms";
      tpl->Expand(&out, &dict);
    } else {
      LOG(INFO) << "NORESULT";
      out = SetNoResult(userId, query, param);
    }
  } catch (exception& ex) {
    LOG(WARNING) << ex.what();
  }
  SetHeader(response);
  response->WriteRaw(out);
  end = get_now();
  long expandHtmlCost = end -last;
  long allCost = end - start;

  LOG(INFO) << "TIMECOST getParamCost:" << getParamCost << "ms; passAntispamCost:" << passAntispamCost << "ms; searchCost:" << searchCost << "ms; getSearchResultCost:" << getSearchResultCost << "ms; expandHtmlCost:" << expandHtmlCost << "ms; allCost:" << allCost << "ms";
  return cwf::HC_OK;
}
