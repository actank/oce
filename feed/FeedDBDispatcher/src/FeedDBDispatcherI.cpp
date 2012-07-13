#include "FeedDBDispatcherI.h"
#include "FeedDBAdapter.h"
#include "IceLogger.h"
#include "ServiceI.h"

using namespace xce::feed;
using namespace MyUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add( &FeedDBDispatcherI::instance(), service.createIdentity("M",""));
	TaskManager::instance().scheduleRepeated(&FeedDBDispatcherI::instance());
}

void FeedDBDispatcherI::putBatch(const FeedItem& item, const MyUtil::IntSeq& userIds, const Ice::Current&){
	incPut();
	//MCE_INFO("@@@ " << userIds.size());
	Int2IntMap usersWeight;
	for(int i=0; i<userIds.size(); i++){
		usersWeight[userIds.at(i)] = 0;
		//MCE_INFO(userIds.at(i));
	}
	try{
		FeedDBMasterAdapter::instance().putBatch(item, usersWeight);
	}catch(Ice::Exception& e){
		MCE_WARN("invoke FeedDBData error, " << e);	
	}
}
/*
void FeedDBDispatcherI::put(int userid, const FeedItem& item, const Ice::Current&){
	//timeval beg, end, end1;
	//gettimeofday(&beg, NULL);
	//if(userid != 257876975){
	//	return;
	//}
	incPut();
	try{
		//MCE_INFO("### " << userid);
		FeedDBDataAdapter::instance().put(userid, item);
	}catch(Ice::Exception& e){
		MCE_WARN("invoke FeedDBData error, " << e);	
	}
	//gettimeofday(&end, NULL);
	//float elipse = ((end.tv_sec-beg.tv_sec)*1000000 + end.tv_usec-beg.tv_usec)/1000.0;
	//MCE_INFO("elipse " << elipse);
}*/
/*
FeedItemSeq FeedDBDispatcherI::get(int userid, int begin, int count, const Ice::Current&){

}*/

