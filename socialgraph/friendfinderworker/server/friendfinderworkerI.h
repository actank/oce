#ifndef _FRIENDFINDERWORKER_H_
#define _FRIENDFINDERWORKER_H_

#include <set>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "socialgraph/socialgraphutil/socialgraphworkerI.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "UtilCxx/src/TaskManager.h"
#include "socialgraph/friendfinderworker/algorithm/commonfriendexecuter.h"
#include "socialgraph/friendfinderworker/algorithm/commonfactory.h"
#include "item.pb.h"

namespace xce {
namespace socialgraph {

const static int LEVEL_REBUILD_CREATE = 1;
const static int LEVEL_REBUILD_NORMAL = 2;
const static int LEVEL_LOAD = 3;
const static int LEVEL_REMOVE = 4;
const static int LEVEL_BATCH_REBUILD = 5;

const static int rebuildInterval = 2 * 24 * 3600;  //重算时间2天
const static int CF_LOWERBOUNDARY = 5;
const static int CF_REBUILD_THRESHOLD = 18;   //当cache中可能认识人的数量小于18时，从db中取数据填充cache

//---------------------------------------------------------------------------------------------------------------

inline bool CommonFriendWeight(const CommonFriend& lhs, const CommonFriend& rhs) {
  return lhs.weight > rhs.weight;
}

//---------------------------------------------------------------------------------------------------------------

class FriendFinderWorkerI : virtual public SocialGraphWorkerI,
		virtual public MyUtil::Singleton<FriendFinderWorkerI> {
public:
	FriendFinderWorkerI() { 
		finish_load_ = true; 
	
		time_t t = time(0);
		struct tm *Tm = localtime(&t);
		time_ = Tm->tm_hour;
	}

	virtual void Report(const Info&, const Ice::Current&);	
	virtual void PreLoad(int, bool, const Ice::Current&);
	virtual void Rebuild(int, const Ice::Current&);

private:
	void SetRebuildList(int userid);
	void CheckRebuildTime();
	void BatchRebuild();

	friend class MyUtil::Singleton<FriendFinderWorkerI>;

	IceUtil::RWRecMutex monitor_;
	std::set<int> rebuild_list_;
	bool finish_load_;
	int time_;
};

//---------------------------------------------------------------------------------------------------------------

class RebuildTask : public MyUtil::Task {
public:
	RebuildTask(Ice::Int userid, bool flag, int level = LEVEL_REBUILD_NORMAL):
		Task(level), userid_(userid), flag_(flag) {
	}
	~RebuildTask() {
	}
	virtual void handle();

private:
	Ice::Int userid_;
	bool flag_;
};

//---------------------------------------------------------------------------------------------------------------

class CommonFriendRemoveTask : public MyUtil::Task {
public:
	CommonFriendRemoveTask(Ice::Int hostid, Ice::Int friendid, int level = LEVEL_REMOVE):
		MyUtil::Task(level), hostid_(hostid), friendid_(friendid) {
	}
	virtual void handle();

private:
	void UpdateMemcachedNormal(xce::socialgraph::Items& puk_friends);	
	void UpdateMemcachedExpand(xce::socialgraph::Items& puk_friends);

	Ice::Int hostid_;
	Ice::Int friendid_;
};

//---------------------------------------------------------------------------------------------------------------

class LoadTask : public MyUtil::Task {
public:
	LoadTask(Ice::Int userid, bool flag, int level = LEVEL_LOAD): 
		MyUtil::Task(level), userid_(userid), flag_(flag) {
	}
	virtual void handle();

private:
	Ice::Int userid_;
	bool flag_;
};

}
}


#endif
