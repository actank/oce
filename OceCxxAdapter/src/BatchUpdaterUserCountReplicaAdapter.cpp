#include "BatchUpdaterUserCountReplicaAdapter.h"
#include "boost/lexical_cast.hpp"
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <iostream>

using namespace mop::hi::svc::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;
using namespace xce::clusterstate;

//---------------------------------------------------------------------------

void BatchUpdaterUserCountReplicaAdapter::inc(int id, int type, int count) {
	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	getProxy(INDEX)->inc(keyvalue, count, GeneralUserCountName);
}

void BatchUpdaterUserCountReplicaAdapter::dec(int id, int type, int count) {
	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	getProxy(INDEX)->dec(keyvalue, count, GeneralUserCountName);
}

void BatchUpdaterUserCountReplicaAdapter::set(int id, int type, int count) {
	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	getProxy(INDEX)->set(keyvalue, boost::lexical_cast<std::string>(count), GeneralUserSetterName);
}

void BatchUpdaterUserCountReplicaAdapter::incLong(long id, int type, int count) {
        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(id);
        keyvalue.push_back(type);
        getProxy(INDEX)->incLong(keyvalue, count, GeneralUserCountName);
}

void BatchUpdaterUserCountReplicaAdapter::decLong(long id, int type, int count) {
        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(id);
        keyvalue.push_back(type);
        getProxy(INDEX)->decLong(keyvalue, count, GeneralUserCountName);
}

void BatchUpdaterUserCountReplicaAdapter::setLong(long id, int type, int count) {
        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(id);
        keyvalue.push_back(type);
        getProxy(INDEX)->setLong(keyvalue, boost::lexical_cast<std::string>(count), GeneralUserSetterName);
}

