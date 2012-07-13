#ifndef __NOTIFY_SENDER_ADAPTER_H__
#define __NOTIFY_SENDER_ADAPTER_H__

#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include <NotifySender.h>

namespace xce {
namespace notifysender {
namespace adapter {

using namespace MyUtil;
using namespace com::xiaonei::wService;

class NotifySenderAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::WServiceChannel, NotifySenderAdapter>,
	virtual public IceUtil::Monitor<IceUtil::Mutex>  {
public:
	NotifySenderAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void sendByMessage(int toId, string message);
	void sendByAIGuideStage(int toId, AIGuideStage stage);
private:
	virtual string name() {
		return "M";
	}
	virtual string endpoints() {
		return "@NotifySender";
	}
	virtual size_t cluster() {
		return 1;
	}
	NotifySenderManagerPrx getNotifySenderManagerOneway(int id);
	vector<NotifySenderManagerPrx> _managersOneway;

};

};
};
};

#endif

