#include "socialgraph/friendfinderworker/server/friendfinderworkerI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "util/cpp/InvokeMonitor.h"
#include "util/cpp/TimeCost.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "socialgraph/friendfinderworker/util/commondbhelper.h"
#include "socialgraph/friendfinderworker/util/createcommondatahelper.h"

void MyUtil::initialize() {
	using namespace xce::serverstate;
	using namespace MyUtil;
	using namespace com::xiaonei::xce;
	using namespace xce::socialgraph;

	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FriendFinderWorkerI::instance(),
			service.createIdentity("FFW", ""));

	TaskManager::instance().config(LEVEL_LOAD, ThreadPoolConfig(1, 20));
	TaskManager::instance().config(LEVEL_REBUILD_CREATE, ThreadPoolConfig(1, 20));
	TaskManager::instance().config(LEVEL_REBUILD_NORMAL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(LEVEL_REMOVE, ThreadPoolConfig(1, 40));

	TaskManager::instance().config(LEVEL_BATCH_REBUILD, ThreadPoolConfig(1, 1));

	ConnectionPoolManager::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderWorker.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderWorker.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerFriendFinderWorker", &FriendFinderWorkerI::instance(), mod, sInterval);
	ServerStateSubscriber::instance().setStatus(1);

	//注册zookeeper
	FriendFinderWorkerI::instance().Register("FFW", "ControllerFriendFinderWorker");

	//启动监控
	ServiceMonitorManager::instance().start();
}

//---------------------------------------------------------------------------------------------------------------

namespace xce {
namespace socialgraph {

void FriendFinderWorkerI::Report(const Info& information, const Ice::Current& current) {
	std::ostringstream oss;
	oss << "[FriendFinderWorker] Report operation(" << information.operation << ") hostId(" << information.hostId
	 		<< ") type(" << information.type << ") key(" << information.key << ") friendIds size(" 
			<< information.friendIds.size() << ") data(" << information.data.size() << ")";

	if (information.operation == DELETE) {
		if (information.friendIds.empty()) {
			MCE_WARN("[FriendFinderWorker] Report DELETE information.friendIds empty");	
			return;
		}
		for (MyUtil::IntSeq::const_iterator iter = information.friendIds.begin();
				iter != information.friendIds.end(); ++iter) {
			MyUtil::TaskManager::instance().execute(new CommonFriendRemoveTask(information.hostId, *iter));
		}
	} else if (information.operation == UNDO) {
		::xce::socialgraph::Items puk_object;
		puk_object.ParseFromArray(information.data.data(), information.data.size());
		MCE_INFO("[FriendFinderWorker] Report UNDO pukObject.items_size : " << puk_object.items_size() 
				<< " hostId(" << information.hostId << ") information.data.size(" << information.data.size() 
				<< ")");

		if (puk_object.items_size() < CF_LOWERBOUNDARY) {
			MyUtil::TaskManager::instance().execute(new RebuildTask(information.hostId, true, LEVEL_REBUILD_CREATE)); 
		} else if ((time(NULL) - puk_object.timestamp()) >= rebuildInterval) {
			SetRebuildList(information.hostId);     
		}
		CheckRebuildTime();     
	}  else if (information.operation == UPDATE) {
		MyUtil::TaskManager::instance().execute(new RebuildTask(information.hostId, false, LEVEL_REBUILD_CREATE));	//rebuild user's data
	}
}

void FriendFinderWorkerI::Rebuild(int userId, const Ice::Current& current) {
	vector<unsigned char> binary_data;
	CreateCommonDataHelper::CalCommonFriends(userId, binary_data);

	std::ostringstream oss;
	oss << "[FriendFinderWorker] Rebuild userId : " << userId << "  key : FFW_" << userId << " binary_data : " << binary_data.size();
	
	std::string key = Comb2("FFW", userId);
	MemcachedClient::GetInstance().SetByDistCache(key, binary_data);
}

void FriendFinderWorkerI::PreLoad(int userId, bool signal, const Ice::Current& current) {
	using namespace com::xiaonei::xce;

	vector<int> idseq;
	CommonDbHelper::GetRecentlyloginUserId(idseq); 		//获取最近一天登录的用户id列表

	int index = userId % 10;
	int task_size = 0;
	for (vector<int>::iterator iter = idseq.begin(); iter != idseq.end(); ++iter) {
		if (index != *iter % 10) {            //加载相应的尾号
			continue;
		}
		MyUtil::TaskManager::instance().execute(new LoadTask(*iter, signal));
		while ((task_size = MyUtil::TaskManager::instance().size(LEVEL_LOAD)) > 10000) {
			sleep(2);
		}
	}
}

void FriendFinderWorkerI::SetRebuildList(int userid) {
	IceUtil::RWRecMutex::WLock lock(monitor_);
	MCE_INFO("[FriendFinderWorker] SetRebuildList userid(" << userid << ") rebuild_list_ size(" << rebuild_list_.size() << ")");
	rebuild_list_.insert(userid);
}

void FriendFinderWorkerI::CheckRebuildTime() {
	time_t t = time(0);
	struct tm *Tm = localtime(&t);
	int hour = Tm->tm_hour;
	MCE_INFO("[FriendFinderWorkerI] CheckRebuildTime current hour : " << hour << " time_ : " << time_);
	if (hour - time_ >= 1 || (0 == hour && 23 == time_)) { 	//一小时重算一次
		finish_load_ = false;
		time_ = hour;
		BatchRebuild();
	}
}

void FriendFinderWorkerI::BatchRebuild() {
	MCE_INFO("[FriendFinderWorkerI] BatchRebuild rebuild_list_ size : " << rebuild_list_.size());
	std::set<int> id_set_;
	{
		IceUtil::RWRecMutex::WLock lock(monitor_);
		rebuild_list_.swap(id_set_);
	}
	for (set<int>::iterator iter = id_set_.begin();
			iter != id_set_.end(); ++iter) {
		MyUtil::TaskManager::instance().execute(new RebuildTask(*iter, false, LEVEL_BATCH_REBUILD));
	}
}

//---------------------------------------------------------------------------------------------------------------

void RebuildTask::handle() {
	if (flag_) {   //先从数据库里面取数据，然后重算
		vector<unsigned char> result; 
		CreateCommonDataHelper::GetFromDb(userid_, result);

		::xce::socialgraph::Items puk_object;
		puk_object.ParseFromArray(result.data(), result.size());

		MCE_INFO("[RebuildTask] handle flag=true userId(" << userid_ << ") result size(" 
					<< result.size() << ") items_size(" << puk_object.items_size() << ")");

		if (puk_object.items_size() < CF_LOWERBOUNDARY) {		//数据不够就重算
			CreateCommonDataHelper::CalCommonFriends(userid_, result);
		}

		std::string key = Comb2("FFW", userid_);
		MemcachedClient::GetInstance().SetByDistCache(key, result);
	} else {
		vector<unsigned char> result; 
		CreateCommonDataHelper::CalCommonFriends(userid_, result);

		std::string key = Comb2("FFW", userid_);		//向cache中插入数据为手机wap端提供数据
		MemcachedClient::GetInstance().SetByDistCache(key, result);
	}
}

//---------------------------------------------------------------------------------------------------------------

void CommonFriendRemoveTask::handle() {
	using namespace com::xiaonei::xce;

	MyUtil::TimeCost tc = MyUtil::TimeCost::create("[CommonFriendRemoveTask] handle", MyUtil::TimeCost::LOG_LEVEL_INFO);
	MCE_INFO("[CommonFriendRemoveTask] handle hostId(" << hostid_ << ") friendId(" << friendid_ << ")");

	CommonDbHelper::RemoveCommonsFromDb(hostid_, friendid_);		//从common_table 和 block_table 中删除对应的id

	vector<unsigned char> data;
	std::string key = Comb2("FFW", hostid_);
	MemcachedClient::GetInstance().GetByDistCache(key, data);
	::xce::socialgraph::Items puk_friends;
	puk_friends.ParseFromArray(data.data(), data.size());
	tc.step("SetDB and ParseFromArray", false);

	if (puk_friends.items_size() <= CF_REBUILD_THRESHOLD) {
		UpdateMemcachedExpand(puk_friends);
	} else {
		UpdateMemcachedNormal(puk_friends);
	}
}

//cache中有足够的数据，正常remove
void CommonFriendRemoveTask::UpdateMemcachedNormal(xce::socialgraph::Items& puk_friends) {
	MCE_INFO("[CommonFriendRemoveTask] UpdateMemcachedNormal hostid_(" << hostid_ << ") friendid_(" 
		<< friendid_ << ") puk_friends commfriends size(" << puk_friends.items_size() << ")");

	xce::socialgraph::Items result;
	::google::protobuf::RepeatedPtrField< ::xce::socialgraph::Item >* seq
		= puk_friends.mutable_items();
	int size = seq->size();
	for (int index = 0; index < size; ++index) {
		::xce::socialgraph::Item* tmp_item = seq->Mutable(index);
		if (friendid_ == tmp_item->id()) {
			continue;
		}
		::xce::socialgraph::Item* result_item = result.add_items();
		result_item->Swap(tmp_item);
	}
	result.set_timestamp(time(NULL));
	vector<unsigned char> binary_data;
	int byte_size = result.ByteSize();
	binary_data.resize(byte_size);
	result.SerializeToArray(binary_data.data(), byte_size);

	std::string key = Comb2("FFW", hostid_);
	MemcachedClient::GetInstance().SetByDistCache(key, binary_data);
}

//cache中数据不足，需要补全 
void CommonFriendRemoveTask::UpdateMemcachedExpand(xce::socialgraph::Items& puk_friends) {
	using namespace com::xiaonei::xce;

	MCE_INFO("[CommonFriendRemoveTask] UpdateMemcachedExpand hostid_(" << hostid_ << ") friendid_(" 
		<< friendid_ << ") puk_friends commfriends size(" << puk_friends.items_size() << ")");

  CommonFriendSeq comm_friends;
	CommonDbHelper::GetCommonsFromDb(hostid_, comm_friends);		//从db中取出数据

	if (comm_friends.empty()) {		//如果为0，直接重算，并返回
		MyUtil::TaskManager::instance().execute(new RebuildTask(hostid_, false, LEVEL_REBUILD_CREATE));	 
		return;
	}

	::xce::socialgraph::Items result;
	for (CommonFriendSeq::iterator iter = comm_friends.begin(); (iter != comm_friends.end()) 
			&& (iter < comm_friends.begin() + CF_SAVE_THRESHOLD_DISTCACHE); ++iter) {
		if (friendid_ == iter->userId) {     			//remove操作
			continue;
		}
		::xce::socialgraph::Item* item = result.add_items();
			
		item->set_id(iter->userId);
		item->set_field(iter->sharesCount);
		for (MyUtil::IntSeq::iterator it = iter->shares.begin();
				it != iter->shares.end(); ++it) {
			if (item->fields_size() >= CACHE_SHARES_SIZE) {
				break;
			}
			item->add_fields(*it);
		}
	}

	result.set_timestamp(time(NULL));

	vector<unsigned char> binary_data;
	int byte_size = result.ByteSize();
	binary_data.resize(byte_size);
	result.SerializeToArray(binary_data.data(), byte_size);

	std::string key = Comb2("FFW", hostid_);
	MemcachedClient::GetInstance().SetByDistCache(key, binary_data);
}

//---------------------------------------------------------------------------------------------------------------

void LoadTask::handle() {
	MCE_INFO("[LoadTask] handle userid_(" << userid_ << ")");
	vector<unsigned char> data;
	if (flag_) {         //从数据库加载
		CreateCommonDataHelper::GetFromDb(userid_, data);
	} else {
		CreateCommonDataHelper::CalCommonFriends(userid_, data);
	}

	if (!data.empty()) {
		std::string key = Comb2("FFW", userid_);
		MemcachedClient::GetInstance().SetByDistCache(key, data);
	}
}

}
}
