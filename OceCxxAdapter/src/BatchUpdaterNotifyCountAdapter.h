#ifndef __BATCHUPDATERNOTIFYCOUNT_ADAPTER_H__
#define __BATCHUPDATERNOTIFYCOUNT_ADAPTER_H__

#include "BatchUpdater.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

class BatchUpdaterNotifyCountAdapter : public AdapterI,
	public AdapterISingleton<OceChannel, BatchUpdaterNotifyCountAdapter> {
public:

	void inc(int id, int count);
	void dec(int id, int count);
	void set(int id, int count);

private:
	virtual void initialize();

	BatchUpdaterManagerPrx _batchUpdaterManager;
	BatchUpdaterManagerPrx _batchUpdaterManagerOneway;

	virtual string name() {
		return "BatchUpdaterUserCount";
	}
	;
	virtual string endpoints() {
		return "@BatchUpdaterUserCount";
	}
	;
	virtual size_t cluster() {
		return 0;
	}
	;
};

}
;
}
;
}
;
}
;

#endif

