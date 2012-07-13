/*
 * CardRelationCacheAdapter.h
 *
 *  Created on: 2009-12-24
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/CardRelationCacheAdapter.h"

using namespace xce::cardcache;
using namespace MyUtil;

CardRelationCacheAdapter::CardRelationCacheAdapter():
		clientCS_("ControllerCardRelationCache", 120, 300) {
	clientCS_.intialize();
}

IntSeq CardRelationCacheAdapter::get(int userId,int begin,int limit ,int requestFlag) {
	try {
		return clientCS_.getProxy(userId)->get(userId,begin,limit,requestFlag);			
	} catch (...) {
		MCE_WARN("[CardRelatioCache] get relation exception");
	}
	return IntSeq();
}
void CardRelationCacheAdapter::remove(int userId, int friendId){
	try {
		return clientCS_.getProxy(0)->remove(userId,friendId);
	} catch (...) {
		MCE_WARN("[CardRelationCacheAdapter::remove] "<<userId<<" "<<friendId);
	}
}
AllRelationPtr CardRelationCacheAdapter::getAllRelation(int userId){
	try{
		return clientCS_.getProxy(0)->getAllRelation(userId);
	} catch(std::exception& ex){
		MCE_WARN("[CardRelationCacheadapter::getRelation ]"<<ex.what());
	} catch (...) {
		MCE_WARN("[CardRelationCacheAdapter::getAllRelation]"<<userId);
	}
	return new AllRelation;
}
