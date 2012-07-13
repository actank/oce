#ifndef __EDM_FEED_EDM_MODEL_H__
#define __EDM_FEED_EDM_MODEL_H__
#include <map>
#include <FeedEdmCache.h>
#include <set>
#include <list>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <string>
#include "Singleton.h"
#include "TaskManager.h"
#include "QueryRunner.h"

namespace xce {
namespace edm {
using namespace MyUtil;
using std::string;
class MiniteCache : public MyUtil::Timer, public MyUtil::Singleton<MiniteCache> {
public:
  MiniteCache(Ice::Int s = 60): Timer(s*1000) {}
  virtual void handle();
  bool Insert(Ice::Int edm_id);
  int GetSendTimes(Ice::Int edm_id);
  bool Reset();
private:
  std::map<Ice::Int,int> edm_send_info_;
  IceUtil::Mutex mutex_;
};
enum SendStatus { kUncomplete = 0, kReady, kSending, kComplete, kClose};

class ConfigureModel : public IceUtil::Shared {
public:
  Ice::Int id();
  void set_id(Ice::Int id);
  int type();
  void set_type(int type);
  int ad_type();
  void set_ad_type(int ad_type);
  int limit_type();
  void set_limit_type(int limit_type);
  int interactive_flag();
  void set_interactive_flag(int interactive_flag);
  int displayzzs();
  void set_displayzzs(int displayzzs);
  int demand();
  void set_demand(int demand);
  int max_per_hour();
  void set_max_per_hour(int max_per_hour);
  string name();
  void set_name(string name);
  int status();
  void set_status(int status);
  time_t begin_time();
  void set_begin_time(time_t begin_time);
  time_t create_ime();
  void set_create_ime(time_t create_ime);
  time_t update_ime();
  void set_update_ime(time_t update_ime);
  string title();
  void set_title(string title);
  string title_link();
  void set_title_link(string title_link);
  string body();       
  void set_body(string body);
  string body_link();
  void set_body_link(string body_link);
  string logo();
  void set_logo(string logo);
  string media();                  
  void set_media(string media);
  int customer_id();                        
  void set_customer_id(int customer_id);
  int consume();
  void set_consume(int consume);
  int click();
  void set_click(int click);
  int expose();
  void set_expose(int expose);
  int ilike();
  void set_ilike(int ilike);
  int share();
  void set_share(int share);
  int pub_type();
  void set_pub_type(int pub_type);
  string time_interval();
  void set_time_interval(string time_interval);
  string feed_clazz();
  void set_feed_clazz(string feed_clazz);
  int reply_count();
  void set_reply_count(int reply_count); 
  string social_title();
  void set_social_title(string social_title);
  int app_id();
  void set_app_id(int app_id);
  int display_friend_type();
  void set_display_friend_type(int display_friend_type);
  string like_page_link();
  void set_like_page_link(string like_page_link);
  string like_name();
  void set_like_name(string like_name);
  string ToString(); 
private:
  Ice::Int id_;
  int type_;
  int ad_type_;
  int limit_type_;
  int interactive_flag_;
  int displayzzs_;
  int demand_;
  int max_per_hour_;
  string name_;
  /* 0:不完整 1:待发 2：发送 3：发送完成 4:关闭 */
  int status_;
  time_t begin_time_;
  time_t create_ime_;
  time_t update_ime_;
  string title_;
  string title_link_;
  string body_;
  string body_link_;
  string logo_;
  string media_;
  int customer_id_;
  int consume_;
  int click_;
  int expose_;
  int ilike_;
  int share_;
  int pub_type_;
  string time_interval_;
  string feed_clazz_;
  int reply_count_;
  string social_title_;
  int app_id_;
  int display_friend_type_;
  string like_page_link_;
  string like_name_;
};
typedef IceUtil::Handle<ConfigureModel> ConfigureModelPtr;

class MarryInfo : public IceUtil::Shared {
};
typedef IceUtil::Handle<MarryInfo> MarryInfoPtr;

class NullTypeMarryInfo : public MarryInfo {
};

typedef IceUtil::Handle<NullTypeMarryInfo> NullTypeMarryInfoPtr;

struct School {
  string id_;
  string name_;
  string year_;
};
class OrientMarryInfo : public MarryInfo {
public:
  Ice::Int id();
  void set_id(Ice::Int id);
  Ice::Int edm_id();
  void set_edm_id(Ice::Int edm_id);
  int gender();
  void set_gender(int gender);
  int age();
  void set_age(int age);
  int age_start();
  void set_age_start(int age_start);
  int age_end();
  void set_age_end(int age_end);
  int stage();
  void set_stage(int stage);
  int area();
  void set_area(int area);
  int special();
  void set_special(int special);
  string city();
  void set_city(string city);
  string school();
  void set_school(string school);
  std::vector<string> province_area();
  void set_province_area(std::vector<string>& province_area);
  std::vector<string> province_city();
  void set_province_city(std::vector<string>& province_city);
  std::vector<School> univ();
  void set_univ(std::vector<School>& univ);
  std::vector<School> hight();
  void set_hight(std::vector<School>& hight);
private:
  Ice::Int id_;
  Ice::Int edm_id_;
  int gender_;
  int age_;
  int age_start_;
  int age_end_;
  int stage_;
  int area_;
  int special_;
  string city_;
  string school_;

  std::vector<string> province_area_;
  std::vector<string> province_city_;

  
  std::vector<School> univ_;
  std::vector<School> hight_;

public:
  string ToString();
};

typedef IceUtil::Handle<OrientMarryInfo> OrientMarryInfoPtr;

class AssignMarryInfo : public MarryInfo {
public:
  bool Lookup(Ice::Int user_id);
  bool Insert(Ice::Int user_id);
  bool Remove(Ice::Int user_id);
  bool Swap(std::set<Ice::Int>& other_set); 
private:
  std::set<Ice::Int> assign_user_;
};

typedef IceUtil::Handle<AssignMarryInfo> AssignMarryInfoPtr;

enum PubType { kNullType = 0, kOrient, kAssign };

class FeedEdmModel;
typedef IceUtil::Handle<FeedEdmModel> FeedEdmModelPtr;
class FeedEdmModel : public IceUtil::Shared {
public: 
  FeedEdmModel(ConfigureModelPtr configure_model_ptr = 0); 
  virtual FeedEdmModelPtr ValidateFunc(const FeedUserInfoPtr& user) = 0;
  virtual bool LoadMarryInfo() = 0;
  virtual void AfterSender(Ice::Int user_id);
  virtual ConfigureModelPtr configure_model_ptr() { return configure_model_ptr_; }
  Ice::Int id() {return configure_model_ptr_->id();}
  int pub_type() { return configure_model_ptr_->pub_type();}
  int consume() { return configure_model_ptr_->consume(); }
  int demand() { return configure_model_ptr_->demand(); }
  int app_id() { return  configure_model_ptr_->app_id(); }
  int display_friend_type() { return configure_model_ptr_->display_friend_type(); }
  std::string social_title() { return configure_model_ptr_->social_title(); } 
  std::string like_page_link() { return configure_model_ptr_->like_page_link(); }
  std::string like_name() { return configure_model_ptr_->like_name(); }
  void set_partner(std::vector<Ice::Int>& partner) { partner_ = partner;}
  std::vector<Ice::Int> partner() { return partner_;}
  void set_status(int status) { configure_model_ptr_->set_status(status);}
  virtual string ToString() = 0;
protected:
  virtual bool _BasicValidate(const FeedUserInfoPtr& user);
  std::vector<Ice::Int> partner_;
  ConfigureModelPtr configure_model_ptr_;
};

class FeedNullTypeEdmModel : public FeedEdmModel {
public:
  FeedNullTypeEdmModel(ConfigureModelPtr configure_model_ptr = 0, NullTypeMarryInfoPtr marry_info_ptr = 0);
  virtual bool LoadMarryInfo(); 
  virtual FeedEdmModelPtr ValidateFunc(const FeedUserInfoPtr& user);  
  virtual string ToString();
private:
  NullTypeMarryInfoPtr marry_info_ptr_;
};
typedef IceUtil::Handle<FeedNullTypeEdmModel> FeedNullTypeEdmModelPtr;

class FeedOrientEdmModel : public FeedEdmModel {
public: 
  FeedOrientEdmModel(ConfigureModelPtr configure_model_ptr = 0, OrientMarryInfoPtr marry_info_ptr = 0); 
  virtual FeedEdmModelPtr ValidateFunc(const FeedUserInfoPtr& user);
  virtual bool LoadMarryInfo();
  virtual string ToString();
private:
  bool _GenderValidate(const FeedUserInfoPtr& user);
  bool _AgeValidate(const FeedUserInfoPtr& user);
  bool _StageValidate(const FeedUserInfoPtr& user);
  bool _SchoolValidate(const FeedUserInfoPtr& user);
  bool _CityByIPValidate(const FeedUserInfoPtr& user);
  bool _InfoValidate(const FeedUserInfoPtr& user);
private:
  string _NormalizeProviceName(const string& province);
  bool _FormatOrientMarryInfo(mysqlpp::Row& row);
  OrientMarryInfoPtr marry_info_ptr_;
};
typedef IceUtil::Handle<FeedOrientEdmModel> FeedOrientEdmModelPtr;

class FeedAssignEdmModel : public FeedEdmModel {
public:
  FeedAssignEdmModel(ConfigureModelPtr configure_model_ptr = 0, AssignMarryInfoPtr marry_info_ptr = 0);
  virtual void AfterSender(Ice::Int user_id);
  virtual bool LoadMarryInfo();
  virtual FeedEdmModelPtr ValidateFunc(const FeedUserInfoPtr& user);
  virtual string ToString();
private:
  class DbMarryInfoOperator : public MyUtil::Task {
    public:
      DbMarryInfoOperator(Ice::Int edm_id, Ice::Int user_id) : Task(TASK_LEVEL_QUERY), edm_id_(edm_id), user_id_(user_id) {}
      virtual void handle();
    private:
      Ice::Int edm_id_;
      Ice::Int user_id_;
  };
  typedef IceUtil::Handle<DbMarryInfoOperator> DbMarryInfoOperatorPtr;
private:
  AssignMarryInfoPtr marry_info_ptr_;
};
typedef IceUtil::Handle<FeedAssignEdmModel> FeedAssignEdmModelPtr;

}
}
#endif
