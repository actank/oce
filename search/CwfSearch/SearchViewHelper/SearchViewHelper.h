/*
 * =====================================================================================
 *
 *       Filename:  UserView.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/16/2011 07:07:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef _SEARCHVIEWHELPER__H_
#define _SEARCHVIEWHELPER__H_
#include <string>
#include <vector>
#include <set>
#include "IceExt/src/ServiceI.h"
#include <SearchModel.h>
#include <IceUtil/IceUtil.h>
#include <UserBase.h>
#include "search/SearchCache2/src/DistSearchCacheLogic/DistSearchCacheAdapter.h"
//#include "OceCxxAdapter/src/DistSearchCacheAdapter.h"
#include "OceCxxAdapter/src/UserDangerAdapter.h"
#include <sys/time.h>

using namespace std;
using namespace com::xiaonei::search2::model;
using namespace xce::dist::searchcache;
using namespace xce::searchcache::dboperation;
using namespace xce::userdanger;

const int WORK_PLACE_LIMIT = 2;
const int SCHOOL_LIMIT = 3;
const int REGION_LIMIT = 1;
const string IS_FRIEND = "is_friend";
const string FRIEND_TYPE = "FriendsType";
const string COMMON_FRIENDS_NUM = "CommonFriendsNum";
const int CStateSELECTED = 0;
const int CStateVIPMEMBER = 1;
const int CStateMSNIMME = 2;
const int CStateMOBILEBIND = 3;

const int VIEW_PERMIT_LEVEL0 = -1; //自己可见
const int VIEW_PERMIT_LEVEL1 = 0;  //好友可见
const int VIEW_PERMIT_LEVEL2 = 1;  //同城可见
const int VIEW_PERMIT_LEVEL3 = 3;  //好友及同网络可见
const int VIEW_PERMIT_LEVEL4 = 99;  //所有人可见

const string HEADURL = "headUrl";
const string TINYURL = "tinyUrl";
const string PAGE_NAME = "name";
const string PAGE_PTYPE = "p_type";
const string PAGE_FANS_COUNT = "fans_count";
const string PAGE_SUB_TYPE = "sub_type";
const string APP_ID = "app_id";
const string APP_NAME = "app_name";
const string APP_DESCRIPTION = "app_short_desc";
const string APP_COMPANY = "company_name";
const string APP_LOGO = "logo";
const string APP_SIDE_NAV_URL = "side_nav_url"; 

class SearchView : public IceUtil::Shared {
  private:
    int _id; //ID 
    string _name; //名称
    string _headurl; //头像
    string _tinyurl;
    IndexType _indextype;
  public:
    SearchView() {}
    virtual ~SearchView() {}
    virtual void setId(int id);
    virtual void setName(string name);
    virtual void setHeadUrl(string headurl);
    virtual void setTinyUrl(string tinyurl);
    virtual void setIndexType(IndexType type);
    virtual int getId();
    virtual string getName();
    virtual string getHeadUrl();
    virtual string getTinyUrl();
    virtual IndexType getIndexType();
};
typedef IceUtil::Handle<SearchView> SearchViewPtr;

class SearchUserView : public SearchView {
  public:
    SearchUserView() {}
    ~SearchUserView() {}
    bool isFriend();
    bool isSelected(); 
    bool isVipMember();
    bool isMSNIMME();
    bool isMobileBind(); 
    void setState(int state);
    void setStatus(int status); 
    void setCommonFriendsNum(string common_frineds_num); 
    void setFriendType(string friend_type);
    void setIsFriend(string is_friend);
    void setGender(string gender);
    void addNetwork(string network_name);
    void addEducation(string school_name);
    void addWorkplaces(string workplace_name);
    void addRegion(string region_name);
    void addFinalView(string final_name); 

    bool isWorkplacesExist(string workplace_name);
    bool isEducationExist(string school_name);
    bool isRegionExist(string region_name);

    vector<string> getFinalView(); 

    int getStatus();
    string getCommonFriendsNum();
    string getFriendType(); 
    string getGender();

  private:
    bool _is_star;
    string _is_friend;
    bool _is_vip;
    string _common_friends_num;
    int _state;
    int _status; 
    string _gender; 
    string _friend_type;
    vector<string> _network; 
    vector<string> _education;
    vector<string> _region;
    vector<string> _workplaces;
    vector<string> _final_view;
};
typedef IceUtil::Handle<SearchUserView> SearchUserViewPtr;

class SearchAppView : virtual public SearchView {
  private:
    string _description;  //描述
    string _company; //开发者
    string _side_nav_url;//应用终端页    
    string _logo;

  public:
    SearchAppView() {}
    ~SearchAppView() {}
    string getDescription();
    string getCompany();
    string getSideNavUrl(); 
    string getLogo(); 
    void setDescription(string description);
    void setCompany(string company);
    void setSideNavUrl(string side_nav_url);
    void setLogo(string logo);



};
typedef IceUtil::Handle<SearchAppView> SearchAppViewPtr;

class SearchPageView : virtual public SearchView  {
  private:
    int _fans_count; //粉丝数量
    string _sub_type; //用途
    int _ptype; //公共主页类型
  public:
    SearchPageView() {}
    ~SearchPageView() {}
    void setPType(int ptype);
    void setFansCount(int fans_count);
    void setSubType(string sub_type); 
    int getPType();
    int getFansCount();
    string getSubType();
};
typedef IceUtil::Handle<SearchPageView> SearchPageViewPtr;


class SearchResult : virtual public IceUtil::Shared {

  public:
    int& getTotalCount();
    int getCurrentPosition();
    double getTimeCost();
    vector<SearchViewPtr>&  getContents();
    map<IndexType, int>& getTypedCount();
    void setTotalCount(int total_count);
    void setCurrentPosition(int current_position);
    void setTimeCost(double time_cost);
    void setContents(vector<SearchViewPtr>& contents);
    void setTypedCount(map<IndexType, int>& typed_count);

  private:
    int _total_count;
    int _current_position;
    double _time_cost;
    vector<SearchViewPtr>  _contents;
    map<IndexType, int> _typed_count;
};

typedef IceUtil::Handle<SearchResult> SearchResultPtr;


class SearchViewHelper {//TODO 统一User,App和Page取资料
  public:
    SearchViewHelper(int userid);
    SearchResultPtr searchViewFilter(IndexResultPtr result);//暴露给外边的接口
    //    SearchResultPtr getSearchView();
    bool isAdmin(long userid);
  private:
    void assembleUserView(SearchUserViewPtr view_ptr, SearchCacheMemDataPtr mem_ptr, string is_friend, string friend_type, string common_friends_num, bool basic_info, bool education, bool workplaces);
    void divideView(IndexResultPtr result); 
    void generateUserView();
    void generatePageView();
    void generateFinalView(IndexResultPtr result);
    void filterPrivacy(long userid, vector<long>&  searchedIds);
    int educationPrivacy(string profile_privacy);
    int regionPrivacy(string profile_privacy);
    int workplacePrivacy(string profile_privacy);
    bool isSameNetwork(SearchCacheMemDataPtr user, SearchCacheMemDataPtr searched_result);

    long get_now(){
      struct timeval tv;
      memset(&tv, 0, sizeof(tv));
      gettimeofday(&tv, NULL);
      return tv.tv_sec*1000+tv.tv_usec/1000;
    }

    long _userid;
    vector<SearchUserViewPtr> _user_contents;
    vector<SearchPageViewPtr> _page_contents; 
    SearchResultPtr _result_ptr;
    vector<long> _user_ids;
    vector<long> _page_ids;
    vector<string> _common_friends;
    vector<string> _friends_type;
    vector<string> _is_friend;
    vector<bool> _user_danger;
};


#endif
