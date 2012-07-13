/*
 * FeedFocusInvertI.h
 *
 *  Created on: May 21, 2010
 *      Author: yejingwei
 */
#include "FeedFocusInvertI.h"
#include "FeedMiniAdapter.h"
#include "FeedContentAdapter.h"
#include <IceUtil/IceUtil.h>
#include "RFeed.h"
#include <utility>
#include "TaskManager.h"
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>
#include "ServiceI.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <vector>
#include <QueryRunner.h>
#include "ServiceI.h"
#include <Ice/Exception.h>
#include <exception>
#include <stdexcept>
#include <ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h>

using namespace MyUtil;
using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;

//-----------------------------------------------------------------------------------------------

class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container) {
    ostringstream ios;
    ios << "(";
    bool first_item = true;
    int i = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr, ++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << ",";
      }
      ios << *itr;
    }
    ios << ")";
    return ios.str();
  }
};


class TimeStat {
public:
  TimeStat() {  reset(); }
  void reset() { gettimeofday(&_begin, NULL); }
  float Get(bool clear = true) {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec)
    								+( _end.tv_usec - _begin.tv_usec);
    if(clear) reset();
    return timeuse / 1000;
  }
private:
  timeval _begin, _end;
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedFocusInvertI::instance(), service.createIdentity(
      "M", ""));
  Ice::CommunicatorPtr communicator = service.getCommunicator();

  int mod = communicator->getProperties()->getPropertyAsInt( "FeedFocusInvert.Mod");
  int interval = communicator->getProperties()->getPropertyAsIntWithDefault( "FeedFocusInvert.Interval", 5);
  xce::serverstate::ServerStateSubscriber::instance().initialize( "ControllerFeedFocusInvertR", &FeedFocusInvertI::instance(), mod, interval,
      new XceFeedChannel());
  MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);


  TaskManager::instance().scheduleRepeated(&FeedFocusInvertStatTimer::instance());
}


//-----------------------------------------------------------------------------------------------

void FeedFocusInvertI::KickTarget(int target,const ::Ice::Current&){
	MCE_INFO("FeedFocusInvertI::KickTarget. target:" << target);
	IceUtil::Mutex::Lock lock(mutex_);
	UidIndex & index = container_.get<1> ();
	index.erase(target);
}

MyUtil::IntSeq FeedFocusInvertI::GetSources(int target, const Ice::Current& ){
	MCE_INFO("FeedFocusInvertI::GetSources. target:" << target  << " begin" );
	try{

	TargetInfoPtr tinfo;
	TimeStat ts;
	set<int> sources;
	bool in_mem = false;;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		tinfo = Find(target);
		if(tinfo){
			sources=(tinfo->GetSources());
			in_mem = true;
		}
	}

	if(in_mem){
		MCE_INFO("FeedFocusInvertI::GetSources. target:" << target << " in mem:1" << " cost:" << ts.Get()  << " source:" << PrintHelper::Print(sources) );
		return vector<int>(sources.begin(),sources.end());
	}

  if( !GetDB(target, tinfo)){
    MCE_WARN("FeedFocusInvertI::GetTargets --> call GetDB fail , target:" << target << " cost:" << ts.Get());
    return vector<int>();
  }

  sources = tinfo->GetSources();

  AddCache(target,tinfo);

	MCE_INFO("FeedFocusInvertI::GetSources. target:" << target << " in mem:0" << " cost:" << ts.Get()  << " source:" << PrintHelper::Print(sources) );
  return vector<int>(sources.begin(),sources.end());

  } catch (Ice::Exception& e) {
    MCE_WARN(" FeedFocusInvertI::GetDB --> error:" << e << ",target = " << target);
  } catch (std::exception& e) {
    MCE_WARN(" FeedFocusInvertI::GetDB --> error:" << e.what() << ",target = " << target);
	}
	retrun vector<int>();
}


//---------------------------------------------------------------------------

void FeedFocusInvertI::AddCache(int target,TargetInfoPtr & tinfo){
	IceUtil::Mutex::Lock lock(mutex_);

	SeqIndex & sindex = container_.get<0> ();
	pair<SeqIndex::iterator, bool> p = container_.push_front(tinfo);

	if (!p.second) {
		(*p.first)->Add(tinfo->GetSources());
		sindex.relocate(sindex.begin(), p.first);
	} else {
		Kick();
	}
	return ;
}

TargetInfoPtr FeedFocusInvertI::Find(int target,bool relocate){
	TargetInfoPtr tinfo;
	UidIndex & index = container_.get<1> ();
	UidIndex::iterator uit = index.find(target);
	if (uit != index.end()) { //在cache中
		tinfo = *uit;
		if(relocate){
			SeqIndex::iterator sit = container_.project<0> (uit);//新查的放到前面
			container_.relocate(container_.get<0> ().begin(), sit);
		}
	}
	return tinfo;
}
//添加source新增的关注好友
void FeedFocusInvertI::AddFocus(::Ice::Int source, int target, const ::Ice::Current&){
	KickTarget(target);
	MCE_INFO("FeedFocusInvertI::AddFocus. source:" << source << " target:" << target);
}

//删除source设定的关注好友
void FeedFocusInvertI::DelFocus(::Ice::Int source, int target, const ::Ice::Current& ){
	KickTarget(target);
	MCE_INFO("FeedFocusInvertI::AddFocus. source:" << source << " target:" << target);
}
bool FeedFocusInvertI::GetDB(int target, TargetInfoPtr & tinfo) { //返回false说明查询数据库失败
  Statement sql;
  sql << "SELECT source FROM relation_feed_high WHERE target = " << target;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("feedwb", CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FeedFocusInvertI::GetDB --> error:" << e << ",target = " << target);
    return false;
  } catch (std::exception& e) {
    MCE_WARN(" FeedFocusInvertI::GetDB --> error:" << e.what() << ",target = " << target);
    return false;
  }
  //  vector<int> result;
  if (!res) {
    MCE_WARN("FeedFocusInvertI::GetDB --> StoryQueryResult wrong " << ",target = " << target);
    return false;
  }

  tinfo = new TargetInfo(target);

//  if (res.empty()) {
//    MCE_INFO("FeedFocusInvertI::GetDB. target:" << target << " no source in db");
//    return true;
//  }

  vector<int> sources;
  for (size_t i = 0; i < res.size(); ++i) {
    int source = res.at(i).at(0);
    sources.push_back(source);
  }
  tinfo->Add(sources);
  MCE_INFO("FeedFocusInvertI::GetDB. target:" << target << " source size:" << sources.size() <<" source:" << PrintHelper::Print(tinfo->GetSources()));
  return true;
}

void FeedFocusInvertI::Kick() {
  while (container_.size() > kFocusSize) { //在外层加锁
    MCE_DEBUG("FeedFocusInvertI::Kick --> Kicking source = " << (*(container_.rbegin()))->GetTarget());
    container_.pop_back();
  }
}

void FeedFocusInvertI::Dump(){
	size_t sz;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		sz = container_.size();
	}
	MCE_INFO("FeedFocusInvertI::Dump. size:" << sz);
}
