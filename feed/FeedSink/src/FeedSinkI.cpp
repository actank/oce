/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "FeedSinkI.h"
#include "LogWrapper.h"
#include "ServiceI.h"
#include "Singleton.h"
#include <boost/foreach.hpp>
#include "FeedNews50Adapter.h"
using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace boost;
//using namespace com::xiaonei::xce;


class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container,int limit = 30) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";
    	ios << v;
      if(idx >= limit) break;
    }
    ios << ")";

    return ios.str();
  }
};

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedSinkI::instance(), service.createIdentity("M",			""));
	FeedNews50Handler::instance().Init();
}

void FeedSinkI::AddFeed(const FeedItem& fitem,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight,
    const Ice::Current&) {
	MCE_INFO("FeedSinkI::AddFeed. fid:" << fitem.feed
			<< " stype:" << (fitem.type & 0xFFFF)
			<< " actor:" << fitem.actor
			<< " time:" << fitem.time
			<< " merge:" << fitem.merge
			<< " target size:" << id2weight.size()
	);

//	FeedNews50Handler::instance().Handle(fitem,config,id2weight);
  TaskManager::instance().execute(new AddFeedTask(fitem, config,id2weight));
}
//-----------------------------------------------------------------------------

void AddFeedTask::handle(){
	FeedNews50Handler::instance().Handle(item_,config_,id2weight_);
}

//------------------------------------------------------------------------------------

void FeedNews50Handler::Init(){
	int uids[] = {
			239556923,//zjc
			241610785,//kw
			220678672,//yjw
			347348947,//jw1
			225092473,//ld
			40125272,//lq
	};
	testers_.insert(uids,uids+sizeof(uids)/sizeof(uids[0]));
	MCE_INFO("FeedNews50Handler::Init. testers size:" << testers_.size() << " testers:" << PrintHelper::Print(testers_));
}

Int2IntMap FeedNews50Handler::GetTester(const MyUtil::Int2IntMap& id2weight){
	Int2IntMap res;
	BOOST_FOREACH(const Int2IntMap::value_type & kv,id2weight){
		if(testers_.count(kv.first)){
			res.insert(kv);
		}
	}
	return res;
}


void FeedNews50Handler::Handle(const FeedItem& fitem,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight){
	Int2IntMap testers ;
	try{
	 testers = GetTester(id2weight);
	 if(!testers.empty()){
		FeedNews50Adapter::instance().addFeed(fitem,config,testers);
	 }
		MCE_INFO("FeedNews50Handler::Handle. fid:" << fitem.feed
				<< " stype:" << (fitem.type & 0xFFFF)
				<< " actor:" << fitem.actor
				<< " time:" << fitem.time
				<< " merge:" << fitem.merge
				<< " target size:" << id2weight.size()
				<< " testers size:" << testers.size()
		);

	}	catch(Ice::Exception& e) {
		MCE_WARN("FeedNews50Handler::Handle. fid:" << fitem.feed
				<< " stype:" << (fitem.type & 0xFFFF)
				<< " actor:" << fitem.actor
				<< " time:" << fitem.time
				<< " merge:" << fitem.merge
				<< " testers size:" << testers.size()
				<<" error:" << e);
	} catch (...){
		MCE_WARN("FeedNews50Handler::Handle. fid:" << fitem.feed
				<< " stype:" << (fitem.type & 0xFFFF)
				<< " actor:" << fitem.actor
				<< " time:" << fitem.time
				<< " merge:" << fitem.merge
				<< " testers size:" << testers.size()
				<<" unknown error" );
	}
}
//-------------------------------------------------------------------------------------------
void TargetPrinter::Handle(const FeedItem& fitem,const MyUtil::Int2IntMap& id2weight){
	if(fitem.actor == 366549583  || fitem.actor == 425544141){
		set<int> targets;
		BOOST_FOREACH(const Int2IntMap::value_type & p, id2weight){
			targets.insert(p.first);
		}
		MCE_INFO("FeedNews50Handler::Handle. fid:" << fitem.feed
				<< " stype:" << (fitem.type & 0xFFFF)
				<< " actor:" << fitem.actor
				<< " time:" << fitem.time
				<< " merge:" << fitem.merge
				<< " target size:" << id2weight.size()
				<< " target:" << PrintHelper::Print(targets)
		);
	}
}
