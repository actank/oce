/*
 * FeedSyncDbI.cpp
 *
 *  Created on: 2010-11-17
 *      Author: wu
 */

#include "FeedSyncDbI.h"
#include "ServiceI.h"
#include "FeedContentReplySync.h"

using namespace xce::feed;
using namespace com::xiaonei::xce;



void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedSyncDbI::instance(), service.createIdentity("SYNC", ""));
	MyUtil::TaskManager::instance().scheduleRepeated(new ReplyDbUpdateTimer);
}



namespace xce {

namespace feed {


void FeedSyncDbI::replaceFeedReply(const MyUtil::LongSeq& feedIds, const FeedReplyPtr& reply,const Ice::Current&)
{
	MCE_INFO("FeedSyncDbI::replaceFeedReply");
	FeedContentReplySync::instance().replaceFeedReply(feedIds, reply);
}

}

}



