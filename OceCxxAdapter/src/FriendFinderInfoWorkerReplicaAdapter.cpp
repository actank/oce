#include "FriendFinderInfoWorkerReplicaAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::friendfinder::adapter;
using namespace std;

//*********************************************************************************************
void FriendFinderInfoWorkerReplicaAdapter::createDataFromDB(int userId) {
	getProxy(userId)->createDataFromDB(userId);
}

void FriendFinderInfoWorkerReplicaAdapter::restartControl() {
	getProxy(0)->restartControl();
}
