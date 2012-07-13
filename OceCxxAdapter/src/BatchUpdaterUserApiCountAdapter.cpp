#include "BatchUpdaterUserApiCountAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace mop::hi::svc::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;

//---------------------------------------------------------------------------
void BatchUpdaterUserApiCountAdapter::initialize() {
	_batchUpdaterManager = locate<BatchUpdaterManagerPrx> ("M", TWO_WAY);
	_batchUpdaterManagerOneway = locate<BatchUpdaterManagerPrx> ("M",
			ONE_WAY);
}

void BatchUpdaterUserApiCountAdapter::inc(int id, int type, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	try{
	_batchUpdaterManagerOneway->inc(keyvalue, count, GeneralUserApiCountName);
	}catch(Ice::Exception &e)
	{
		MCE_DEBUG("BatchUpdaterUserApiCountAdapter::inc Exception:"<<e.what());
	}
}

void BatchUpdaterUserApiCountAdapter::dec(int id, int type, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	_batchUpdaterManagerOneway->dec(keyvalue, count, GeneralUserApiCountName);
}

void BatchUpdaterUserApiCountAdapter::set(int id, int type, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	_batchUpdaterManagerOneway->set(keyvalue, boost::lexical_cast<std::string>(count), GeneralUserApiSetterName);
}
