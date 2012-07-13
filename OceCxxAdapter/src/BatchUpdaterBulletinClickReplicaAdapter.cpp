#include "BatchUpdaterBulletinClickReplicaAdapter.h"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace com::xiaonei::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;
using namespace xce::clusterstate;

void BatchUpdaterBulletinClickReplicaAdapter::initialize()  {

	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
}

void BatchUpdaterBulletinClickReplicaAdapter::click(int userId, int bulletinId)  {

	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(userId);
	keyvalue.push_back(bulletinId);
	_clientCS.getProxy(INDEX)->inc(keyvalue, 1, BulletinClickName);
}

void BatchUpdaterBulletinClickReplicaAdapter::click(int userId, long bulletinId)  {

        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(userId);
        keyvalue.push_back(bulletinId);
        _clientCS.getProxy(INDEX)->incLong(keyvalue, 1, BulletinClickName);
}

