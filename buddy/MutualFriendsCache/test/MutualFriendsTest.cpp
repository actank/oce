#include "MutualFriendsAdapter.h"
#include <Util.h>
#include <iostream>
#include <sstream>
#ifndef _GLOBALID_H_
#define _GLOBALID_H_
#include "UtilCxx/src/GlobalId.h"
#endif

using namespace std;
using namespace MyUtil;
using namespace xce::buddy::adapter;

int main(int argc, char * argv[]){
	if(argc != 3){
		cout << "Usage : " << argv[0] << " uid gid" << endl;
		return -1;
	}
	int uid = atoi(argv[1]);
	long appId = atol(argv[2]);
	GlobalIdPtr gid = GlobalIdTransfer::get(appId);
	cout << "appId : " << appId << ", uid : " << uid << endl;
	cout << "after convert appId = " << GlobalIdTransfer::get(gid) << endl;
	cout << "gid->type = " << gid->type << ", gid->id = " << gid->id << endl;
	try{
		cout << "getMutualFriendsCount = " << MutualFriendsCacheAdapter::instance().getMutualFriendsCount(uid, gid) << endl;
		cout << "getUnMutualFriendsCount = " << MutualFriendsCacheAdapter::instance().getUnMutualFriendsCount(uid, gid) << endl;
		cout << "getFansCount = " << MutualFriendsCacheAdapter::instance().getFansCount(gid) << endl;
	} catch (Ice::TimeoutException e) {
		cout << "Ice::TimeoutException : " << e.what() << endl;
	} catch (Ice::Exception e) {
		cout << "Ice::Exception : " << e.what() << endl;
	} catch (...) {
		cout << "Unkown exception " << endl;
	}

	return 0;
}

