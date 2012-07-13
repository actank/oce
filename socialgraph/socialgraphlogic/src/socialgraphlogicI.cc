#include "socialgraph/socialgraphlogic/src/socialgraphlogicI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

void MyUtil::initialize() {
	using namespace xce::serverstate;
	using namespace xce::socialgraph;

	ServiceI& service = ServiceI::instance();
	
	service.getAdapter()->add(&SocialGraphLogicI::instance(),
		service.createIdentity("SGL", ""));

	TaskManager::instance().config(TASK_LEVEL_DISTRIBUTE, ThreadPoolConfig(1, 10));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SocialGraphLogic.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SocialGraphLogic.Interval", 5);

	//register in controller
	ServerStateSubscriber::instance().initialize("ControllerSocialGraphLogic", &SocialGraphLogicI::instance(), mod, interval);
	ServerStateSubscriber::instance().setStatus(1);
}

//---------------------------------------------------------------------------------------------------------------------------

namespace xce {
namespace socialgraph {

static ZooKeeperController* zkc = NULL;
static void InitZooKeeperController() {
	zkc = new ZooKeeperController();
}

ZooKeeperController& ZooKeeperController::GetInstance() {
	static XCE_DECLARE_ONCE(once_guard_);
	XceOnceInit(&once_guard_, &InitZooKeeperController);
	return *zkc;
}

ZooKeeperController::ZooKeeperController() {
	xce::ZooConfig::GetInstance().Attach(this);
	Init();
}

ZooKeeperController::~ZooKeeperController() {
	xce::ZooConfig::GetInstance().Detach(this);
}

void ZooKeeperController::Init() {
	string value;
  int limit = 10;

  while (!xce::ZooConfig::GetInstance().Get(SG_ZNODE, &value)) {
    MCE_FATAL("Not get SG_ZNODE value!! limit(" << limit << ")");
    if (--limit <= 0) {
      break;
    }
  }

	if (!value.empty()) {
		UpdateTypeMap(SG_ZNODE, value);
	}
}

void ZooKeeperController::Update(const std::string& key, const std::string& value) {
	if (SG_ZNODE == key) {
		UpdateTypeMap(key, value);
	}
}

//更新本地map，zk信息
void ZooKeeperController::UpdateTypeMap(const std::string& key, const std::string& value) {
	MCE_INFO("[ZooKeeperController] key(" << key << ") value( " << value << ")");
	IceUtil::RWRecMutex::WLock lock(init_flag_mutex_);

	type_adapter_map_.clear();
	std::vector<std::string> raw_info; 
	boost::algorithm::split(raw_info, value, boost::algorithm::is_any_of(g_token_1) );

	if (!raw_info.empty()) {
		BOOST_FOREACH (std::string item, raw_info) {
			std::vector<std::string> info;
			boost::algorithm::split(info, item, boost::algorithm::is_any_of(g_token_2));

			if (2 == (int)info.size()) {
				type_adapter_map_.insert(std::make_pair<std::string, std::string>(info[0], info[1]));
			}
		}
	}
}

//获取adapter的endpoint
std::string ZooKeeperController::GetAdapter(const std::string& key) const {
	IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	std::string value = "";
	std::map<std::string, std::string>::const_iterator fIt = type_adapter_map_.find(key);
	
	if (fIt != type_adapter_map_.end()) {
		return fIt->second;
	}
	return value;
}

//---------------------------------------------------------------------------------------------------------------------------

SocialGraphLogicI::~SocialGraphLogicI() {
}

void SocialGraphLogicI::Report(const Message& msg, const Ice::Current& current) {
	MCE_INFO("[SocialGraphLogicI] Report [" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "]");
	MyUtil::TaskManager::instance().execute(new DistributeTask(msg));
}

//---------------------------------------------------------------------------------------------------------------------------

void DistributeTask::handle() {
	for (Str2InfoMap::const_iterator iter = msg_.content.begin();
			iter != msg_.content.end(); ++iter) {
		const std::string& adapter = ZooKeeperController::GetInstance().GetAdapter(iter->first);
		if (!adapter.empty()) {
			Notify(iter->first, adapter, iter->second);
		}
	}
}

//通知具体worker
void DistributeTask::Notify(const std::string& type, const std::string& adapter, const Info& info) {
	MCE_INFO("[DistributeTask] type(" << type << ") adapter(" << adapter << ") info.hostId(" << info.hostId << ")");
	try {
		SocialGraphWorkerAdapter::instance().Report(adapter, info.hostId, info);
	} catch (Ice::Exception e) {
		MCE_WARN("[DistributeTask] type(" << type << ") adapter(" << adapter << ") found Ice::Exception : " << e.what());
	} catch (std::exception e) {
		MCE_WARN("[DistributeTask] type(" << type << ") adapter(" << adapter << ") found std::Exception : " << e.what());
	} catch (...) {
		MCE_WARN("[DistributeTask] type(" << type << ") adapter(" << adapter << ") found unknown::Exception");
	}
}

}
}
