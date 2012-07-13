#include "BatchUpdaterUserCountAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace mop::hi::svc::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;

//---------------------------------------------------------------------------
void BatchUpdaterUserCountAdapter::initialize() {
	_batchUpdaterManager = locate<BatchUpdaterManagerPrx> ("M", TWO_WAY);
	_batchUpdaterManagerOneway = locate<BatchUpdaterManagerPrx> ("M",
			ONE_WAY);
}

void BatchUpdaterUserCountAdapter::inc(int id, int type, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	_batchUpdaterManagerOneway->inc(keyvalue, count, GeneralUserCountName);
}

void BatchUpdaterUserCountAdapter::dec(int id, int type, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	_batchUpdaterManagerOneway->dec(keyvalue, count, GeneralUserCountName);
}

void BatchUpdaterUserCountAdapter::set(int id, int type, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	keyvalue.push_back(type);
	_batchUpdaterManagerOneway->set(keyvalue, boost::lexical_cast<std::string>(count), GeneralUserSetterName);
}
