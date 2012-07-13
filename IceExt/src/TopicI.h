/*
 * TopicI.h
 *
 *  Created on: 2008-8-9
 *      Author: bbc
 */

#ifndef TOPICI_H_
#define TOPICI_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceStorm/IceStorm.h>

#include "TopicManagerI.h"
#include "Util.h"

namespace xce {
namespace storm {
class TopicI;
typedef IceUtil::Handle<TopicI> TopicIPtr;

class KeepSubscribeThread : virtual public IceUtil::Thread{
public:
	KeepSubscribeThread(TopicI& topicI);
	void keep(const MyUtil::Str2StrMap& qos, const Ice::ObjectPrx& prx);
	virtual void run();
private:
    IceUtil::Mutex _mutex;
	TopicI& _topicI;
	struct SubscriberInfo{
		MyUtil::Str2StrMap qos;
		Ice::ObjectPrx prx;
	};
	std::vector<struct SubscriberInfo> _subscriberInfos;
};
typedef IceUtil::Handle<KeepSubscribeThread> KeepSubscribeThreadPtr;

class TopicI : virtual public IceUtil::Shared {
public:
	TopicI(const TopicManagerIPtr& topicManager, const std::string& topicName);
	TopicI(const std::string& topicManagerName, const std::string& topicName);
	TopicI(const std::string& topicName);
	IceStorm::TopicPrx getTopic();
	Ice::ObjectPrx getPublisher();
	Ice::ObjectPrx subscribe(const MyUtil::Str2StrMap& qos, const Ice::ObjectPtr& obj);
	void unsubscribe(Ice::ObjectPrx prx);
	Ice::ObjectPrx addObject(const Ice::ObjectPtr& obj);
	void startKeepSubscribeThread();
private:
	TopicManagerIPtr _topicManager;
	std::string _topicName;

	Ice::ObjectPrx _publisher;
	IceUtil::Mutex _mutexPublisher;

	KeepSubscribeThreadPtr _keepSubscribeThread;
	IceUtil::Mutex _mutexKeepSubscribeThread;
};

}
}

#endif /* TOPICI_H_ */
