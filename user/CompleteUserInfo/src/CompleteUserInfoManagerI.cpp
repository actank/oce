#include "CompleteUserInfoManagerI.h"
#include <ServiceI.h>
#include <QueryRunner.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <iostream>
#include "UserPassportAdapter.h"
#include "UserNameAdapter.h"
#include "UserConfig.h"
#include "UserDesc.h"
#include "UserLogic.h"
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "UserConfigCommon.h"
#include "TaskManager.h"
#include "ObjectCacheI.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "UserConfigAdapter.h"
#include "UserDescAdapter.h"
#include "UserLogicAdapter.h"
#include "UserBaseAdapter.h"
#include "UserRightAdapter.h"
#include "UserStateAdapter.h"
#include "UserStageAdapter.h"
#include "UserBornAdapter.h"
#include "UserUrlAdapter.h"
#include "UserTimeAdapter.h"
#include "UserContactWRAdapter.h"
#include "UserDoingAdapter.h"
#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"

using namespace xce::userbase;
using namespace xce::userdesc;
using namespace xce::adapter;
using namespace xce::adapter::userbase;
using namespace xce::adapter::userpassport;
using namespace xce::adapter::username;
using namespace xce::adapter::userright;
using namespace xce::adapter::userstate;
using namespace xce::adapter::userstage;
using namespace xce::adapter::userborn;
using namespace xce::adapter::userurl;
using namespace xce::adapter::usercontact;
using namespace xce::adapter::usertime;
using namespace xce::adapter::userdoing;
using namespace xce::completeuserinfo;
using namespace xce::serverstate;
using namespace std;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&CompleteUserInfoManagerI::instance(), service.createIdentity("COMPLETE_USER_INFO", ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("CompleteUserInfo.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("CompleteUserInfo.Interval", 5);
	MCE_INFO("[MyUtil::initialize] CompleteUserInfo.Mod = " << mod);
	CompleteUserInfoManagerI::instance().setValid(true);
	ServerStateSubscriber::instance().initialize("ControllerCompleteUserInfo", &CompleteUserInfoManagerI::instance(), mod, interval,new MyUtil::OceChannel, "3000", "3000", "1048576");
  ServiceMonitorManager::instance().start();
}

template<class TypePtr, class Type> 
bool getInfo(TypePtr (Type::*fp)(int, const Ice::Context& ctx), int user_id) {
	string type_name = typeid(Type).name();
	try {
		MCE_DEBUG("[" << type_name << ":" << "get" << type_name << "] userId = " << user_id);
		TypePtr ptr = (Type::instance().*fp)(user_id, Ice::Context());
		MCE_DEBUG("[" << type_name << ":" << "get" << type_name << "] userId = " << user_id << " success");
		return true;
	} catch(const Ice::TimeoutException& e) {
		MCE_INFO("[" << type_name << ":" << "get" << type_name << "] userId = " << user_id << " " << e.what() << endl);
		return true;
	} catch(const Ice::Exception& e) {
		string type_name = typeid(Type).name();
		MCE_INFO("[" << type_name << ":" << "get" << type_name << "] userId = " << user_id << " " << e.what() << endl);
		return false;
	} catch(const exception& e) {
		string type_name = typeid(Type).name();
		MCE_INFO("[" << type_name << ":" << "get" << type_name << "] userId = " << user_id << " " << e.what() << endl);
		return false;
	} 
}
void CompleteUserInfoManagerI::init() {
	fpIndex_.resize(CHECK_USERINFO_SIZE);
	fpIndex_[0].createfp_ = &CompleteUserInfoManagerI::createDefaultUserName;
	fpIndex_[0].getfp_ = &CompleteUserInfoManagerI::getUserName;
	fpIndex_[0].name_ = "UserName";
	fpIndex_[1].createfp_ = &CompleteUserInfoManagerI::createDefaultUserRight;
	fpIndex_[1].getfp_ = &CompleteUserInfoManagerI::getUserRight;
	fpIndex_[1].name_ = "UserRight";
	fpIndex_[2].createfp_ = &CompleteUserInfoManagerI::createDefaultUserState;
	fpIndex_[2].getfp_ = &CompleteUserInfoManagerI::getUserState;
	fpIndex_[2].name_ = "UserState";
	fpIndex_[3].createfp_ = &CompleteUserInfoManagerI::createDefaultUserUrl;
	fpIndex_[3].getfp_ = &CompleteUserInfoManagerI::getUserUrl;
	fpIndex_[3].name_ = "UserUrl";
	fpIndex_[4].createfp_ = &CompleteUserInfoManagerI::createDefaultUserBorn;
	fpIndex_[4].getfp_ = &CompleteUserInfoManagerI::getUserBorn;
	fpIndex_[4].name_ = "UserBorn";
	fpIndex_[5].createfp_ = &CompleteUserInfoManagerI::createDefaultUserStage;
	fpIndex_[5].getfp_ = &CompleteUserInfoManagerI::getUserStage;
	fpIndex_[5].name_ = "UserStage";
	fpIndex_[6].createfp_ = &CompleteUserInfoManagerI::createDefaultUserDoing;
	fpIndex_[6].getfp_ = &CompleteUserInfoManagerI::getUserDoing;
	fpIndex_[6].name_ = "UserDoing";
	fpIndex_[7].createfp_ = &CompleteUserInfoManagerI::createDefaultUserTime;
	fpIndex_[7].getfp_ = &CompleteUserInfoManagerI::getUserTime;
	fpIndex_[7].name_ = "UserTime";
	fpIndex_[8].createfp_ = &CompleteUserInfoManagerI::createDefaultUserContact;
	fpIndex_[8].getfp_ = &CompleteUserInfoManagerI::getUserContact;
	fpIndex_[8].name_ = "UserContact";
	fpIndex_[9].createfp_ = &CompleteUserInfoManagerI::createDefaultUserConfig;
	fpIndex_[9].getfp_ = &CompleteUserInfoManagerI::getUserConfig;
	fpIndex_[9].name_ = "UserConfig";
	fpIndex_[10].getfp_ = &CompleteUserInfoManagerI::getUserPassport;
}

void CompleteUserInfoManagerI::createDefaultUserName(int userId) {
	UserNameInfoPtr ptr = new UserNameInfo;
	ptr->set_id(userId);
	ptr->set_name(DEFAULT_NAME);
	UserNameAdapter::instance().createUserName(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserRight(int userId) {
	UserRightInfoPtr ptr = new UserRightInfo;
	ptr->set_id(userId);
	UserRightAdapter::instance().createUserRight(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserState(int userId) {
	UserStateInfoPtr ptr = new UserStateInfo;
	ptr->set_id(userId);
	ptr->set_star(DEFAULT_STAR);
	UserStateAdapter::instance().createUserState(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserUrl(int userId) {
	UserUrlInfoPtr ptr = new UserUrlInfo;
	ptr->set_id(userId);
	ptr->set_headUrl(DEFAULT_HEAD_URL);
	ptr->set_mainUrl(DEFAULT_MAIN_URL);
	ptr->set_tinyUrl(DEFAULT_TINY_URL);
	ptr->set_largeUrl(DEFAULT_LARGE_URL);
	UserUrlAdapter::instance().createUserUrl(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserBorn(int userId) {
	UserBornInfoPtr ptr = new UserBornInfo;
	ptr->set_id(userId);
	UserBornAdapter::instance().createUserBorn(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserStage(int userId) {
	UserStageInfoPtr ptr = new UserStageInfo;
	ptr->set_id(userId);
	ptr->set_stage(DEFAULT_STAGE);
	UserStageAdapter::instance().createUserStage(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserDoing(int userId) {
	UserDoingInfoPtr ptr = new UserDoingInfo;
	ptr->set_id(userId);
	UserDoingAdapter::instance().createUserDoing(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserTime(int userId) {
	UserTimeInfoPtr ptr = new UserTimeInfo;
	ptr->set_id(userId);
	int now_time = time(NULL);
	ptr->set_registerTime(now_time);
	ptr->set_lastLoginTime(now_time);
	ptr->set_lastLogoutTime(now_time);
	UserTimeAdapter::instance().createUserTime(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserContact(int userId) {
	UserContactInfoPtr ptr = new UserContactInfo;
	ptr->set_id(userId);
	ptr->set_msn(DEFAULT_MSN);
	UserContactWRAdapter::instance().createUserContact(userId, ptr);
}

void CompleteUserInfoManagerI::createDefaultUserConfig(int userId) {
	UserConfigInfoPtr ptr = new UserConfigInfo;
	ptr->set_userId(userId);
	ptr->set_basicConfig(DEFAULT_BASIC_CONFIG);
	ptr->set_pokeConfig(DEFAULT_POKE_CONFIG);
	ptr->set_wantSeeCss(DEFAULT_WANT_SEE_CSS);
	ptr->set_recvFeedConfig(DEFAULT_RECV_FEED_CONFIG);
	ptr->set_browseConfig(DEFAULT_BROWSE_CONFIG);
	ptr->set_requestFriendConfig(DEFAULT_REQUEST_FRIEND_CONFIG);
	ptr->set_emailConfig(DEFAULT_EMAIL_CONFIG);
	ptr->set_profilePrivacy(DEFAULT_PROFILE_PRIVACY);
	UserConfigAdapter::instance().createUserConfig(userId, ptr);
}

bool CompleteUserInfoManagerI::getUserName(int userId) {
	return getInfo(&UserNameAdapter::getUserName, userId);
}
bool CompleteUserInfoManagerI::getUserRight(int userId) {
	return getInfo(&UserRightAdapter::getUserRight, userId);
}

bool CompleteUserInfoManagerI::getUserState(int userId) {
	 return getInfo(&UserStateAdapter::getUserState, userId);
}

bool CompleteUserInfoManagerI::getUserUrl(int userId) {
	return getInfo(&UserUrlAdapter::getUserUrl, userId);
}
bool CompleteUserInfoManagerI::getUserBorn(int userId) {
	return getInfo(&UserBornAdapter::getUserBorn, userId);
}

bool CompleteUserInfoManagerI::getUserStage(int userId) {
	return getInfo(&UserStageAdapter::getUserStage, userId);
}

bool CompleteUserInfoManagerI::getUserDoing(int userId) {
	return getInfo(&UserDoingAdapter::getUserDoing, userId);
}

bool CompleteUserInfoManagerI::getUserTime(int userId) {
	return getInfo(&UserTimeAdapter::getUserTime, userId);
}

bool CompleteUserInfoManagerI::getUserContact(int userId) {
	return getInfo(&UserContactWRAdapter::getUserContact, userId);
}

bool CompleteUserInfoManagerI::getUserConfig(int userId) {
	return getInfo(&UserConfigAdapter::getUserConfig, userId);
}

bool CompleteUserInfoManagerI::getUserPassport(int userId) {
	return getInfo(&UserPassportAdapter::getUserPassport, userId);
}

void CompleteUserInfoManagerI::setValid(bool valid, const Ice::Current& ctx) {
  InvokeClient ic = InvokeClient::create(ctx, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	IceUtil::RWRecMutex::WLock lock(validMutex_);
	valid_ = valid;
	if(valid_) {
		ServerStateSubscriber::instance().setStatus(1);
	} else {
		ServerStateSubscriber::instance().setStatus(0);
	}
}

bool CompleteUserInfoManagerI::isValid(const Ice::Current& ctx) {
  InvokeClient ic = InvokeClient::create(ctx);
	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}
void CompleteUserInfoManagerI::completeUserInfo(const vector<int>& userIds, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId size = " << userIds.size();
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
	for(vector<int>::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
		if((CompleteUserInfoManagerI::instance().*(fpIndex_[CHECK_USERINFO_SIZE - 1].getfp_))(*it)) {
			for(int i = 0; i < CHECK_USERINFO_SIZE - 1; ++i) {
				if(!(CompleteUserInfoManagerI::instance().*(fpIndex_[i].getfp_))(*it)) {
					try {
						MCE_INFO("[CompleteUserInfoManagerI::create" << fpIndex_[i].name_ << " userId = " << *it);
						(CompleteUserInfoManagerI::instance().*(fpIndex_[i].createfp_))(*it);
					} catch(const Ice::Exception& e) {
						MCE_WARN("[CompleteUserInfoManagerI::create" << fpIndex_[i].name_ << "] Ice::Exception " << e.what());
					} catch(const exception& e) {
						MCE_WARN("[CompleteUserInfoManagerI::create" << fpIndex_[i].name_ << "] Cplusplus Exception " << e.what());
					} catch(...) {
						MCE_WARN("[CompleteUserInfoManagerI::create" << fpIndex_[i].name_ << "] unknonwn Exception");
					}
				}
			}
		}
	}
}

