/*
 * TopicI.cpp
 *
 *  Created on: 2008-8-9
 *      Author: bbc
 */

#include "TopicI.h"
#include "ServiceI.h"
using namespace ::xce::storm;

KeepSubscribeThread::KeepSubscribeThread(TopicI& topicI) : _topicI(topicI){
}

void
KeepSubscribeThread::keep(const MyUtil::Str2StrMap& qos, const Ice::ObjectPrx& prx){
	IceUtil::Mutex::Lock lock(_mutex);
	struct SubscriberInfo info;
	info.qos = qos;
	info.prx = prx;
	_subscriberInfos.push_back(info);
}

void KeepSubscribeThread::run(){
	while(true){
		sleep(60);
		try{
			vector<struct SubscriberInfo> newInfos;
			{
				IceUtil::Mutex::Lock lock(_mutex);
				newInfos = _subscriberInfos;
			}
			MCE_INFO("KeepSubscribeThread::run() => will try to keep " << newInfos.size() << " proxies");
			for(std::vector<struct SubscriberInfo>::iterator it = newInfos.begin();
					it != newInfos.end(); ++it){
				try {
					MCE_INFO("KeepSubscribeThread::run() => will keep " << it->prx);
					_topicI.getTopic()->subscribeAndGetPublisher(it->qos, it->prx);
				} catch (IceStorm::AlreadySubscribed& e) {
					// OK, this is good.
				} catch (IceStorm::BadQoS& e) {
					MCE_WARN("KeepSubscribeThread::run => " << e);
				} catch (...) {
					MCE_WARN("KeepSubscribeThread::run => unknown execption");
				}
			}
		}catch(...){
			MCE_WARN("KeepSubscribeThread::run main loop exception ");
		}
	}
}


TopicI::TopicI(const TopicManagerIPtr& topicManager, const std::string& topicName) :
	_topicManager(topicManager), _topicName(topicName) {
	//std::cout << "TopicI A " << (_topicManager==0) << std::endl;
}

TopicI::TopicI(const std::string& topicManagerName,
		const std::string& topicName)
   : _topicManager(TopicManagerI::getInstance(topicManagerName)),_topicName(topicName)
{
//	std::cout << "TopicI B " << (_topicManager==0) << std::endl;
}

TopicI::TopicI(const std::string& topicName):
	_topicManager(TopicManagerI::getInstance("IceStorm")),_topicName(topicName)
{
//	std::cout << "TopicI C " << (_topicManager==0) << std::endl;
}

IceStorm::TopicPrx TopicI::getTopic() {
	try {
		return _topicManager->getTopicManager()->retrieve(_topicName);
	} catch (IceStorm::NoSuchTopic& e) {
		try {
			return _topicManager->getTopicManager()->create(_topicName);
		} catch (IceStorm::TopicExists& e1) {
			// insane exception
			return 0;
		}
	}
}

Ice::ObjectPrx TopicI::getPublisher() {
	if (!_publisher) {
		IceUtil::Mutex::Lock lock(_mutexPublisher);
		if (!_publisher) {
			_publisher = getTopic()->getPublisher();
		}
	}
	return _publisher;
	// return getTopic().getPublisher();
}

Ice::ObjectPrx TopicI::subscribe(const MyUtil::Str2StrMap& qos,
		const Ice::ObjectPtr& obj) {
//	std::cout << "subscribe" << std::endl;
	Ice::ObjectPrx result = addObject(obj);
	try {
		Ice::ObjectPrx publisher = getTopic()->subscribeAndGetPublisher(qos,
				result);
		if (!_publisher) {
			IceUtil::Mutex::Lock lock(_mutexPublisher);
			if (!_publisher) {
				_publisher = publisher;
			}
		}
	} catch (IceStorm::AlreadySubscribed& e) {
		MCE_WARN("TopicI::subscribe " << e);
	} catch (IceStorm::BadQoS& e) {
		MCE_WARN("TopicI::subscribe " << e);
	}

	IceUtil::Mutex::Lock lock(_mutexKeepSubscribeThread);
	if (_keepSubscribeThread){
		_keepSubscribeThread->keep(qos, result);
	}
	return result;
}

void TopicI::unsubscribe(Ice::ObjectPrx prx) {
	getTopic()->unsubscribe(prx);
}

Ice::ObjectPrx TopicI::addObject(const Ice::ObjectPtr& obj) {
//	std::cout << "addObject " << (_topicManager==0) << std::endl;
	Ice::ObjectPrx o = _topicManager->addObjectWithUUID(obj);
//	std::cout << "addObject done" << std::endl;
	return o;
}

void TopicI::startKeepSubscribeThread() {
	if (!_keepSubscribeThread) {
		IceUtil::Mutex::Lock lock(_mutexKeepSubscribeThread);
		_keepSubscribeThread = new KeepSubscribeThread(*this);
	}
	_keepSubscribeThread->start(128*1024);
}
