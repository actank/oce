#ifndef _ADANALYSISMANAGERI_H_
#define _ADANALYSISMANAGERI_H_

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
#include "AdAnalysis.h"
#include "Singleton.h"
#include "Date.h"

namespace xce{
namespace ad{
using namespace std;
using namespace com::xiaonei::xce;

class Member;
class Plan;
class Group;

typedef ::IceUtil::Handle<Member> MemberPtr;
typedef vector<MemberPtr> MemberSeq;
typedef map<Ice::Long, MemberPtr> MemberMap;

typedef ::IceUtil::Handle<Plan> PlanPtr;
typedef vector<PlanPtr> PlanSeq;
typedef map<Ice::Long, PlanPtr> PlanMap;

typedef ::IceUtil::Handle<Group> GroupPtr;
typedef vector<GroupPtr> GroupSeq;
typedef map<Ice::Long, GroupPtr> GroupMap;

const string DB_INSTANCE = "ad_db";

IceUtil::Mutex mutex_seq[100];

class Member : public IceUtil::Shared{
public:
	Ice::Long member_id;
	int statues;
	bool Available();
};

class Plan : public IceUtil::Shared{
public:
  Ice::Long plan_id;
  Ice::Long member_id;
	time_t start_time;
	time_t end_time;
	int running_area_hour;
	int running_area_day;
	bool is_active;
	bool Available();
};

class Group : public IceUtil::Shared{
public:
  Ice::Long group_id;
  Ice::Long plan_id;
  Ice::Long member_id;
  Ice::Long zone_id;
  int highest_bid;
	bool is_active;
	bool Available();
};

class AdAnalysisManagerI: virtual public AdAnalysisManager,
    virtual public MyUtil::Singleton<AdAnalysisManagerI> {
public:
	AdAnalysisManagerI();
	void Load();
	virtual void Pv(const AdPvParameterSeq& paraseq, const Ice::Current& = Ice::Current());
	virtual void Click(const AdClickParameterPtr& para, const Ice::Current& = Ice::Current());

	virtual void Pv2(const AdPvParameterSeq& paraseq, const MyUtil::StrSeq& append_str,const Ice::Current& = Ice::Current());
	virtual void Click2(const AdClickParameterPtr& para, const string& append_str, const Ice::Current& = Ice::Current());

	virtual void ReloadGroupById(Ice::Long group_id, const Ice::Current& = Ice::Current());
	virtual void ReloadMemberById(Ice::Long member_id, const Ice::Current& = Ice::Current());
	virtual void ReloadCampaignById(Ice::Long plan_id, const Ice::Current& = Ice::Current());
	virtual void ReloadCreativeById(Ice::Long creative_id, const Ice::Current& = Ice::Current());


	void PushAvailableGroups();
	void LoadCacheData();
	bool Check(Ice::Long group_id);
	GroupPtr GetGroup(Ice::Long group_id);
	MemberPtr GetMember(Ice::Long member_id);
	PlanPtr GetPlan(Ice::Long plan_id);
	void LoadGroup(Ice::Long group_id);
	void LoadMember(Ice::Long member_id);
	void LoadPlan(Ice::Long plan_id);
private:
	GroupMap group_cache_;
	MemberMap member_cache_;
	PlanMap plan_cache_;

	IceUtil::RWRecMutex cache_mutex_;

	bool init_ok_;
};


class LoadTask : virtual public MyUtil::Task{
public:
	LoadTask(){}
	virtual void handle();
};

class LoadTimer : virtual public MyUtil::Timer{
public:
  LoadTimer() : Timer(1000*60){

  }
  virtual void handle(){
	MCE_DEBUG("_______LOAD_______");
    AdAnalysisManagerI::instance().LoadCacheData();
  }
};


class ReloadGroupTimer : virtual public MyUtil::Timer{
public:
  ReloadGroupTimer(Ice::Long group_id):Timer(60 * 1000), group_id_(group_id){
    ;
  }
  virtual void handle();
private:
  Ice::Long group_id_;
};

class ReloadMemberTimer : virtual public MyUtil::Timer{
public:
  ReloadMemberTimer(Ice::Long member_id):Timer(60 * 1000), member_id_(member_id){
    ;
  }
  virtual void handle();
private:
  Ice::Long member_id_;
};

class ReloadPlanTimer : virtual public MyUtil::Timer{
public:
  ReloadPlanTimer(Ice::Long plan_id):Timer(60 * 1000),plan_id_(plan_id){
    ;
  }
  virtual void handle();
private:
  Ice::Long plan_id_;
};

class ReloadCreativeTimer : virtual public MyUtil::Timer{
public:
  ReloadCreativeTimer(Ice::Long creative_id):Timer(60 * 1000), creative_id_(creative_id){
    ;
  }
  virtual void handle();
private:
  Ice::Long creative_id_;
};




class PushAvailableGroupsTimer : virtual public MyUtil::Timer{
public:
  PushAvailableGroupsTimer(int reset_interval_) :
    Timer(reset_interval_) {
    ;
  }
  virtual void handle();
};

};
};
#endif
