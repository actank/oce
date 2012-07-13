#include "socialgraph/homereconnect/writereconnect/server/writereconnectworkerI.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/IntSeqSerialize.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

void MyUtil::initialize() {
	using namespace xce::serverstate;
	using namespace MyUtil;
	using namespace com::xiaonei::xce;
	using namespace xce::socialgraph;

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&WriteReconnectWorkerI::instance(),
      service.createIdentity("FFWR", ""));
  
  TaskManager::instance().config(LEVEL_REMOVE, ThreadPoolConfig(1, 40));

  ConnectionPoolManager::instance().initialize();

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("WriteReconnectWorker.Mod");
  int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("WriteReconnectWorker.Interval", 5);

  //向controller注册
  ServerStateSubscriber::instance().initialize("ControllerWriteReconnectWorker", &WriteReconnectWorkerI::instance(), mod, sInterval);
  ServerStateSubscriber::instance().setStatus(1); 

  //注册zookeeper
	WriteReconnectWorkerI::instance().Register("FFWR", "ControllerWriteReconnectWorker");
}

//---------------------------------------------------------------------------------------------------------------

namespace xce {
namespace socialgraph {

void WriteReconnectWorkerI::Report(const Info& information, const Ice::Current& current) {
	MCE_INFO("[WriteReconnectWorkerI] DistCache Report operation(" << information.operation << ") hostId(" << information.hostId
  << ") type(" << information.type << ") key(" << information.key << ") friendIds size("
  << information.friendIds.size() << ")");

	if (information.operation == DELETE) {
    if (information.friendIds.empty()) {
      MCE_WARN("[WriteReconnectWorkerI] Report DELETE information.friendIds empty");
      return;
    }

    for (MyUtil::IntSeq::const_iterator iter = information.friendIds.begin();
        iter != information.friendIds.end(); ++iter) {
      MyUtil::TaskManager::instance().execute(new RemoveTask(information.hostId, *iter, MyUtil::IntSeq()));
    }
  }
}

void WriteReconnectWorkerI::PreLoad(int userId, bool signal, const Ice::Current& current) {
	MCE_INFO("[WriteReconnectWorkerI] PreLoad userId(" << userId << ") signal(" << signal << ")");
}

void WriteReconnectWorkerI::Rebuild(int userId, const Ice::Current& current) {
	MCE_INFO("[WriteReconnectWorkerI] Rebuild userId(" << userId << ")");
}

//---------------------------------------------------------------------------------------------------------------
//删除操作
void RemoveTask::handle() {
	using namespace com::xiaonei::xce;
	MCE_INFO("[RemoveTask] handle userId(" << _userId << ") friendId(" 
						<< _friendId << ") blockIds size(" << _blockIds.size() << ")");
  try {
  	Statement sqlWithBlock;
  	sqlWithBlock << "insert into " << TableName::recommend_block_table(_userId)
  	    << " (owner_id, friend_id, block_ids) values (" << _userId << ", "
  	    << _friendId << ", " << mysqlpp::quote << MyUtil::IntSeqSerialize::serialize(_blockIds) << ")";
  	sqlWithBlock << " on duplicate key update " << "block_ids=values(block_ids);";
  	QueryRunner("friendfinder_activeuser", CDbWServer,
  	  TableName::recommend_block_table(_userId)).execute(sqlWithBlock);
		
		RemoveDistCache(); 			//移除cache信息
  } catch ( Ice::Exception& e ) {
    MCE_WARN("[RemoveTask] handle userId(" << _userId << ") ice exception " << e.what());
  } catch ( std::exception& e) {
    MCE_WARN("[RemoveTask] handle userId(" << _userId << ")  std exception " << e.what());
  } catch ( ... ) {
    MCE_WARN("[RemoveTask] handle userId(" << _userId << ") unknow exception");
  }
}
//清除distcache中的信息
void RemoveTask::RemoveDistCache() {
	MCE_DEBUG("[RemoveTask] RemoveDistCache userId(" << _userId << ") friendId(" 
						<< _friendId << ") blockIds size(" << _blockIds.size() << ")");
	std::ostringstream mckey;
	mckey << "FFWR_" << _userId;

	std::vector<unsigned char> data;
	MemcachedClient::GetInstance().GetByDistCache(mckey.str(), data);
	Items items;
	items.ParseFromArray(data.data(), data.size());
	
	::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >* item_seq =
			items.mutable_items();
	int item_size = item_seq->size();

	Items result;
	for (int index = 0; index < item_size; ++index) {
		::xce::socialgraph::Item* tmp_item = item_seq->Mutable(index);
		if (_friendId == tmp_item->id()) {
			continue;
		}

		Item* result_item = result.add_items();
		result_item->Swap(tmp_item);
	}

	std::vector<unsigned char> binary_data;
	int byte_size = result.ByteSize();
	binary_data.resize(byte_size);
	result.SerializeToArray(binary_data.data(), byte_size);
	MemcachedClient::GetInstance().SetByDistCache(mckey.str(), binary_data);
}

}
}
