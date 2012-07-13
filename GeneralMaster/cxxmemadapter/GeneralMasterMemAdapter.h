#ifndef __GENERALMASTER_MEM_ADAPTER_H__
#define __GENERALMASTER_MEM_ADAPTER_H__

#include <IceExt/src/AdapterI.h>
#include <CallGuarantee/src/CallGuaranteeManager.h>

namespace xce
{
	namespace generalmastermemadapter
	{
		template <class T> class GeneralMasterMemAdapterI :
						   public ::MyUtil::ReplicatedClusterAdapterI<T>,
					       public ::callguaranteemanager::CallGuaranteeManager
		{
			public:
				GeneralMasterMemAdapterI(const string& strEndpoint, const int interval, const int timeout,
									  const bool isMutilService = false, const string& identifier = "",
									  int redocount = 2, unsigned int redosize = 10000):
									 ::MyUtil::ReplicatedClusterAdapterI<T>(strEndpoint, interval, timeout, isMutilService, identifier),
									 ::callguaranteemanager::CallGuaranteeManager(redocount, redosize)
			{
			}
		};
	}
}
#endif
