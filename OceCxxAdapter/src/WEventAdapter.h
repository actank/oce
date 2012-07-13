#ifndef WEBEVENTSADAPTER_H_
#define WEBEVENTSADAPTER_H_


#include "AdapterI.h"
#include "Singleton.h"
#include "WEvent.h"
#include "XFeed.h"
#include "Dispatch.h"

namespace talk{
namespace adapter {

using namespace talk::wevent;
using namespace xce::xfeed;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;

class WEventAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, WEventAdapter> {
public:
	WEventAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void deliverNotify(Ice::Int userid,const string& title, const string& msg);
	void deliverXFeed(const XFeedLetterPtr& xftl);
	void loadOfflineXFeed(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId);
	//void deliverReply(const mop::hi::svc::dispatch::NotePtr& note);
	
protected:

	WEventManagerPrx getManagerOneway(int id);
	virtual string name() {
		return "WEvent";
	}
	virtual string endpoints() {
		return "@WEvent";
	}
	virtual size_t cluster() {
		return 10;
	}
	vector<WEventManagerPrx> _managersOneway;

};
}
;
}
;



#endif /*WEBEVENTSADAPTER_H_*/
