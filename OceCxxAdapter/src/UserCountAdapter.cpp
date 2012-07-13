#include "UserCountAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::usercount::adapter;
using namespace std;

//---------------------------------------------------------------------------
UserCountManagerPrx UserCountAdapter::getUserCountManager(int id) {
	return getProxy(id);
}

UserCountManagerPrx UserCountAdapter::getUserCountManagerOneway(int id) {
	return getProxyOneway(id);
}

void UserCountAdapter::inc(int id, int type, int count)	{
	getUserCountManagerOneway(id)->inc(id, type, count);
}


void UserCountAdapter::dec(int id, int type, int count) {
	getUserCountManagerOneway(id)->dec(id, type, count);
}

void UserCountAdapter::set(int id, int type, int count) {
	getUserCountManagerOneway(id)->set(id, type, count);
}

int UserCountAdapter::get(int id, int type)	{
	return getUserCountManager(id)->get(id, type);
}

UserCounterPtr UserCountAdapter::getAll(int id){
	return getUserCountManager(id)->getAll(id);
}

void UserCountAdapter::reload(int id)	{
	getUserCountManager(id)->reload(id);
}
