#ifndef XCEALERTADAPTER_H_
#define XCEALERTADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include <XceAlert.h>
#include <AdapterI.h>
#include <TaskManager.h>

namespace com {
namespace xiaonei {
namespace xce {

class XceAlertAdapter : public MyUtil::ReplicatedClusterAdapterI<XceAlertPrx>, public MyUtil::Singleton<XceAlertAdapter> {
public:
	XceAlertAdapter();
	void reloadAlertMap();	
	void notifyAlert(const MyUtil::StrSeq& typeSeq, const std::string& msg);
	void notifyAlertN(const MyUtil::StrSeq& typeSeq, const std::string& msg, int sendType);
};

}
}
}
#endif /* USERCACHECLIENT_H_ */
