#include "BatchUpdaterBulletinClickAdapter.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace com::xiaonei::adapter;
using namespace std;
using namespace mop::hi::svc::batchtraits;

void BatchUpdaterBulletinClickAdapter::initialize() {

	_batchUpdaterManager = locate<BatchUpdaterManagerPrx> ("M", TWO_WAY);
	_batchUpdaterManagerOneWay = locate<BatchUpdaterManagerPrx> ("M", ONE_WAY);
}

void BatchUpdaterBulletinClickAdapter::click(int userId, int bulletinId)  {

	IntSeq keyvalue;
	keyvalue.push_back(userId);
	keyvalue.push_back(bulletinId);
	_batchUpdaterManagerOneWay->inc(keyvalue, 1, BulletinClickName);
}
