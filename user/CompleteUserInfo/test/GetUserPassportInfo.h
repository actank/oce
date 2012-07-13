#ifndef __GET_USER_PASSPORT_INFO_H__
#define __GEG_USER_PASSPORT_INFO_H__

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
#include "UserContactAdapter.h"
#include "UserDoingAdapter.h"

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

string info[] = {"UserName", "UserRight", "UserState", "UserUrl", "UserBorn", "UserStage", "UserDoing", "UserTime",
					"UserContact", "UserConfig", "UserPassport"};
const int USER_DETAIL_SIZE = 11;
const int MAX_TRY_TIME = 5;
template<class TypePtr, class Type> 
void try_get(TypePtr (Type::*fp)(int, const Ice::Context& ctx), int user_id, vector<bool>& result, int index) {
	string type_name = typeid(Type).name();
	try {
		TypePtr ptr = (Type::instance().*fp)(user_id, Ice::Context());
	} catch(const Ice::TimeoutException& e) {
		MCE_INFO("[" << type_name << ":" << "get" << type_name << "] userId = " << user_id << " " << e.what() << endl);
	} catch(const exception& e) {
		cout << "[" << type_name << ":" << "get" << type_name << "] userId = " << user_id << " " << e.what() << endl;
		result[index] = false;
	}
}
void getUserPassportInfo(int userId, vector<bool>& result) {
	try_get(&UserNameAdapter::getUserName, userId, result, 0);
	try_get(&UserRightAdapter::getUserRight, userId, result, 1);
	try_get(&UserStateAdapter::getUserState, userId, result, 2);
	try_get(&UserUrlAdapter::getUserUrl, userId, result, 3);
	try_get(&UserBornAdapter::getUserBorn, userId, result, 4);
	try_get(&UserStageAdapter::getUserStage, userId, result, 5);
	try_get(&UserDoingAdapter::getUserDoing, userId, result, 6);
	try_get(&UserTimeAdapter::getUserTime, userId, result, 7);
	try_get(&UserContactAdapter::getUserContact, userId, result, 8);
	try_get(&UserConfigAdapter::getUserConfig, userId, result, 9);
	try_get(&UserPassportAdapter::getUserPassport, userId, result, 10);
}
void show_error_info(const vector<bool>& result, string info[], int userId) {
	cout << userId;
	size_t size = result.size();
	for(size_t i = 0; i < size; ++i) {
		if(!result[i]) {
			cout << "\t" << info[i] + "Error";
		}
	}
	cout << endl;
}

#endif
