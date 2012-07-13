#include "socialgraph/socialgraphutil/socialgraphworkerI.h"

using namespace xce::socialgraph;

void SocialGraphWorkerI::Register(const std::string& type, const std::string& endpoint) {
	MCE_INFO("[SocialGraphWorkerI] Register type(" << type << ") endpoint(" << endpoint << ")");

	if (ZooKeeperController::GetInstance().IsHasType(type)) {
		MCE_FATAL("[SocialGraphWorkerI] Register type has existed!");
		return;
	}
	
	ZooKeeperController::GetInstance().SetZnode(type, endpoint);
}

//-----------------------------------------------------------------------------------------------------

static ZooKeeperController* zkc = 0;

static void InitZooKeeperController() {
	zkc = new ZooKeeperController();
}

ZooKeeperController& ZooKeeperController::GetInstance() {
	MCE_INFO("[ZooKeeperController] GetInstance");
    static XCE_DECLARE_ONCE(once_guard_);
    XceOnceInit(&once_guard_, &InitZooKeeperController);
    return *zkc;
}

ZooKeeperController::ZooKeeperController() {
	MCE_INFO("[ZooKeeperController] ZooKeeperController");
    static XCE_DECLARE_ONCE(once_guard_);
    xce::ZooConfig::GetInstance().Attach(this);
    Init();
}

ZooKeeperController::~ZooKeeperController() {
	xce::ZooConfig::GetInstance().Detach(this);
}

void ZooKeeperController::Init() {
	MCE_INFO("[ZooKeeperController] Init");
    static XCE_DECLARE_ONCE(once_guard_);
    std::string value;
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
	MCE_INFO("[ZooKeeperController] Update key(" << key << ") value(" << value << ")");
    if (SG_ZNODE == key) {
        UpdateTypeMap(key, value);
    }
}

void ZooKeeperController::UpdateTypeMap(const std::string& key, const std::string& value) {
	MCE_INFO("[ZooKeeperController] UpdateTypeMap key(" << key << ") value(" << value << ")");
    IceUtil::RWRecMutex::WLock lock(init_flag_mutex_);
	type_adapter_map_.clear();

    std::vector<std::string> raw_info;
    boost::algorithm::split(raw_info, value, boost::algorithm::is_any_of(g_token_1));

    if (!raw_info.empty()) {
        for (std::vector<std::string>::iterator iter = raw_info.begin();
                iter != raw_info.end(); ++iter) {
            std::vector<std::string> info;
      	    boost::algorithm::split(info, *iter, boost::algorithm::is_any_of(g_token_2));

            if (2 == (int)info.size()) {
                type_adapter_map_.insert(std::make_pair<std::string, std::string>(info[0], info[1]));
            }
        }
    }

	for (std::map<std::string, std::string>::iterator iter = type_adapter_map_.begin();
			iter != type_adapter_map_.end(); ++iter) {
		MCE_INFO("key : " << iter->first << "  value : " << iter->second);
	}
}

std::string ZooKeeperController::GetAdapter(const std::string& key) {
    IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
    std::string value = "";
    std::map<std::string, std::string>::iterator fIt = type_adapter_map_.find(key);

    if (fIt != type_adapter_map_.end()) {
        return fIt->second;
    }
    return value;
}

bool ZooKeeperController::IsHasType(const std::string& key) {
	MCE_INFO("[ZooKeeperController] IsHasType key(" << key << ") ");
    IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	for (std::map<std::string, std::string>::iterator iter = type_adapter_map_.begin();
			iter != type_adapter_map_.end(); ++iter) {
		MCE_INFO("map key(" << iter->first << ") key(" << iter->second << ")");
	}

    std::map<std::string, std::string>::iterator fIt = type_adapter_map_.find(key);

    if (fIt != type_adapter_map_.end()) {
        return true;
    }
    return false;
}

void ZooKeeperController::SetZnode(const std::string& key, const std::string& value) {
	MCE_INFO("[ZooKeeperController] SetZnode key(" << key << ") value(" << value << ")");
	if (IsHasType(key)) {
		return;
	}

	std::string content = "";
	if (type_adapter_map_.empty()) {
		MCE_INFO("[ZooKeeperController] SetZnode type_adapter_map_ empty!"); 
		content = key + g_token_2 + value;
	} else {
		MCE_INFO("[ZooKeeperController] SetZnode type_adapter_map_ no empty!"); 
    	xce::ZooConfig::GetInstance().Get(SG_ZNODE, &content);
		content += g_token_1 + key + g_token_2 + value; 
	}
	MCE_INFO("SG_ZNODE : " << SG_ZNODE << "   content : " << content);	

	if (!content.empty()) {
		xce::ZooConfig::GetInstance().Set(SG_ZNODE, &content);
	}
}
