#ifndef ACTIVEMOTION_FACADE_ADAPTER_H_
#define ACTIVEMOTION_FACADE_ADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "ActiveTrack.h"

namespace xce {
namespace activetrack {
namespace adapter {

class ActiveMotionFacadeAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::activetrack::ActiveMotionFacadePrx>, public MyUtil::Singleton<ActiveMotionFacadeAdapter> {
public:
	ActiveMotionFacadeAdapter() :
		MyUtil::ReplicatedClusterAdapterI <xce::activetrack::ActiveMotionFacadePrx>("ControllerActiveMotionFacade", 120, 1000) {
	}

        void motion(const MyUtil::StrSeq& );
};


};
};
};

#endif /*ACTIVEMOTION_FACADE_ADAPTER_H_*/
