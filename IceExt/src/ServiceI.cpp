// **********************************************************************
//
// Copyright (c) 2006-2007 CIAC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// MCE_LICENSE file included in this distribution.
//
// **********************************************************************
#include "ServiceI.h"
#include "TaskManager.h"
#include <sys/stat.h>
#include "util/cpp/String.h"
#include "Date.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "XceStormAdapter.h"
#include "ConnectionHolder.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::storm;
using namespace xce::svcapi;

extern "C" {
SERVICE_I_API ::IceBox::Service* create(Ice::CommunicatorPtr communicator) {
	return &ServiceI::instance();
}
}

void dump(int signo) {
	void *array[50];
	size_t size;
	char **strings;
	size_t i;

	size = backtrace(array, 50);
	strings = backtrace_symbols(array, size);

	MCE_FATAL("Obtained stack frames: " << size);

	for (i = 0; i < size; i++) {
		MCE_FATAL(strings[i]);
	}
	free(strings);

	abort();
}

//---------------------------------------------------------------------------
void ServiceI::start(const std::string& name,
		const Ice::CommunicatorPtr& communicator, const Ice::StringSeq& args) {
	_name = name;
	_args = args;
	_business = _name;
	while (!_business.empty()) {
		size_t last = _business.size() - 1;
		char c = _business.at(last);
		if ((c < '0' || c > '9') && c != '-') {
			break;
		}
		_business.erase(last);
	}

	//signal(SIGSEGV, &dump);

	Ice::PropertiesPtr props = communicator->getProperties();
	bool newLogger = props->getPropertyWithDefault("Ice.LoggerPlugin", "")
			== "" ? false : true;
	if (newLogger) {
		_ic = communicator;
		_logger = _ic->getLogger();
	} else {
		string path = props->getPropertyWithDefault("Service." + name
				+ ".LogPath", "../log/" + name);
		string level = props->getPropertyWithDefault("Service." + name
				+ ".LogLevel", "INFO");
		mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		_logger = new IceLogger(path + "/oce_log", level);
		Ice::InitializationData id;
		id.logger = _logger;
		id.properties = props;
		id.stats = communicator->getStats();
		_ic = Ice::initialize(id);
	}

	try {
		_adapter = _ic->createObjectAdapter(name);

		_cache = new ObjectCacheI(_adapter);

		_task_api = new TaskManagerAPII(_adapter);

		configure(_ic->getProperties());

		ConnectionPoolManager::instance().initialize();

		MyUtil::initialize();

		registerXceObserver(new Descriptor);
	} catch (MceException& e) {
		MCE_WARN("ServiceI::start -> " << e);
		throw e;
	} catch (Ice::Exception& e) {
		MCE_WARN("ServiceI::start -> " << e);
		throw e;
	} catch (std::exception& e) {
		MCE_WARN("ServiceI::start -> " << e.what());
		throw e;
	}

	_adapter->activate();

	MCE_FATAL("ServiceI::start -> " << _name << " started");
}

void ServiceI::stop() {
	_adapter->deactivate();
	MCE_FATAL("ServiceI::start -> " << _name << " stopped");
}

void ServiceI::registerObjectCache(Ice::Int category, const string& alias,
		const ServantFactoryPtr& factory,bool useEvictor) {
	_cache->registerCategory(category, alias, factory, _ic->getProperties(),useEvictor);
}

void ServiceI::registerXceObserver(const DescriptorPtr& descriptor) {
	if (!_descriptor) {
		_descriptor = descriptor;
		TaskManager::instance().schedule(new DescriptorLoader(_descriptor));
	}
}

void ServiceI::configure(const Ice::PropertiesPtr& props) {
	string level = props->getProperty("Service." + _name + ".LogLevel");
	if (!level.empty()) {
		MCE_LEVEL(level);
	}

	TaskManager::instance().config(props, "Normal", TASK_LEVEL_NORMAL, ThreadPoolConfig(0, 100));
	TaskManager::instance().config(props, "Create", TASK_LEVEL_CREATE, ThreadPoolConfig(0, 100));
	TaskManager::instance().config(props, "DelayCreate", TASK_LEVEL_DELAY_CREATE, ThreadPoolConfig(0, 100));
	TaskManager::instance().config(props, "Query", TASK_LEVEL_QUERY, ThreadPoolConfig(0, 10));
	TaskManager::instance().config(props, "Preload", TASK_LEVEL_PRELOAD, ThreadPoolConfig(0, 10));
	TaskManager::instance().config(props, "Reload", TASK_LEVEL_RELOAD, ThreadPoolConfig(0, 1));
	TaskManager::instance().config(props, "ParallelGet", TASK_LEVEL_PARALLEL_GET, ThreadPoolConfig(0, 200));
	TaskManager::instance().config(props, "Timer", TASK_LEVEL_TIMER, ThreadPoolConfig(0, 100));

	_cache->configure(props);
}

bool Descriptor::update(const Str2StrMap& context, const Ice::Current&) {
	if (changed(context)) {
		_context = context;
//		Ice::PropertiesPtr props = ServiceI::instance().getCommunicator()->getProperties()->clone();
		Ice::PropertiesPtr props = ServiceI::instance().getCommunicator()->getProperties();
		for (Str2StrMap::const_iterator it = context.begin(); it
				!= context.end(); ++it) {
			props->setProperty(it->first, it->second);
			MCE_INFO("Descriptor::update -> " << it->first << " = " << it->second);
		}
		ServiceI::instance().configure(props);
		configure(props);
		MCE_INFO("Descriptor::update -> changed");
		return true;
	} else {
		MCE_INFO("Descriptor::update -> unchanged");
		return false;
	}
}

string Descriptor::md5sum(const Str2StrMap& context) {
	std::ostringstream oss;
	for (MyUtil::Str2StrMap::const_iterator it = context.begin(); it
			!= context.end(); ++it) {
		oss << it->first << it->second;
	}
	return MyUtil::StrUtil::md5(oss.str());
}

bool Descriptor::changed(const Str2StrMap& context) {
	if (context.empty()) {
		return false;
	}
	return md5sum(_context) != md5sum(context);
}

void DescriptorLoader::handle() {
	string business = ServiceI::instance().getBusiness();
	try {
		XceStormAdapter::instance().subscribe(business, _descriptor);
	} catch(Ice::Exception& e) {
		MCE_WARN("ServiceI::registerXceObserver -->" << e);
	}
	Str2StrMap context;
	{
	ConnectionHolder conn("xce_descriptor",CDbWServer," ");
	mysqlpp::Query query = conn.query();
	//Statement sql;
	query << "SELECT b.* FROM descriptor_service a, descriptor_property b "
		" WHERE a.id = b.service_id and a.name=" << mysqlpp::quote << business;

	
#ifndef NEWARCH
	//mysqlpp::Result res = QueryRunner("xce_descriptor").store(sql.str());
	mysqlpp::Result res = query.store();
	for (size_t i = 0; i < res.size(); ++i) {
		mysqlpp::Row row = res.at(i);
		context[row["key"].get_string()] = row["value"].get_string();
	}
#else
	//mysqlpp::StoreQueryResult res = QueryRunner("xce_descriptor").store(sql);
	mysqlpp::StoreQueryResult res = query.store();
	for (size_t i = 0; i < res.size(); ++i) {
		mysqlpp::Row row = res.at(i);
		context[row["key"].data()] = row["value"].c_str();
	}
#endif
	}
	_descriptor->update(context);
}
