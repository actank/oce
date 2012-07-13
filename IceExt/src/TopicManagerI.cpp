/*
 * TopicManagerI.cpp
 *
 *  Created on: 2008-8-9
 *      Author: bbc
 */

#include "TopicManagerI.h"

using namespace xce::storm;
using namespace MyUtil;
using namespace std;
using namespace Ice;
using namespace IceStorm;

IceUtil::Mutex TopicManagerI::_instanceMutex;
TopicManagerI::InstancesType TopicManagerI::_instances;
TopicManagerIPtr TopicManagerI::getInstance(const std::string& instanceName) {
//	std::cout << "start getInstance " << instanceName << std::endl;
	InstancesType::iterator it = _instances.find(instanceName);
	if (it == _instances.end()) {
		IceUtil::Mutex::Lock lock(_instanceMutex);
		it = _instances.find(instanceName);
		if (it == _instances.end()) {
			_instances.insert(make_pair(instanceName,
					new TopicManagerI(instanceName)));
			it = _instances.find(instanceName);// TODO: use insert result instead.
		}
	}
//	std::cout << "end getInstance " << instanceName << " " << (it->second == 0) << std::endl;
	return it->second;
}

TopicManagerI::TopicManagerI(const std::string& instanceName) :
	_instanceName(instanceName) {
	std::ostringstream idBuff;
	idBuff << _instanceName << "/TopicManager@" << _instanceName
			<< ".TopicManager";
	_topicManager = IceStorm::TopicManagerPrx::uncheckedCast(
			_channel .getCommunicator()->stringToProxy(idBuff.str()));
}

IceStorm::TopicManagerPrx TopicManagerI::getTopicManager() {
	return _topicManager;
}

Ice::ObjectPrx TopicManagerI::addObjectWithUUID(const Ice::ObjectPtr& obj) {
//	std::cout << "addObjectWithUUID" << std::endl;
	if (!_adapter) {
		IceUtil::Mutex::Lock lock(_adapterMutex);
		if (!_adapter) {
			_adapter
					= _channel.getCommunicator()->createObjectAdapterWithEndpoints(
							"dummy", "tcp -h " + IpAddr::inner());
			_adapter->activate();
		}
	}
	return _adapter->addWithUUID(obj);
}
