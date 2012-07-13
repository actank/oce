#ifndef _PAGE_COMMON_FANS_ADAPTER_H_
#define _PAGE_COMMON_FANS_ADAPTER_H_

#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "PageCommonFans.h"

namespace xce {
namespace buddy {
namespace adapter {

class PageCommonFansCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::buddy::PageCommonFansCacheManagerPrx>,
                              public  MyUtil::Singleton<PageCommonFansCacheAdapter> {

public:
	PageCommonFansCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::buddy::PageCommonFansCacheManagerPrx>("ControllerPageCommonFansCache", 120, 1000) {
	}

  virtual MyUtil::IntSeq GetCommon(Ice::Int uid, 
                                   Ice::Int pid, 
                                   Ice::Int begin,  
                                   Ice::Int limit) {
		return getProxy(uid)->GetCommon(uid, pid, begin, limit);
	}


  virtual MyUtil::IntSeq GetUnCommon(Ice::Int uid,
                                     Ice::Int pid,
                                     Ice::Int begin,
                                     Ice::Int limit) {
		return getProxy(uid)->GetUnCommon(uid, pid, begin, limit);
	}

  virtual CommonFans GetCommonAndUnCommon(const CommonFansParam& param) {
		return getProxy(param.uid)->GetCommonAndUnCommon(param);
	}

  virtual bool SetData(const xce::buddy::PageCommonFans& pcf) {
		std::vector<xce::buddy::PageCommonFansCacheManagerPrx> proxies = getAllProxySeq(pcf.uid); 
		std::vector<xce::buddy::PageCommonFansCacheManagerPrx>::iterator iter = proxies.begin();
    for (; iter != proxies.end(); ++iter) {
			try {
				(*iter)->SetData(pcf);
			} catch (...) {
				continue;
			}
		}
		return true;
	}

	

};


}
}
}
#endif

