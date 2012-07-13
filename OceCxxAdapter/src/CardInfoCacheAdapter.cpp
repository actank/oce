/*
 * CardInfoCacheAdapter.h
 *
 *  Created on: 2009-12-24
 *      Author: zhanghan
 */

#include "OceCxxAdapter/src/CardInfoCacheAdapter.h"

using namespace xce::cardcache;

CardInfoCacheAdapter::CardInfoCacheAdapter():
		clientCS_("ControllerCardInfoCache", 120, 300) {
	clientCS_.intialize();
}

CardInfoSeq CardInfoCacheAdapter::get(const MyUtil::IntSeq& userIds) {

	try {
		return clientCS_.getProxy(0)->get(userIds);			
	} catch (...) {
		MCE_WARN("get userIds batch exception");
	}
	return CardInfoSeq();
}

PartOfCardInfoSeq CardInfoCacheAdapter::getPart(const MyUtil::IntSeq& userIds) {

	try {
		return clientCS_.getProxy(0)->getPart(userIds);			
	} catch(std::exception& e){
		MCE_WARN("getPart " << e.what());
	} catch (...) {
		MCE_WARN("getPart userIds batch exception");
	}
	return PartOfCardInfoSeq();
}
void CardInfoCacheAdapter::setProperty(int userId, const MyUtil::Str2StrMap& props){
	try {
		MyUtil::Str2StrMap::const_iterator it=props.find("profile_privacy");

		if(it!=props.end()){
			string privacy = parse.getJsonStr(it->second);	
			clientCS_.getProxy(userId)->setPrivacy(userId, privacy);
		}else{
		 	clientCS_.getProxy(userId)->setProperty(userId,props);
		}
	}catch(...){
		MCE_WARN("set userid property error "<<userId );
	}
}
/*int main(){
	map<std::string,std::string> m;
	m["qq"]="123456";
	CardInfoCacheAdapter::instance().setProperty(220840375,m);
	m["profile_privacy"]="{a:3,c:3,e:3,f:3,g:99,j:0,k:0,l:3,m:3,p:3,q:null,s:99,t:99,w:3,h:-1,n:99,d:3,i:-1}";
	CardInfoCacheAdapter::instance().setProperty(220840375,m);
}*/
