#ifndef ACTIVETRACK_FACADE_ADAPTER_H_
#define ACTIVETRACK_FACADE_ADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "ActiveTrack.h"

namespace xce {
namespace activetrack {
namespace adapter {

class ActiveTrackFacadeAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::ActiveTrackChannel, ActiveTrackFacadeAdapter> {
public:
	ActiveTrackFacadeAdapter() {
		_managersUDP.resize(cluster() > 0 ? cluster() : 1);
	}
        void action(int, const MyUtil::StrSeq&, const std::string&, const std::string&);
protected:
	virtual string name() {
		return "ActiveTrackFacade";
	}
	virtual string endpoints() {
		return "@ActiveTrackFacade";
	}
	;
	virtual size_t cluster() {
		return 1;
	}

	xce::activetrack::ActiveTrackFacadePrx getManagerUDP(int id);

	std::vector<xce::activetrack::ActiveTrackFacadePrx> _managersUDP;

};



};
};
};

#endif /*ACTIVETRACK_FACADE_ADAPTER_H_*/
