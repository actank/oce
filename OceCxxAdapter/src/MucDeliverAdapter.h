#ifndef MUCDELIVERADAPTER_H_
#define MUCDELIVERADAPTER_H_

#include "MucDeliver.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace mucdeliver {
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::mucdeliver;

class MucDeliverAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, MucDeliverAdapter> {
public:
	MucDeliverAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void deliver(const MessageSeq& seq);
protected:
	virtual string name() {
		return "MucDeliver";
	}
	virtual string endpoints() {
		return "@MucDeliver";
	}
	virtual size_t cluster() {
		return 10;
	}

	MucDeliverManagerPrx getManager(int id);
	MucDeliverManagerPrx getManagerOneway(int id);

	vector<MucDeliverManagerPrx> _managersOneway;
	vector<MucDeliverManagerPrx> _managers;
};

}
;
}
;
}
;

#endif /*TALKMESSAGEADAPTER_H_*/
