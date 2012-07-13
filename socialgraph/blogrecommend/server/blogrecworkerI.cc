#include "socialgraph/blogrecommend/server/blogrecworkerI.h"

#include <boost/lexical_cast.hpp>

#include "util/cpp/TimeCost.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "socialgraph/blogrecommend/algorithm/executer.h"
#include "socialgraph/blogrecommend/algorithm/methodfactory.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"

void MyUtil::initialize() {
	using namespace xce::serverstate;
	using namespace xce::socialgraph;

	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BlogrecWorkerI::instance(), service.createIdentity(BLOG, ""));

	TaskManager::instance().config(LEVEL_CREATE, ThreadPoolConfig(1, 40));
	TaskManager::instance().config(LEVEL_REMOVE, ThreadPoolConfig(1, 20));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogrecWorker.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BlogrecWorker.Interval", 5);

	//register in controller
	ServerStateSubscriber::instance().initialize("ControllerBlogrecWorker", &BlogrecWorkerI::instance(), mod, interval, new MyUtil::SocialGraphChannel);
	ServerStateSubscriber::instance().setStatus(1);

	//register in zookeeper
	//BlogrecWorkerI::instance().Register(BLOG, "ControllerBlogrecWorker");
}

//--------------------------------------------------------------

namespace xce {
namespace socialgraph {

void BlogrecWorkerI::Report(const Info& information, const Ice::Current& current) {
	MCE_INFO("[BlogrecWorkerI] Report operation(" << information.operation << ") hostid("
			<< information.hostId << ") type(" << information.type << ") key(" << information.key
			<< ") friends size(" << information.friendIds.size() << ")");
	switch (information.operation) {
		case GET: break;
		case DELETE: break;
		case UNDO: break;
		case CREATE:
			{
				try {
					long blogid = boost::lexical_cast<long>(information.key);
					MyUtil::TaskManager::instance().execute(new ProcessTask(blogid));
				} catch (Ice::Exception& e) {
					MCE_WARN("[BlogrecWorkerI] Report operation(" << information.operation << ") key(" 
							<< information.key << ") Ice::Exception " << e.what());
				} catch (boost::bad_lexical_cast& e) {
					MCE_WARN("[BlogrecWorkerI] Report operation(" << information.operation << ") key(" 
							<< information.key << ") boost::bad_lexical_cast " << e.what());
				} catch (...) {
					MCE_WARN("[BlogrecWorkerI] Report operation(" << information.operation << ") key(" 
							<< information.key << ") unknown exception");
				}
				break;
			}
		case APPLY: break;
		case ACCEPT: break;
		case IGNORE: break;
		case DENY: break;
		case UPDATE: break;
		case ADD: break;
		default: break;
	}
}

void BlogrecWorkerI::PreLoad(int userid, bool signal, const Ice::Current& current) {
	MCE_INFO("[BlogrecWorkerI] PreLoad userid(" << userid << ")");
}

void BlogrecWorkerI::Rebuild(int userid, const Ice::Current& current) {
	MCE_INFO("[BlogrecWorkerI] Rebuild userid(" << userid << ")");
}

//--------------------------------------------------------------

void ProcessTask::handle() {
	MCE_INFO("[Process] handle blogid_ : " << blogid_);
	Executer executer;
	executer.SetInstance(MethodFactory::GetInstance().Create(Participle));
	executer.Process(blogid_);
}

}
}
