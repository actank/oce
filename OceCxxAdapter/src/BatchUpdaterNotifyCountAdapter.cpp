#include "BatchUpdaterNotifyCountAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace mop::hi::svc::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;

//---------------------------------------------------------------------------
void BatchUpdaterNotifyCountAdapter::initialize() {
	_batchUpdaterManager = locate<BatchUpdaterManagerPrx> ("M", TWO_WAY);
	_batchUpdaterManagerOneway = locate<BatchUpdaterManagerPrx> ("M",
			ONE_WAY);
}

void BatchUpdaterNotifyCountAdapter::inc(int id, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	_batchUpdaterManagerOneway->inc(keyvalue, count, NotifyCountName);
}

void BatchUpdaterNotifyCountAdapter::dec(int id, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	_batchUpdaterManagerOneway->dec(keyvalue, count, NotifyCountName);
}

void BatchUpdaterNotifyCountAdapter::set(int id, int count) {
	IntSeq keyvalue;
	keyvalue.push_back(id);
	_batchUpdaterManagerOneway->set(keyvalue, boost::lexical_cast<std::string>(count), NotifySetterName);
}
