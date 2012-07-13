#ifndef __ADUSERCACHE_LOADER_I_H__
#define __ADUSERCACHE_LOADER_I_H__

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include <QueryRunner.h>
#include "ServiceI.h"
#include "AdUserCache.h"
#include "Singleton.h"
#include "AdUserCacheDataI.h"
#include "FeedMemcProxy/client/user_profile_client.h"

namespace xce {
namespace ad {

const std::string DB_USER_BORN = "useradscache_user_born";//gender && age
const std::string DB_USER_STAGE = "useradscache_user_born";////stage
const std::string DB_REGION_INFO = "useradscache_region_info";//region
const std::string DB_UNIVERSITY_INFO = "useradscache_university_info";//school && grade
const std::string DB_UNIVERSITY = "useradscache_university_info";
const std::string DB_AD_CHINA_AREA = "useradscache_university_info";
const std::string DB_AD_USER_INFO = "ad_db";
const std::string DB_WORKPLACE_INFO = "useradscache_workplace_info";

const std::string DB_HIGH_SCHOOL_INFO = "useradscache_high_school_info";//高中
const std::string DB_COLLEGE_INFO = "useradscache_college_info";//技校
const std::string DB_JUNIOR_HIGH_SCHOOL_INFO = "useradscache_junior_high_school_info";//初中
const int LOAD_TIMEOUT = 150;

class UserDataLoader;
typedef IceUtil::Handle<UserDataLoader> UserDataLoaderPtr;
class LoaderHandler;
typedef IceUtil::Handle<LoaderHandler> LoaderHandlerPtr;

class UserDataLoader: public IceUtil::Shared, public IceUtil::Monitor<
    IceUtil::Mutex> {
public:
  UserDataLoader() :
    count_(0){
  }
  ;
  void query();
  void addQuery(const LoaderHandlerPtr& handler);
  void finish() {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    if (--count_ == 0) {
      notify();
    };
  }
private:
  int count_;
  vector<MyUtil::TaskPtr> taskseq_;

  class SqlExecuteTask: public MyUtil::Task {
  public:
    SqlExecuteTask(const UserDataLoaderPtr& loader, const LoaderHandlerPtr& handler) :
      Task(), loader_(loader), handler_(handler){
    }
    ;
    virtual void handle();

  private:
    UserDataLoaderPtr loader_;
    LoaderHandlerPtr handler_;
  };
};


class LoaderHandler : public IceUtil::Shared{
public:
  virtual void handle(){}
};


class UserBornLoaderHandlerI : public LoaderHandler{
public:
  UserBornLoaderHandlerI(const string& pattern, const UserProfileWrapperMap& result) : result_(result){
    sql_
        << "SELECT id, gender, birth_year, birth_month, birth_day, home_city FROM user_born WHERE id IN " << pattern;
  }
  virtual void handle();
private:
  com::xiaonei::xce::Statement sql_;
  UserProfileWrapperMap result_;
};

class UserStageLoaderHandlerI : public LoaderHandler{
public:
  UserStageLoaderHandlerI(const string& pattern, const UserProfileWrapperMap& result) : result_(result){
    sql_ << "SELECT id, stage FROM user_stage WHERE id IN " << pattern;
  }
  virtual void handle();
private:
  com::xiaonei::xce::Statement sql_;
  UserProfileWrapperMap result_;
};

class UniversityLoaderHandlerI : public LoaderHandler{
public:
  UniversityLoaderHandlerI(const string& pattern, const UserProfileWrapperMap& result) : result_(result){
    sql_
          << "SELECT userid,university_id, type_of_course, max(enroll_year) as enroll_year FROM university_info WHERE userid IN "
          << pattern << " group by userid";
  }
  virtual void handle();
private:
  com::xiaonei::xce::Statement sql_;
  UserProfileWrapperMap result_;
};

class WorkplaceLoaderHandlerI : public LoaderHandler{
public:
  WorkplaceLoaderHandlerI(const string& pattern, const UserProfileWrapperMap& result) : result_(result){
    sql_ << "select userid, city_id, min( join_year ) as join_year from workplace_info where userid in "<<pattern << " group by userid";
  }
  virtual void handle();
private:
  com::xiaonei::xce::Statement sql_;
  UserProfileWrapperMap result_;
};

class JuniorLoaderHandlerI : public LoaderHandler{
public:
  JuniorLoaderHandlerI(const string& pattern, const UserProfileWrapperMap& result) : result_(result){
    sql_ << "select  userid, junior_high_school_id, max( junior_high_school_year ) as junior_high_school_year from junior_high_school_info where userid in " << pattern << " group by userid";
  }
  virtual void handle();
private:
  com::xiaonei::xce::Statement sql_;
  UserProfileWrapperMap result_;
};

class HighSchoolLoadertHandlerI : public LoaderHandler{
public:
  HighSchoolLoadertHandlerI(const string& pattern, const UserProfileWrapperMap& result) : result_(result){
    sql_ << "select userid, high_school_id, max(enroll_year) as enroll_year from high_school_info where userid in " << pattern << " group by userid";
  }
  virtual void handle();
private:
  com::xiaonei::xce::Statement sql_;
  UserProfileWrapperMap result_;
};


class AdUserAreaCache: public IceUtil::Shared {
public:
  Ice::Long univ_id_;
  int region_id_;
  string area_code_;
};
typedef ::IceUtil::Handle<AdUserAreaCache> AdUserAreaCachePtr;
typedef ::std::vector<AdUserAreaCachePtr> AdUserAreaCacheSeq;
typedef ::std::map<Ice::Long, AdUserAreaCachePtr> AdUserAreaCacheMap;

class AdUserCacheLoaderI: virtual public AdUserCacheLoader,
    virtual public MyUtil::Singleton<AdUserCacheLoaderI> {
public:
  AdUserCacheLoaderI();
  virtual ~AdUserCacheLoaderI() {
  }
  virtual void LoadByIdSeq(const ::MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());
  virtual void LoadById(Ice::Int userId, const Ice::Current& = Ice::Current());
  virtual void ReloadUserInfoByIdSeq(const ::MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());
  virtual void ReloadUserInfoById(Ice::Int userId, const Ice::Current& = Ice::Current());
  virtual string GetById(Ice::Int userId, const Ice::Current& = Ice::Current());
  void InitUserAreaData();
  void SetUserAreaData();
  string GetAreacodeFromUserAreaCache(Ice::Long univid);
  string GetAreacodeFromUserCityCache(const string& city_name);
  string GetAreacodeFromUserJuniorCache(Ice::Long juniorschool_id);
  string GetAreacodeFromUserHighSchoolCache(Ice::Long highschool_id);
  UserProfileWrapperMap CreateUserInfo(const MyUtil::IntSeq& userids);
private:
  AdUserAreaCacheMap user_area_data_;
  ::std::map<string, string> user_city_data_;
  ::std::map<Ice::Long, string> user_highschool_data_;
  ::std::map<Ice::Long, string> user_juniorschool_data_;
  IceUtil::RWRecMutex mutex_;
};

class LoadTask: virtual public ::MyUtil::Task {
public:
  LoadTask(const ::MyUtil::IntSeq& user_ids, bool is_reload = false);
  void LoadFromAdUserInfo();
  virtual void handle();
  ::std::vector<string> GetSerialize(){
    return UpfSerialize;
  }
private:
  MyUtil::IntSeq user_ids_;
  MyUtil::IntSeq miss_ids_;
  vector<string> UpfSerialize;
  bool is_reload_;
};

class InitUserAreaDataTask: virtual public ::MyUtil::Task {
public:
  InitUserAreaDataTask() {
  }
  virtual void handle();
};



class UserAreaDataResetTimer: virtual public ::MyUtil::Timer {
public:
  UserAreaDataResetTimer(int reset_interval_) :
    Timer(reset_interval_) {
    ;
  }
  virtual void handle();
};




}
}

#endif
