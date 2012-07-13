#include "socialgraph/autogroup/server/autogroupworkerI.h"

#include "util/cpp/TimeCost.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

void MyUtil::initialize() {
  using namespace xce::serverstate;
  using namespace xce::socialgraph;

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AutoGroupWorkerI::instance(), service.createIdentity(CATEGORY, ""));

	TaskManager::instance().config(LEVEL_GROUPING, ThreadPoolConfig(1, 40));
	TaskManager::instance().config(LEVEL_REMOVE, ThreadPoolConfig(1, 20));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("AutoGroupWorker.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("AutoGroupWorker.Interval", 5);

  //register in controller
  ServerStateSubscriber::instance().initialize("ControllerAutoGroupWorker", &AutoGroupWorkerI::instance(), mod, interval);
	ServerStateSubscriber::instance().setStatus(1);

	//register in zookeeper
	AutoGroupWorkerI::instance().Register(CATEGORY, "ControllerAutoGroupWorker");
}

//---------------------------------------------------------------------------------------------------------------------------

namespace xce {
namespace socialgraph {

void AutoGroupWorkerI::Report(const Info& information, const Ice::Current& current) {
	MCE_INFO("[AutoGroupWorkerI] Report operation(" << information.operation << ") hostid(" 
			<< information.hostId << ") type(" << information.type << ") key(" << information.key
			<< ") friendids size(" << information.friendIds.size() << ")");

	switch (information.operation) {
		case GET:	
			{
				std::vector<unsigned char> data;
				std::string key = xce::GET_KEY(CATEGORY, information.hostId);
				MemcachedAdapter::GetInstance().Get(key, data);

				if (data.empty()) {
					MyUtil::TaskManager::instance().execute(new AutoGroupingTask(information.hostId));
				} else {
					Items autogroup_item;
					autogroup_item.ParseFromArray(data.data(), data.size());
					if ((time(NULL) - autogroup_item.timestamp()) >= REBUILDINTERVAL) {
						MyUtil::TaskManager::instance().execute(new AutoGroupingTask(information.hostId));
					}
				}
				break;
			}
		case DELETE: break;
		case UNDO: break;
		case CREATE: 
			{
				Rebuild(information.hostId, Ice::Current());
			}
		case APPLY: break;
		case ACCEPT: break;
		case IGNORE: break;
		case DENY: break;
		case UPDATE:
			{
				MyUtil::TaskManager::instance().execute(new AutoGroupingTask(information.hostId));
				break;
			} 
		case ADD: break;
		default: break;
	}
}

void AutoGroupWorkerI::PreLoad(int userid, bool signal, const Ice::Current& current) {
	MCE_INFO("[AutoGroupWorkerI] PreLoad userid(" << userid << ")");
}

void AutoGroupWorkerI::Rebuild(int userid, const Ice::Current& current) {
  Executer executer;
  executer.SetAutoGroupInstance(MethodFactory::GetInstance().Create(NewGroupingWithLabel));		//使用并行取用户信息的方法打tag
  executer.Grouping(userid);

	Items result;
	executer.GetGroupResult(result);
	MCE_INFO("[AutoGroupWorkerI] Rebuild userid(" << userid << ") result size(" << result.items_size() << ")");

	if (0 < result.items_size()) {
		std::vector<unsigned char> data;
		int byte_size = result.ByteSize();
		data.resize(byte_size);
		result.SerializeToArray(data.data(), byte_size);

		std::string key = xce::GET_KEY(CATEGORY, userid);
		MemcachedAdapter::GetInstance().Set(key, data);
	}
}

Info AutoGroupWorkerI::GetInfo(int userid, const Ice::Current& current) {
	return Info();
}

//--------------------------------------------------------------------------------

void AutoGroupingTask::handle() {
	std::ostringstream oss;
	oss << "[AutoGroupingTask] handle userid " << userid_; 
	MyUtil::TimeCost tc = MyUtil::TimeCost::create(oss.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);

  Executer executer;
  executer.SetAutoGroupInstance(MethodFactory::GetInstance().Create(NewGroupingWithLabel));		//使用并行取用户信息的方法打tag
  executer.Grouping(userid_);

	Items result;
	executer.GetGroupResult(result);
	MCE_INFO("[AutoGroupingTask] handle userid_(" << userid_ << ") result size(" << result.items_size() << ")");

	if (0 < result.items_size()) {
		std::vector<unsigned char> data;
		int byte_size = result.ByteSize();
		data.resize(byte_size);
		result.SerializeToArray(data.data(), byte_size);

		std::string key = xce::GET_KEY(CATEGORY, userid_);
		MemcachedAdapter::GetInstance().Set(key, data);
		//Display(result); 	//展示
	}
}

void AutoGroupingTask::Display(Items result) {
	MCE_INFO("result items_size : " << result.items_size());
	MCE_INFO("result items timestamp : " << result.timestamp());
	const ::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >& item_list =
		result.items();
	for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator iter = item_list.begin();
			iter != item_list.end(); ++iter) {
		std::ostringstream labels;
		for (::google::protobuf::RepeatedPtrField< ::std::string>::const_iterator it = iter->messages().begin();
				it != iter->messages().end(); ++it) {
			labels << *it << " ";
		}
		MCE_INFO("group name : " << labels.str());

		const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >& idlist =
			iter->fields();
		std::ostringstream oss;
		for (::google::protobuf::RepeatedField< ::google::protobuf::int32 >::const_iterator it = idlist.begin();
				it != idlist.end(); ++it) {
			oss << *it << " ";
		}
		MCE_INFO(oss.str());
		MCE_INFO("");
	}
}

//--------------------------------------------------------------------------------

void RemoveTask::handle() {
	MCE_INFO("[RemoveTask] handle userid_(" << userid_ << ")");
}

}
}
