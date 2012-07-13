#ifndef _SOCIALGRAPHWORKERI_H_
#define _SOCIALGRAPHWORKERI_H_

#include <SocialGraphWorker.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "LogWrapper.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "socialgraph/socialgraphutil/zkclient.h"
#include "base/once.h"

namespace xce {
namespace socialgraph {
static const std::string SG_ZNODE = "/SOCIALGRAPHLOGIC";

static const std::string g_token_1 = ":";
static const std::string g_token_2 = "_";

//-----------------------------------------------------------------------------------------------------

class SocialGraphWorkerI : public SocialGraphWorker {
public:
	virtual void Report(const Info& information, const Ice::Current& current) = 0;
	virtual void PreLoad(int userId, bool signal, const Ice::Current& current) = 0;
	virtual void Rebuild(int userId, const Ice::Current& current) = 0;
	virtual ~SocialGraphWorkerI() {}

	void Register(const std::string& type, const std::string& endpoint); 
};

//-----------------------------------------------------------------------------------------------------

class ZooKeeperController : public xce::Observer {
public:
    static ZooKeeperController& GetInstance();

    ZooKeeperController();
    virtual ~ZooKeeperController();
    virtual void Update(const std::string&, const std::string&);
    std::string GetAdapter(const std::string& key);
	bool IsHasType(const std::string& key);
	void SetZnode(const std::string& key, const std::string& value);

private: 
    void Init();
    void UpdateTypeMap(const std::string& key, const std::string& value);

    IceUtil::RWRecMutex init_flag_mutex_;
    std::map<std::string, std::string> type_adapter_map_;
};

}
}
#endif
