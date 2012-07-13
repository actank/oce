#ifndef _FEEDREPLYPASSAGE_H_
#define _FEEDREPLYPASSAGE_H_



#include "TransitAider.h"
#include "WTransit.h"
#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include "TalkProxy.h"
#include "RFeed.h"
#include "ServiceI.h"
#include "TalkCommon.h"
#include "TalkUtil/src/pugixml.hpp"
#include "Pipe.h"
#include "FeedPassage.h"


namespace talk {
namespace wtransit {

using namespace MyUtil;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace xce::feed;
using namespace pugi;




class FeedReplyWithTarget: public Ice::Object{
public:
    FeedReplyWithTarget( const ReplyDataPtr& replyData, MyUtil::IntSeq target)
		:replyData(replyData),target(target){
			latets.reset();
	}
    virtual ~FeedReplyWithTarget(){	}

	ReplyDataPtr replyData;
	MyUtil::IntSeq target;
	TimeStat latets;
}; 

typedef IceUtil::Handle<FeedReplyWithTarget> FeedReplyWithTargetPtr;	
typedef vector<FeedReplyWithTargetPtr> FeedReplyWithTargetSeq;	

	

class FeedReplyToMessagConverter{
public:
	void setData(const ReplyDataPtr& replyData);
	MessagePtr getToTalkMessage( const JidPtr& to );
	MessagePtr getToPhoneMessage( const JidPtr& to );
	static string _getTemplate(int stype);
private:
	string _buildFNode();
	static string _getType(int stype);
	static string _getSType(int type);
	
	ReplyDataPtr _replyData;
	JidPtr _jfrom;
	//int _replyType;
	string _talkMsg;
//	xml_document _toTalkMasg;
};



	
class FeedReplyPipe : public MyUtil::Timer{
public:
	FeedReplyPipe() : Timer(20, 11) {
	}
	;
	virtual void handle();
private:
//private:

	//MessageSeq _msgs;

};	
	
};
};


#endif /* _FEEDREPLYPASSAGE_H_ */
