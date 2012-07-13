#ifndef _SOCIALGRAPHWORKER_ADAPTER_H_
#define _SOCIALGRAPHWORKER_ADAPTER_H_

#include "SocialGraphWorker.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class SocialGraphWorkerPrxData : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphWorkerPrx>, 
		virtual public Ice::Object {
public:
	SocialGraphWorkerPrxData(const std::string& endpoint) :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphWorkerPrx>(endpoint, 120, 1000) {
	}

	void Report(int userId, const Info& information);
	void PreLoad(int userId, bool signal);
	void Rebuild(int userId);
};
typedef IceUtil::Handle<SocialGraphWorkerPrxData> ProxyTypePtr;

//-------------------------------------------------------------------------------------

class SocialGraphWorkerAdapter : public MyUtil::Singleton<SocialGraphWorkerAdapter> {
public:
	SocialGraphWorkerAdapter() {
	}
	void Report(const std::string& endpoint, int userId, const Info& information);
	void PreLoad(const std::string& endpoint, int userId, bool signal);
	void Rebuild(const std::string& endpoint, int userId);

private:
	IceUtil::RWRecMutex init_flag_mutex_;
	std::map<std::string, ProxyTypePtr> proxyptr_map_;
};

}
}

#endif
