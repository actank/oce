#include "BatchUpdaterUserApiCountReplicaAdapter.h"
#include "boost/lexical_cast.hpp"
//#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"
#include <iostream>

using namespace mop::hi::svc::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;
using namespace xce::clusterstate;

//---------------------------------------------------------------------------

void BatchUpdaterUserApiCountReplicaAdapter::inc(int id, int type, int count) {
	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	try{
		getProxy(0)->inc(keyvalue, count, GeneralUserApiCountName);
	}catch(Ice::Exception &e)
	{
		MCE_DEBUG("BatchUpdaterUserApiCountReplicaAdapter::inc Exception:"<<e.what());
	}
}

void BatchUpdaterUserApiCountReplicaAdapter::dec(int id, int type, int count) {
	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	getProxy(0)->dec(keyvalue, count, GeneralUserApiCountName);
}

void BatchUpdaterUserApiCountReplicaAdapter::set(int id, int type, int count) {
	MyUtil::IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	getProxy(0)->set(keyvalue, boost::lexical_cast<std::string>(count), GeneralUserApiSetterName);
}

void BatchUpdaterUserApiCountReplicaAdapter::incLong(long id, int type, int count) {
        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(id);
        keyvalue.push_back(type);
        try{
                getProxy(0)->incLong(keyvalue, count, GeneralUserApiCountName);
        }catch(Ice::Exception &e)
        {
                MCE_DEBUG("BatchUpdaterUserApiCountReplicaAdapter::incLong Exception:"<<e.what());
        }
}

void BatchUpdaterUserApiCountReplicaAdapter::decLong(long id, int type, int count) {
        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(id);
        keyvalue.push_back(type);
        getProxy(0)->decLong(keyvalue, count, GeneralUserApiCountName);
}

void BatchUpdaterUserApiCountReplicaAdapter::setLong(long id, int type, int count) {
        MyUtil::LongSeq keyvalue;
        keyvalue.push_back(id);
        keyvalue.push_back(type);
        getProxy(0)->setLong(keyvalue, boost::lexical_cast<std::string>(count), GeneralUserApiSetterName);
}

