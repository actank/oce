#include "UserNetworkAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

using namespace mop::hi::svc::adapter;
using namespace std;


//---------------------------------------------------------------------------
NetworkManagerPrx UserNetworkAdapter::getNetworkManager(int id) {	
	return locate<NetworkManagerPrx>(_managers, "NM", id, TWO_WAY, 10000);
}

NetworkManagerPrx UserNetworkAdapter::getNetworkManagerOneway(int id) {
	return locate<NetworkManagerPrx>(_managersOneway, "NM", id, ONE_WAY);
}

NetworkBigObjectNPtr UserNetworkAdapter::getBigObjectN(int userId){
	return getNetworkManager(userId)->getBigObjectN(userId);
}

