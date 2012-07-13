#ifndef _FEEDPASSAGE_H_
#define _FEEDPASSAGE_H_


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



namespace talk {
namespace wtransit {

using namespace MyUtil;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
//using namespace mop::hi::svc::dispatch;
using namespace xce::feed;
using namespace pugi;

class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};




class FeedWithTarget: public Ice::Object{
public:
//	FeedWithTarget(){	}
//	FeedWithTarget(const FeedContentPtr& content);
//	FeedWithTarget(const FeedSeedPtr& feed, const MyUtil::IntSeq& target);
	FeedWithTarget(const FeedSeedPtr& feed, const MyUtil::IntSeq& target);
	FeedWithTarget(const FeedContentPtr& content);
	// MessagePtr getToPhoneMessage( const JidPtr& to );
	// MessagePtr getToWebPagerMessage( const JidPtr& to );
	// MessagePtr getToTalkMessage( const JidPtr& to );
	// MyUtil::IntSeq getTarget(){
	// 	return target;
	// }
//private:
	// string _buildFNode();
	// JidPtr _getJfrom();
	// static string _getType(int stype);
	// static string _getSType(int type);
	// static string _getTemplate(int stype);
	Ice::Long feed;
	Ice::Long source;
	int bigType;//bigType 是原来系统中的type，smallType是原系统中的stype, 例如 601，bigType=6，smallType=601
	int smallType;
	int actor;
	string feedxml;
	int time;
	int replyCount;
	// JidPtr jfrom;
	// xml_document toPhoneMsg;
	// xml_document toTalkMasg;
//	xml_document toWebPagerMsg;
	MyUtil::IntSeq target;
	TimeStat latets;
};

typedef IceUtil::Handle<FeedWithTarget> FeedWithTargetPtr;	
typedef vector<FeedWithTargetPtr> FeedWithTargetSeq;

const string PHOTO_SERVER="http://photo.renren.com";	

class FeedToMessagConverter{
public:
	void setData(const FeedContentPtr& content);
	void setData(const FeedWithTargetPtr& feed);
	MessagePtr getToPhoneMessage( const JidPtr& to );
	MessagePtr getToWebPagerMessage( const JidPtr& to );
	MessagePtr getToTalkMessage( const JidPtr& to, bool offline=false);
	

	static string _getTemplate(int stype);
private:
	string _buildFNode();
//	JidPtr _getJfrom();
	static string _getType(int stype);
	static string _getSType(int type);
	
	FeedWithTargetPtr _feed;
	JidPtr _jfrom;
	string _toPhoneMsg;
//	string _toTalkMsg;
//	xml_document _toPhoneMsg;
	xml_document _toTalkMasg;
	bool _toTalkMasgReady;
};

	

	

class FeedPipe : public MyUtil::Timer{
public:
	FeedPipe() : Timer(20, 10) {
	}
	;
	virtual void handle();
private:
//private:

//	MessageSeq _msgs;

};	




	
};
};


#endif /* _FEEDPASSAGE_H_ */
