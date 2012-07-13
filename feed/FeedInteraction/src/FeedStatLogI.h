/*
 * FeedStatLog.h
 *
 *  Created on: 2011-2-17
 *      Author: ld
 */

#ifndef __Feed_STAT_LOG_H__
#define  __Feed_STAT_LOG_H__


#include <string>
#include <vector>
#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "TaskManager.h"
#include "Singleton.h"
#include "FeedStatLog.h"
#include "OceCxxAdapter/src/XceLoggerAdapter.h"
#include "XceLogger.h"

namespace xce {

namespace feed {



class FeedStatLogI: public FeedStatLog,
    public MyUtil::Singleton<FeedStatLogI> {
public:
	FeedStatLogI() {
    logger_ = com::xiaonei::service::XceLoggerAdapter::instance().getLogger();
    logger_->initialize();
  }
//	virtual ~FeedStatLogI();

  virtual void AddLog(const ::MyUtil::StrSeq& category, const string& log_str, const ::Ice::Current& cur= ::Ice::Current());
  //virtual void foo(const ::Ice::Current& cur= ::Ice::Current()) {}
  //void ClearDirtyCache();
private:
	//FeedInteractionCache feed_interaction_cache_;
  com::xiaonei::service::XceLoggerPtr logger_; 
};

/*
class ResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
  ResultHandlerI(MyUtil::ObjectResultPtr& result) :
    _result(result) {
  }
  virtual bool handle(mysqlpp::Row& row) const;
private:
  MyUtil::ObjectResultPtr _result;
};

class UpdateTimer : public MyUtil::Timer, public MyUtil::Singleton<UpdateTimer>{
public:
  UpdateTimer() : Timer(10*1000, 2){}
  void handle();
  void PushClick(Ice::Long feedid){
    IceUtil::Mutex::Lock lock(mutex_);
    clicks_.push_back(feedid);
  }
  void PushReply(Ice::Long feedid){
    IceUtil::Mutex::Lock lock(mutex_);
    replys_.push_back(feedid);
  }
private:
  IceUtil::Mutex    mutex_;
  vector<Ice::Long> clicks_;
  vector<Ice::Long> replys_;
};
*/
}
}
#endif 
