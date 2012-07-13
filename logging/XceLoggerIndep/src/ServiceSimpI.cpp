// **********************************************************************
//
// Copyright (c) 2006-2007 CIAC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// MCE_LICENSE file included in this distribution.
//
// **********************************************************************
#include "ServiceSimpI.h"
#include <sys/stat.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

using namespace MyUtil;

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

	//MCE_FATAL("Obtained stack frames: " << size);

	//for (i = 0; i < size; i++) {
		//MCE_FATAL(strings[i]);
	//}
	free(strings);

	abort();
}

//---------------------------------------------------------------------------
void ServiceI::start(const std::string& name,
		const Ice::CommunicatorPtr& communicator, const Ice::StringSeq& args) {
	_name = name;
	_args = args;
/*
	_business = _name;
	while (!_business.empty()) {
		size_t last = _business.size() - 1;
		char c = _business.at(last);
		if (c < '0' || c > '9') {
			break;
		}
		_business.erase(last);
	}
*/
	//signal(SIGSEGV, &dump);

	Ice::PropertiesPtr props = communicator->getProperties();
//	bool newLogger = props->getPropertyWithDefault("Ice.LoggerPlugin", "")
			//== "" ? false : true;
	//if (newLogger) {
	//	_ic = communicator;
		//_logger = _ic->getLogger();
	//} else {
//		string path = props->getPropertyWithDefault("Service." + name
	//			+ ".LogPath", "../log/" + name);
//		string level = props->getPropertyWithDefault("Service." + name
	//			+ ".LogLevel", "INFO");
//		mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
//		_logger = new IceLogger(path + "/oce_log", level);
	Ice::InitializationData id;
	//		id.logger = _logger;
	id.properties = props;
	//id.stats = communicator->getStats();
	_ic = Ice::initialize(id);
	//}

	try {
		_adapter = _ic->createObjectAdapter(name);
		//configure(_ic->getProperties());
		MyUtil::initialize();
	} catch (Ice::Exception& e) {
		//MCE_WARN("ServiceI::start -> " << e);
		throw e;
	} catch (std::exception& e) {
		//MCE_WARN("ServiceI::start -> " << e.what());
		throw e;
	}

	_adapter->activate();

	//MCE_FATAL("ServiceI::start -> " << _name << " started");
}

void ServiceI::stop() {
	_adapter->deactivate();
	//MCE_FATAL("ServiceI::start -> " << _name << " stopped");
}

/*
void ServiceI::configure(const Ice::PropertiesPtr& props) {
	string level = props->getProperty("Service." + _name + ".LogLevel");
	if (!level.empty()) {
		//MCE_LEVEL(level);
	}
}
*/
