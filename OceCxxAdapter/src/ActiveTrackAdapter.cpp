#include "ActiveTrackAdapter.h"

using namespace xce::log;
using namespace com::xiaonei::service;
// using namespace xce::activetrack::adapter;

void ActiveTrack::initialize(string ctrlEndpoints) {

	// 初始化XceLogger
	_logger = XceLoggerReplicaAdapter::instance().getLogger(ctrlEndpoints);
	_logger->initialize();

	// 判断节点,设置logger级别
	_isXiaonei = true;
	string topLevelCategory = "ActiveTrackXN";
	/* if ( MyUtil::IpAddr::isXiaonei() ) {
		topLevelCategory = "ActiveTrackXN";
		_isXiaonei = true;
	}
	else if ( MyUtil::IpAddr::isKaixin() ) {
		topLevelCategory = "ActiveTrackKX";
		_isKaixin = true;
	}
	else {
		topLevelCategory = "ActiveTrack";
	} */
			
	MCE_INFO("Active Track System uses top level category: " << topLevelCategory);
	for (int i = 0; i < CLUSTER; ++i) {
		vector<string> category_action;
		vector<string> category_access;

		ostringstream bucket;
		bucket << "bucket_" << i;

		category_action.push_back(topLevelCategory);
		category_action.push_back("action");
		category_action.push_back(bucket.str());
		_actionCategories.push_back(category_action);

		category_access.push_back(topLevelCategory);
		category_access.push_back("access");
		category_access.push_back(bucket.str());
		_accessCategories.push_back(category_access);
	}
}

void ActiveTrack::initialize(LOG_TYPE log_type, string ctrlEndpoints) {

	// 初始化XceLogger
	_logger = XceLoggerReplicaAdapter::instance().getLogger(ctrlEndpoints);
	_logger->initialize();
	
	// 设置logger级别
	_isXiaonei = true;
	string topLevelCategory = "ActiveTrackXN";
	/* string topLevelCategory;
	if (log_type == XIAONEI) {
		topLevelCategory = "ActiveTrackXN";
		_isXiaonei = true;
	} else if (log_type == KAIXIN) {
		topLevelCategory = "ActiveTrackKX";
		_isKaixin = true;
	} else {
		topLevelCategory = "ActiveTrack";
	} */
	MCE_INFO("Active Track System uses top level category: " << topLevelCategory);
	for (int i = 0; i < CLUSTER; ++i) {
		vector<string> category;
		category.push_back(topLevelCategory);
		category.push_back("action");
		ostringstream bucket;
		bucket << "bucket_" << i;
		category.push_back(bucket.str());
		_actionCategories.push_back(category);
	}
}

void ActiveTrack::action(int userId, const vector<string>& paths, const string& action,	
		const string& object)  {

	ostringstream path;
	for ( vector<string>::const_iterator it = paths.begin(); it != paths.end(); it++ ) {
		path << (*it) << "_";
	}
	doAction(userId, path.str(), action, object);
        // ActiveTrackFacadeAdapter::instance().action(userId,paths,action,object);
}

void ActiveTrack::action(int userId, const string& path, const string& action,	
		const string& object)  {

	doAction(userId, path, action, object);
	// ActiveTrackFacadeAdapter::instance().action(userId,paths,action,object);
}

void ActiveTrack::doAction(int userId, const string& path, const string& action,
		const string& object) {
	ostringstream message;
	
	// 取得当前时间
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	long timestamp = (tv.tv_sec*1000) + (tv.tv_usec/1000);

	// 组合message
	message << timestamp << " ";
	message << userId << " ";
	if ( path != "" ) {
		message << path << " ";
	} else {
		message << "_" << " ";
	}
	message << action << " ";
	message << object;
	try {
		_logger->addLog(_actionCategories[userId % 10], message.str());
	} catch (...) {
		MCE_WARN("XceLogger : add log exception :" << message.str());
	}
	MCE_DEBUG("action Category : " <<  _actionCategories[userId % 10].at(0) << "/" 
			<< _actionCategories[userId % 10].at(1) << "/" << _actionCategories[userId % 10].at(2) 
			<< "\tMessage : " << message.str());
}

void ActiveTrack::access(int userId, const string& ip, const string& url,
		const string& decision) {
	ostringstream message;
	
	// 取得当前时间
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	long timestamp = (tv.tv_sec*1000) + (tv.tv_usec/1000);

	// 组合message
	message << timestamp;
	message << " " << userId;
	message << " " << ip;
	message << " " << url;
	if ( decision != "" ) {
		message << " " << decision;
	}
	try {
		_logger->addLog(_accessCategories[userId % 10], message.str());
	} catch (...) {
		MCE_WARN("XceLogger : add access log exception :" << message.str());
	}
}
