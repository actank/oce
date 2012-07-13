#ifndef __CALLGUARANTEEADAPTER_H__
#define __CALLGUARANTEEADAPTER_H__

#include <IceExt/src/AdapterI.h>
#include "CallGuaranteeManager.h"

namespace callguarantee {

template <class T> class CallGuaranteeAdapterI : public ::MyUtil::ReplicatedClusterAdapterI<T>,
		public ::callguarantee::CallGuaranteeManager {
public:
	CallGuaranteeAdapterI(const string& strEndpoint, const int interval, const int timeout,
		  const bool isMutilService = false, const string& identifier = "",
			  int redocount = 2, unsigned int redosize = 10000):
				 ::MyUtil::ReplicatedClusterAdapterI<T>(strEndpoint, interval, timeout, isMutilService, identifier),
					 ::callguarantee::CallGuaranteeManager(redocount, redosize) {}
};

}
#endif
