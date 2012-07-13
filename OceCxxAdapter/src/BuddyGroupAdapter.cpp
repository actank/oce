#include "BuddyGroupAdapter.h"

using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::model;
using namespace std;
using namespace MyUtil;

//----------------------------------------

void BuddyGroupAdapter::addGroup(int host, const string& groupName) {

	getProxy(host)->addGroup(host, groupName);
}
void BuddyGroupAdapter::renameGroup(int host, const string& oldName,
		const string& newName) {
	getProxy(host)->renameGroup(host, oldName, newName);
}
void BuddyGroupAdapter::removeGroup(int host, const string& groupName) {
	getProxy(host)->removeGroup(host, groupName);
}
void BuddyGroupAdapter::removeBuddy(int host, int buddy){
	getProxy(host)->removeBuddy(host,buddy);
}
void BuddyGroupAdapter::addBuddyToGroup(int host, const string& groupName,
		int buddy) {
	getProxy(host)->addBuddyToGroup(host, groupName, buddy);
}
int BuddyGroupAdapter::changeBuddyGroups(int host, int buddy,
		const MyUtil::StrSeq& nameSeq) {
	try {
		return getProxy(host)->changeBuddyGroups(host,buddy,nameSeq);
	} catch(Ice::TimeoutException& e) {
		MCE_WARN("BuddyGroupAdapter::changeBuddyGroups --> err:"<<e);
		return 2;
	}
	return 0;
}
BuddiedGroupsMap BuddyGroupAdapter::getBuddiedGroupsMap(int host, int timeout) {
	return getProxy(host)->getBuddiedGroupsMap(host);
}

Str2IntMap BuddyGroupAdapter::getAllGroupsBuddyCount(int host){
	return getProxy(host)->getAllGroupsBuddyCount(host);
}

MyUtil::StrSeq BuddyGroupAdapter::getGroupList(int host){
	return getProxy(host)->getGroupList(host);
}
       
MyUtil::IntSeq BuddyGroupAdapter::getBuddyList(int host, const string& groupName, int begin, int limit){
	return getProxy(host)->getBuddyList(host,groupName, begin, limit);
}

void BuddyGroupAdapter::load(int host){
	getProxy(host)->load(host);
} 
