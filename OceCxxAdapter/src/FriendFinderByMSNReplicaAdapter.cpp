#include "FriendFinderByMSNReplicaAdapter.h"

using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= FriendFinderByMSNAdapter =================================

MSNdata FriendFinderByMSNAdapter::getMSNdata(Ice::Int id){
	return getProxy(id)->getMSNdata(id);
}

MSNmap FriendFinderByMSNAdapter::getPUAmap(Ice::Int id){
	return getProxy(id)->getPUAmap(id);
}
MyUtil::IntSeq FriendFinderByMSNAdapter::getPUAids(Ice::Int id){
	return getProxy(id)->getPUAids(id);
}
MyUtil::IntSeq FriendFinderByMSNAdapter::getPUAidsWithMask(Ice::Int id,const MyUtil::IntSeq& mask){
	return getProxy(id)->getPUAidsWithMask(id,mask);
}

MSNmap FriendFinderByMSNAdapter::getPUKmap(Ice::Int id){
	return getProxy(id)->getPUKmap(id);
}
MyUtil::IntSeq FriendFinderByMSNAdapter::getPUKids(Ice::Int id){
	return getProxy(id)->getPUKids(id);
}
MyUtil::IntSeq FriendFinderByMSNAdapter::getPUKidsWithMask(Ice::Int id,const MyUtil::IntSeq& mask){
	return getProxy(id)->getPUKidsWithMask(id,mask);
}

void FriendFinderByMSNAdapter::add(Ice::Int hostId,Ice::Int guestId,Ice::Int type){
	getProxy(hostId)->add(hostId,guestId,type);
}

void FriendFinderByMSNAdapter::remove(Ice::Int hostId,Ice::Int guestId){
	getProxy(hostId)->remove(hostId,guestId);
}
void FriendFinderByMSNAdapter::removePUA(Ice::Int hostId,Ice::Int guestId){
	getProxy(hostId)->removePUA(hostId,guestId);
}
void FriendFinderByMSNAdapter::removePUK(Ice::Int hostId,Ice::Int guestId){
	getProxy(hostId)->removePUK(hostId,guestId);
}

void FriendFinderByMSNAdapter::reload(Ice::Int id){
	getProxy(id)->reload(id);
}

