/*
 * FocusManagerI.h
 *
 *  Created on: May 21, 2010
 *      Author: yejingwei
 */
#ifndef __FEED_FOCUS_H__
#define __FEED_FOCUS_H__
#include "RFeed.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "LogWrapper.h"
#include <IceUtil/Thread.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
namespace xce {
namespace feed {

using namespace MyUtil;
using namespace std;

const int TYPE[] = {100, 500, 600, 700, 800, 900, 1800, 1900, 2000, 2400, 3700, 8100, 8200, 8600};

//------------------------------------------------------------------------------------
class TypeFilter{
	set<int> stypes_;
public:
	TypeFilter(const vector<int> & stypes):stypes_(stypes.begin(),stypes.end()){};
	void Filter(FeedItemSeqSeq & itemseqseq);
};

class FocusManagerI: public FocusManager, public Singleton<FocusManagerI> {
public:
  virtual FeedDataSeq GetFocusFeedData(int user, int begin,int limit, const ::Ice::Current& =
      ::Ice::Current());
  virtual FeedDataSeq GetFocusFeedDataByStype(int user, int begin,int limit, const vector<int> & stypes, const ::Ice::Current& =
      ::Ice::Current());
  virtual bool AddFocuses(::Ice::Int source, int target, const ::Ice::Current& = ::Ice::Current());
  virtual int AddFocusWithCheck(::Ice::Int source, int target, const ::Ice::Current& = ::Ice::Current());
  virtual void DelFocuses(::Ice::Int source, int target, const ::Ice::Current& = ::Ice::Current());
  virtual MyUtil::IntSeq GetFocuses(int user, const Ice::Current& = Ice::Current());
  virtual int GetFocusFeedIncCount(int user, const ::Ice::Current& = ::Ice::Current());
private:
  FeedDataSeq GetFocusFeedDataImpl(int user, int begin, int limit,const vector<int> stypes) ;
  FeedDataSeq BuildFeedData(FeedItemSeqSeq & itemseqseq, FeedContentDict & dict);
  void CheckLoad(int uid, vector<Ice::Long> & fids, FeedContentDict & dict);
  void GetContents(int uid, vector<Ice::Long> & fids, FeedContentDict & dict);

  MyUtil::IntSeq getFixedType();
};


}
;
}
;
#endif
