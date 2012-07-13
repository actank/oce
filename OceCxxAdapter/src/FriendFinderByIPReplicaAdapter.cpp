#include "FriendFinderByIPReplicaAdapter.h"

using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= FriendFinderByIPAdapter =================================

IPNoteSeq FriendFinderByIPAdapter::getFreqIP(Ice::Int userId,Ice::Int limit){
	return getProxy(userId)->getFreqIP(userId,limit);
}
IPNoteSeq FriendFinderByIPAdapter::getRecIP(Ice::Int userId,Ice::Int limit){
	return getProxy(userId)->getRecIP(userId,limit);
}

void FriendFinderByIPAdapter::accessIP(Ice::Int userId,Ice::Long ip){
	getProxy(userId)->accessIP(userId,ip);
}
void FriendFinderByIPAdapter::accessIPStr(Ice::Int userId,const string& ip){
	getProxy(userId)->accessIPStr(userId,ip);
}
void FriendFinderByIPAdapter::accessIPs(Ice::Int userId,const MyUtil::LongSeq& ips){
	getProxy(userId)->accessIPs(userId,ips);
}
void FriendFinderByIPAdapter::accessIPsStr(Ice::Int userId,const MyUtil::StrSeq& ips){
	getProxy(userId)->accessIPsStr(userId,ips);
}

