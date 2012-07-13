#include "CountCacheAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace mop::hi::svc::adapter;
using namespace std;

//---------------------------------------------------------------------------
CountManagerPrx CountCacheAdapter::getCountManager(int id) {
	return locate<CountManagerPrx>(_managers, "MANAGER", id, TWO_WAY);
}

CountManagerPrx CountCacheAdapter::getCountManagerOneway(int id) {
	return locate<CountManagerPrx>(_managersOneway, "MANAGER", id, ONE_WAY);
}

void CountCacheAdapter::incImLeavewordCount(int id, int count) {
	getCountManagerOneway(id)->inc(id, count, CImLeaveword);
}

void CountCacheAdapter::decImLeavewordCount(int id, int count) {
	getCountManagerOneway(id)->dec(id, count, CImLeaveword);
}
void CountCacheAdapter::incGuestRequestCount(int id, int count) {
	getCountManagerOneway(id)->inc(id, count, CGuestRequest);
}
void CountCacheAdapter::decGuestRequestCount(int id, int count) {
	getCountManagerOneway(id)->dec(id, count, CGuestRequest);
}
int CountCacheAdapter::getGuestRequestCount(int id) {
	return getCountManager(id)->get(id, CGuestRequest);
}
CounterPtr CountCacheAdapter::getAll(int id){
	return getCountManager(id)->getAll(id);
}

