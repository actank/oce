#include "FriendInputAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
//using namespace mop::hi::svc::adapter;
using namespace std;
using namespace com::xiaonei::services::FriendInput;
using namespace MyUtil;

//---------------------------------------------------------------------------
FriendInputManagerPrx FriendInputAdapter::getFriendInputManager(int id) {
	return locate<FriendInputManagerPrx>(_managers, "Manager", id, TWO_WAY);
}

FriendInputManagerPrx FriendInputAdapter::getFriendInputManagerOneway(int id) {
	return locate<FriendInputManagerPrx>(_managersOneway, "Manager", id, ONE_WAY);
}

void FriendInputAdapter::reLoadFriend(int id) {
	MCE_INFO("FriendInputAdapter -> reLoadFriend "<<id);
	getFriendInputManagerOneway(id)->reLoadFriend(id);
}

vector<int> FriendInputAdapter::getFriendInput(int hostId, string condition, int limit) {
  FriendInputManagerPrx proxy = getFriendInputManager(hostId);
  std::cout<< "enter into try!"<< std::endl;
  //try {
    std::cout<<"id:"<<hostId<<" condition:"<<condition<<std::endl;
    //proxy->getFriendInput(hostId, condition,limit);
    vector<int> temp = proxy->getFriendInput(hostId, condition,limit);
    std::cout << "hello, getFriendInput, proxy="<<proxy<< std::endl;
    return temp;//proxy->getFriendInput(hostId, condition,limit);
 // } catch (IceUtil::Exception& e) {
    //MCE_INFO("FriendInputAdapter::getFriendInput Exception:" << e.what() << "proxy=" << proxy);
 // }
 /* } catch (ConnectTimeoutException e) {
    Oce.getLogger().error(
        this.getClass().getName() + ".getFriendInput " + e);
  } catch (TimeoutException e) {
    Oce.getLogger().error(
        this.getClass().getName() + ".getFriendInput " + e);
  }*/
  //return _value;
}
