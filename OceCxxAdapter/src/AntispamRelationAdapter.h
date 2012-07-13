#ifndef _ANTISPAMRELATIONADAPTER_H_
#define _ANTISPAMRELATIONADAPTER_H_

#include "AntispamRelation.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace antispamrelation {
namespace adapter {

using namespace ::com::xiaonei::ozein::monitor::generation;

class AntispamRelationAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::MsgFilterChannel, AntispamRelationAdapter> {
public:
	AntispamRelationAdapter(){
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}
	

	void notify(MyUtil::Str2StrMap map);
	
protected:
	virtual string name() {
		return "RelationMonitor";
	}
	virtual string endpoints() {
		return "@RelationMonitor";
	}
	virtual size_t cluster() {
		return 0;
	}
	virtual void initialize();

	RelationMonitorPrx getRelationMonitor(int id);
	RelationMonitorPrx getRelationMonitorOneway(int id);
	typedef vector<RelationMonitorPrx> RelationMonitorSeq;
	RelationMonitorSeq _managersOneway;
	RelationMonitorSeq _managers;
};


}
}
}
#endif
