/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantLoaderI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月28日 12时25分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "BuddyByApplicantLoaderI.h"
#include "buddy/BuddyByApplicantCache/share/BuddyByApplicantFactory.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyByApplicantAdapter.h"
#include "util/cpp/TimeCost.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize(){
        ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(&BuddyByApplicantLoaderI::instance(), service.createIdentity("L", ""));

	TaskManager::instance().config(TASK_LEVEL_RELOAD, ThreadPoolConfig(1, 5));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByApplicantLoader.Mod");
        int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantLoader.Interval", 5);
        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerBuddyByApplicantLoader",
                        &BuddyByApplicantLoaderI::instance(), mod, sInterval);

}

//----------------------------------------------------------------------------------------------------------

void BuddyByApplicantLoaderI::addAccepter(Ice::Int id, const AccepterInfo& accepter, const Ice::Current& current) {
	ostringstream msg;
	msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [addAccepter] id=" << id
			                        << " accepter=" << accepter.accepter;
	TimeCost tc = TimeCost::create(msg.str());
	BuddyByApplicantCacheAdapter::instance().addAccepter(id, accepter);
}

void BuddyByApplicantLoaderI::removeAccepter(Ice::Int id, Ice::Int accepterId, const Ice::Current& current) {
	ostringstream msg;
	msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [removeAccepter] id=" << id
			                        << " accepterId=" << accepterId;
	TimeCost tc = TimeCost::create(msg.str());
	BuddyByApplicantCacheAdapter::instance().removeAccepter(id, accepterId);
}

void BuddyByApplicantLoaderI::reload(Ice::Int id, const Ice::Current& current) {
	ostringstream msg;
	msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [reload] id=" << id ;
	TimeCost tc = TimeCost::create(msg.str());
	TaskManager::instance().execute( new ReloadTask( id ) );
}

void BuddyByApplicantLoaderI::clear(Ice::Int id, const Ice::Current& current) {
	ostringstream msg;
	msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [clear] id=" << id;
	TimeCost tc = TimeCost::create(msg.str());
	BuddyByApplicantCacheAdapter::instance().clear(id);
}

//--------------------------------------------------------------------------
void ReloadTask::handle() {
	try {
		Ice::ObjectPtr data = BuddyByApplicantFactory::instance().create(_id);
		BuddyByApplicantCacheAdapter::instance().reload(_id, data);
	} catch (...) {
		MCE_WARN("[ReloadTask::handle()] " << " id=" << _id);
	}

}
