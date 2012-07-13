/*
 * FeedSyncDbI.h
 *
 *  Created on: 2010-11-17
 *      Author: wu
 */

#ifndef FEEDSYNCDBI_H_
#define FEEDSYNCDBI_H_


#include "Singleton.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "RFeed.h"
#include "FeedSyncDb.h"
#include <Ice/Ice.h>
#include<string>

using namespace std;

namespace xce{
namespace feed{


class FeedSyncDbI : public FeedSyncDb,
		public MyUtil::Singleton<FeedSyncDbI>{
public:
	virtual void replaceFeedReply(const MyUtil::LongSeq& feedIds, const FeedReplyPtr& reply,const Ice::Current& = Ice::Current());
};


}

}

#endif /* FEEDSYNCDBI_H_ */
