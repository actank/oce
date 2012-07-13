#include "socialgraph/autogroup/server2/autogroupI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

void MyUtil::initialize() {
  using namespace xce::serverstate;
  using namespace xce::socialgraph;

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AutoGroupI::instance(), service.createIdentity(CATEGORY, ""));

	TaskManager::instance().config(LEVEL_GROUPING, ThreadPoolConfig(1, 40));
	TaskManager::instance().config(LEVEL_REMOVE, ThreadPoolConfig(1, 20));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("AutoGroup.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("AutoGroup.Interval", 5);

  //register in controller
  ServerStateSubscriber::instance().initialize("ControllerAutoGroup", &AutoGroupI::instance(), mod, interval);
	ServerStateSubscriber::instance().setStatus(1);
}

//---------------------------------------------------------------------------------------------------------------------------

namespace xce {
namespace socialgraph {

Info AutoGroupI::GetInfo(int userid, const Ice::Current& current) {
	std::ostringstream oss;
	oss << "[AutoGroupI] GetInfo userid(" << userid << ")";
	MyUtil::TimeCost tc = MyUtil::TimeCost::create(oss.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);

	Info information;
	information.hostId = userid;
	information.operation = GET;
	information.type = "FFAG";
	information.key = "";
	information.detail = MyUtil::Str2StrMap();
	information.friendIds = MyUtil::IntSeq();
	information.data = MyUtil::ByteSeq();

	try {
  	Executer executer;
  	executer.SetAutoGroupInstance(MethodFactory::GetInstance().Create(GroupingWithExistGroup));
  	executer.Grouping(userid);

		Items result;
		executer.GetGroupResult(result);
		MCE_INFO("[AutoGroupI] GetInfo userid(" << userid << ") result_size(" << result.items_size() << ")");

		if (0 < result.items_size()) {
			std::vector<unsigned char> data;
			int byte_size = result.ByteSize();
			data.resize(byte_size);
			result.SerializeToArray(data.data(), byte_size);
			
			information.data.insert(information.data.end(), data.begin(), data.end());
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("[AutoGroupI] ice exception " << e.what());
	}	catch (...) {
		MCE_WARN("[AutoGroupI] unknow exception");
	}

	return information;
}

//--------------------------------------------------------------------------------

void AutoGroupingTask::handle() {
  Executer executer;
  executer.SetAutoGroupInstance(MethodFactory::GetInstance().Create(GroupingWithExistGroup));
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
		MemcachedClient::GetInstance().SetByDistCache(key, data);
	}
}

void AutoGroupingTask::Display(Items result) {
	MCE_INFO("result items_size : " << result.items_size());
	MCE_INFO("result items timestamp : " << result.timestamp());
	const ::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >& item_list =
		result.items();
	for (::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >::const_iterator iter = item_list.begin();
			iter != item_list.end(); ++iter) {
		MCE_INFO("group name : " << iter->message());
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
