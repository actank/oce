#ifndef _SOCIALGRAPHLOGIC_H_
#define _SOCIALGRAPHLOGIC_H_

#include <SocialGraphLogic.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>

#include <ServiceI.h>
#include "TaskManager.h"
#include "base/once.h"

#include "socialgraph/socialgraphutil/zkclient.h"
#include "SocialGraphWorkerAdapter.h"

namespace xce {
namespace socialgraph {
static const std::string SG_ZNODE = "/SOCIALGRAPHLOGIC";

static const int TASK_LEVEL_DISTRIBUTE = 1;

static const std::string g_token_1 = ":";
static const std::string g_token_2 = "_";

//---------------------------------------------------------------------------------------------------------------------------
//ZK代理负责获取zk通知
class ZooKeeperController : public xce::Observer {
public:
	static ZooKeeperController& GetInstance();

	ZooKeeperController(); 
	virtual ~ZooKeeperController();
	virtual void Update(const std::string&, const std::string&);
	std::string GetAdapter(const std::string&) const;

private:
	void Init();
	void UpdateTypeMap(const std::string&, const std::string&); 

	IceUtil::RWRecMutex init_flag_mutex_;
	std::map<std::string, std::string> type_adapter_map_;
};

//---------------------------------------------------------------------------------------------------------------------------
//服务
class SocialGraphLogicI : virtual public SocialGraphLogic,
		virtual public MyUtil::Singleton<SocialGraphLogicI> {
public:
	virtual void Report(const Message&, const Ice::Current&);
	~SocialGraphLogicI(); 

private:
	friend class MyUtil::Singleton<SocialGraphLogicI>;
};

//---------------------------------------------------------------------------------------------------------------------------
//分发线程
class DistributeTask : public MyUtil::Task {
public:
	DistributeTask(const Message& msg, ::Ice::Int level = TASK_LEVEL_DISTRIBUTE):
		MyUtil::Task(level), msg_(msg) {
	}
	virtual void handle();

private:
	void Notify(const std::string&, const std::string&, const Info&);

	Message msg_;
};

}
}

#endif
