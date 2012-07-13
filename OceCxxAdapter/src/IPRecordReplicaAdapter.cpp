#include "IPRecordReplicaAdapter.h"

using namespace xce::iprecord;
using namespace xce::iprecord::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= IPRecordAdapter =================================

MyUtil::IntSeq IPRecordAdapter::getUsers(Ice::Long ip,Ice::Int limit){
	return getProxy(ip)->getUsers(ip,limit);
}
MyUtil::IntSeq IPRecordAdapter::getUsersWithMask(Ice::Long ip,Ice::Int limit,const MyUtil::IntSeq& mask){
	return getProxy(ip)->getUsersWithMask(ip,limit,mask);
}
Ice::Int IPRecordAdapter::getCount(Ice::Long ip){
	return getProxy(ip)->getCount(ip);
}
void IPRecordAdapter::addUser(Ice::Long ip,Ice::Int userId){
	getProxy(ip)->addUser(ip,userId);
}
void IPRecordAdapter::removeUser(Ice::Long ip,Ice::Int userId){
	getProxy(ip)->removeUser(ip,userId);
}
void IPRecordAdapter::reload(Ice::Long ip){
	getProxy(ip)->reload(ip);
}
void IPRecordAdapter::updateToDB(Ice::Long ip){
	getProxy(ip)->updateToDB(ip);
}


