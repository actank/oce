#include "SbjTopicI.h"

using namespace MyUtil;
using namespace std;

SubjectObserverPrx SbjTopicI::getProxy() {
	
	return _proxy;
}

void SbjTopicI::subscribe(const SbjSubscriberPrx& subscriber,
                             const Ice::Current& current)  {

	try {
		getProxy()->subscribe(_topicname, subscriber, current.ctx);
		// MCE_DEBUG("[SbjTopic] subscrited " << subscriber->ice_toString() << " for " << _topicname << " to " << _endpoints); 
	} catch (Ice::Exception& e) {
		MCE_WARN("[SbjTopic] subscribe exception " << _topicname << " " << e.what());
	} catch (...) {
		MCE_WARN("[SbjTopic] subscribe exception " << _topicname);
	}
}

void SbjTopicI::unsubscribe(const SbjSubscriberPrx& subscriber, const Ice::Current& current) {

	try {
		getProxy()->unsubscribe(_topicname, subscriber, current.ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[SbjTopic] unsubscribe exception " << _topicname << " " << e.what());
	} catch (...) {
		MCE_WARN("[SbjTopic] subscribe exception " << _topicname);
	}
}

void SbjTopicI::run() {
	while ( true ) {
		try {
			subscribe(_subscriber);
			} catch (Ice::Exception& e) {
                        MCE_WARN("[SbjTopic]: subscribe Thread Exception -> " << _topicname << "->" << e.what());
		} catch (std::exception& e) {
                        MCE_WARN("[SbjTopic]: subscribe Thread Exception -> " << _topicname << "->" << e.what());
		} catch (...) {
			MCE_WARN("[SbjTopic]: subscribe Thread Exception");
		}
		sleep(_interval);
	}
}
