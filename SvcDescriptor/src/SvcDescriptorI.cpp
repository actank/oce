#include "SvcDescriptorI.h"
#include "ServiceI.h"
#include "Parser.h"
#include "Date.h"
#include <map>
#include <boost/lexical_cast.hpp>
#include "util/cpp/String.h"

using namespace std;
using namespace MyUtil;

//---------------------------------------------------------------------------

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	SvcDescriptorI* sd = new SvcDescriptorI;
	service.getAdapter()->add(sd, service.createIdentity(SVC_DESCRIPTOR, ""));
	sd->start(131072).detach();
}

SvcInstancePtr SvcDescriptorI::query(const string& name, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(*this);
	SvcInstanceMap::iterator pos = instances.find(name);
	if (pos != instances.end()) {
		MCE_DEBUG("SvcDescriptorI::query success -> " << name);
		return pos->second;
	}
	MCE_WARN("SvcDescriptorI::query none -> " << name);
	return 0;
}

void SvcDescriptorI::notify(const string& name, const Ice::Current& current) {
	IceUtil::Mutex::Lock lock(*this);
	SvcInstanceMap::iterator pos = instances.find(name);
	SvcInstancePtr svci;
	if (pos != instances.end()) {
		svci = pos->second;
	} else {
		MCE_INFO("SvcDescriptorI::notify " << name << " is NULL, create new one");
		svci = new SvcInstance;
		svci->name = name;
		instances.insert(make_pair(name, svci));
	}

	svci->timestamp = time(NULL);

	MCE_INFO("SvcDescriptorI::notify -> " << svci->name << " updated");
	ObserverSetMap::iterator pos1 = observers.find(svci->name);
	if (pos1 != observers.end()) {
		TaskManager::instance().execute(new NotifyTask(*this, svci, current.ctx,
				pos1->second));
	}
}

void SvcDescriptorI::subscribe(const string& name,
		const SvcObserverPrx& observer, const Ice::Current&) {
	MCE_DEBUG("SvcDescriptorI::subscribe -> " << name << " -> "
			<< observer->ice_toString());
	IceUtil::Mutex::Lock lock(*this);
	SvcInstanceMap::iterator posInstance = instances.find(name);
	if (posInstance == instances.end()) {
		SvcInstancePtr svci = new SvcInstance;
		svci->name = name;
		svci->timestamp = time(NULL);
		instances.insert(make_pair(name, svci));
	}

	ObserverSetMap::iterator pos = observers.find(name);
	if (pos == observers.end()) {
		observers[name].insert(observer);
	} else {
		ObserverSet& os = pos->second;
		if (os.find(observer) == os.end()) {
			os.insert(observer);
		}
	}
}

void SvcDescriptorI::unsubscribe(const string& name,
		const SvcObserverPrx& observer, const Ice::Current&) {
	IceUtil::Mutex::Lock lock(*this);
	ObserverSetMap::iterator pos = observers.find(name);
	if (pos != observers.end()) {
		ObserverSet& os = pos->second;
		os.erase(observer);
		MCE_DEBUG("SvcDescriptorI::unsubscribe -> " << name << ", "
				<< observer->ice_toString());
	}
}

void SvcDescriptorI::run() {
	while (true) {
		{
			IceUtil::Mutex::Lock lock(*this);
			TaskManager::instance().execute(new PingTask(*this, observers));
		}
		sleep(20* 60 );
	}
}

string SvcDescriptorI::checksum(const SvcInstancePtr& svci) {
	ostringstream oss;
	oss << svci->name << svci->timestamp;
	for (MyUtil::Str2StrMap::iterator it = svci->properties.begin(); it
			!= svci->properties.end(); ++it) {
		oss << "[" << it->first << "=" << it->second << "]";
	}
	return StrUtil::md5(oss.str());
}

void NotifyTask::handle() {
	for (ObserverSet::iterator i = _observers.begin(); i != _observers.end(); ++i) {
		try {
			(*i)->update(_instance, _ctx);
			MCE_INFO("NotifyTask::handle success -> " << _instance->name << " -> " << (*i)->ice_toString());
		} catch (Ice::Exception& e) {
			MCE_WARN("NotifyTask::handle exception -> " << _instance->name << " -> " << (*i)->ice_toString() << " -> " << e);
			continue;
		} catch (std::exception& e) {
			MCE_WARN("NotifyTask::handle exception -> " << _instance->name << " -> " << (*i)->ice_toString() << " -> " << e.what());
			continue;
		} catch (...) {
			MCE_WARN("NotifyTask::handle exception -> " << _instance->name << " -> " << (*i)->ice_toString() << " unknown");
			continue;
		}
	}
}

void PingTask::handle() {
	for (ObserverSetMap::iterator i = _observers.begin(); i != _observers.end(); ++i) {
		for (ObserverSet::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			try {
				(*j)->ice_ping();
				MCE_DEBUG("PingTask::handle success -> " << i->first << " -> " << (*j)->ice_toString());
				continue;
			} catch (Ice::Exception& e) {
				MCE_WARN("PingTask::handle exception -> " << i->first << " -> " << (*j)->ice_toString() << " -> " << e);
			} catch (std::exception& e) {
				MCE_WARN("PingTask::handle exception -> " << i->first << " -> " << (*j)->ice_toString() << " -> " << e.what());
			} catch (...) {
				MCE_WARN("PingTask::handle exception -> " << i->first << " -> " << (*j)->ice_toString() << " unknown");
			}
			_descriptor.unsubscribe(i->first, *j);
		}
	}
}
