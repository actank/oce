/*
 * =====================================================================================
 *
 *       Filename:  FeedItemFacadeI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年10月27日 18时47分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FeedItemFacadeI.h"
#include "FacadeHelper.hpp"
#include "ServiceI.h"
#include "IceLogger.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "FeedItemCacheByUseridReplicaAdapter.h"
#include "FeedItemCacheByAppidReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedItemFacadeI::instance(), service.createIdentity("M", ""));

	//controller
	int interval = 5;
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedItemFacade.Mod", 0);
	string controller = service.getCommunicator()->getProperties()->getPropertyWithDefault("FeedItemFacade.Controller", "ControllerFeedItemFacade");
	ServerStateSubscriber::instance().initialize(controller, &FeedItemFacadeI::instance(), mod, interval, new XceFeedControllerChannel());

}

FeedItemFacadeI::FeedItemFacadeI() {
}

FeedItemFacadeI::~FeedItemFacadeI() {
}

void FeedItemFacadeI::put(const FeedSeedPtr& seed, const ::Ice::Current& current) {

	ostringstream oss;
	oss << "uid: " << seed->actor << " feedid: " << seed->feed << " type: " << seed->type << " time: " << seed->time;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	std::map<string, string> context = seed->extraProps;

	FeedMeta fm;
	fm.userid = seed->actor;
	fm.feedid = seed->feed;
	fm.type = seed->type;
	fm.time = seed->time;
	fm.weight = seed->baseWeight;
	fm.mergeid = seed->newsMerge;

	//dispatch ByUser
	try {
		FeedItemCacheByUseridReplicaAdapter::instance().put(fm);
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemFacadeI::put FeedItemCacheByUserid, feedid: " << fm.feedid << e.what());
	} catch (...) {
		MCE_WARN("FeedItemFacadeI::put FeedItemCacheByUserid, feedid: " << fm.feedid << " UNKNOWN Exception!");
	}

	if (FacadeHelper::is_exist(context, AppidContextPrefix)) {
		//dispatch ByAppid
		MCE_DEBUG("dispatch to ByAppId");
		try {
			FeedItemCacheByAppidReplicaAdapter::instance().put(fm, context);
		} catch (Ice::Exception& e) {
			MCE_WARN("FeedItemFacadeI::put FeedItemCacheByAppid, feedid: " << fm.feedid << e.what());
		} catch (...) {
			MCE_WARN("FeedItemFacadeI::put FeedItemCacheByAppid, feedid: " << fm.feedid << " UNKNOWN Exception!");
		}
	}

	//write DB.
	FacadeHelper::write_db(fm, context);
}

void FeedItemFacadeI::del(const FeedMeta& fm, const ::Ice::Current& current) {

	ostringstream oss;
	oss << "uid: " << fm.userid << " feedid: " << fm.feedid << " type: " << fm.type << " time: " << fm.time;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	//TODO into del cache.
}
