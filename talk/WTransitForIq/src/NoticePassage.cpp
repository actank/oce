#include "NoticePassage.h"


#include "MessageType.h"
#include "TalkDeliverAdapter.h"
#include "XmppTools.h"
#include "TalkUtil/src/pugixml.hpp"
#include "QueryRunner.h"
#include "OnlineCenterAdapter.h"
#include "IceLogger.h"
//#include "NotifyRplAdapter.h"
#include <boost/lexical_cast.hpp>
#include <util/cpp/String.h>
//#include "FeedDBAdapter.h"
#include "FeedContentAdapter.h"
#include "TransitAider.h"


using namespace talk::wtransit;
using namespace pugi;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace ::xce::feed;
using namespace ::com::xiaonei::xce;
//using namespace mop::hi::svc::dispatch;
using namespace talk::online::adapter;



void NoticePipe::handle(const ObjectSeq& objs){
	
	MessageSeq msgs;
	IntSeq ids;
	for (size_t i = 0; i < objs.size(); ++i) {
		MessagePtr m = MessagePtr::dynamicCast(objs.at(i));
		msgs.push_back(m);
		ids.push_back(m->to->userId);
	}
	JidSeqMap jmap;
	try{
		jmap = OnlineCenterAdapter::instance().getUsersJids(ids);
	}catch(Ice::Exception& e){
		MCE_WARN("NoticePipe::handle-->OnlineCenterAdapter::getUsersJids-->" << e);
		}

	MessageSeq res;
	for (size_t i = 0; i < msgs.size(); ++i) {
		JidSeqMap::iterator it = jmap.find(msgs.at(i)->to->userId);
		if (it == jmap.end()) {
			continue;
		}
		for (size_t j = 0; j < it->second.size(); ++j) {
			if (it->second.at(j)->endpoint.size() > 1
					&& it->second.at(j)->endpoint[0] == 'T') {
				MessagePtr m = new Message();
				m->from = new Jid();
				m->from->userId = -1;
				m->from->endpoint = "appnotice.talk.xiaonei.com";
				m->from->index = -1;
				m->to = it->second.at(j);
				m->type = msgs.at(i)->type;
				m->msg = msgs.at(i)->msg;

				res.push_back(m);
			}

		}
	}

	if (!res.empty()) {
		MCE_DEBUG("Notice size:"<<res.size());
		try{
		TalkDeliverAdapter::instance().deliver(res);
		}catch(Ice::Exception& e){
			MCE_WARN("NoticePipe::handle-->TalkDeliverAdapter::deliver-->" << e);
			}
	}
}
