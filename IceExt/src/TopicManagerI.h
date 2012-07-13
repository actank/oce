/*
 * TopicManagerI.h
 *
 *  Created on: 2008-8-9
 *      Author: bbc
 */

#ifndef TOPICMANAGERI_H_
#define TOPICMANAGERI_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceStorm/IceStorm.h>
#include "Channel.h"

namespace xce {
namespace storm {
class TopicManagerI;
typedef IceUtil::Handle<TopicManagerI> TopicManagerIPtr;

class TopicManagerI: virtual public IceUtil::Shared {
public:
	static TopicManagerIPtr getInstance(const std::string& instanceName);
	IceStorm::TopicManagerPrx getTopicManager();
	Ice::ObjectPrx addObjectWithUUID(const Ice::ObjectPtr& obj);

private:
	typedef std::map<std::string, TopicManagerIPtr> InstancesType;
	static InstancesType _instances;
	static IceUtil::Mutex _instanceMutex;

	TopicManagerI(const std::string& instanceName);

	std::string _instanceName;
	MyUtil::OceChannel _channel;
	Ice::ObjectAdapterPtr _adapter;
	IceUtil::Mutex _adapterMutex;

	IceStorm::TopicManagerPrx _topicManager;
};
}
}
#endif /* TOPICMANAGERI_H_ */
