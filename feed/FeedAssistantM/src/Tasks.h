/*
 * Tasks.h
 *
 *  Created on: Dec 24, 2009
 *      Author: antonio
 */

#ifndef TASKS_H_
#define TASKS_H_

#include "TaskManager.h"
#include "RFeed.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace xce {
namespace feed {

const int DISPATCH_TASK_LEVEL = 10001;
const int REPLY_TASK_LEVEL = 10002;
using namespace MyUtil;
using boost::multi_index_container;
using namespace boost::multi_index;

class ConfigCache: public MyUtil::Singleton<ConfigCache> {
public:
  ConfigCache() :
    kSize_(100000) {
  }
  void Add(const Ice::Long feed_id, const map<string, string> & config);
  map<string, string> Get(const Ice::Long feed_id);
  size_t Size();
private:
  void Kick();
  string PrintHelper();

  /////////////////////////////////////////////////////////////////////////
  struct Item: public IceUtil::Shared {
    Ice::Long feed_id;
    map<string, string> config;
  };
  typedef IceUtil::Handle<Item> ItemPtr;

  struct seqtag {
  };
  struct feedtag {
  };

  typedef multi_index_container<ItemPtr, indexed_by<sequenced<tag<seqtag> > ,
      hashed_unique<tag<feedtag> ,
          BOOST_MULTI_INDEX_MEMBER(Item, Ice::Long,feed_id)> > > ConfigPool;

  typedef ConfigPool::index<seqtag>::type SIndex;
  typedef ConfigPool::index<feedtag>::type FIndex;
  /////////////////////////////////////////////////////////////////////////

  ConfigPool pool_;
  IceUtil::RWRecMutex mutex_;
  const int kSize_; //cache��С
};

////////////////////////////////////////////////////////////////////////
class RemoveFeedByIdTask: public MyUtil::Task {
public:
  RemoveFeedByIdTask(Ice::Long feedId) :
    Task(), _feedId(feedId) {
  }
  ;

  virtual void handle();
private:
  Ice::Long _feedId;
};

class RemoveFeedBySourceStypeTask: public MyUtil::Task {
public:
  RemoveFeedBySourceStypeTask(Ice::Long source, int stype) :
    Task(), _source(source), _stype(stype) {
  }
  ;
  virtual void handle();
private:
  Ice::Long _source;
  int _stype;
};

class RemoveFeedBySourceStypeActorTask: public MyUtil::Task {
public:
  RemoveFeedBySourceStypeActorTask(Ice::Long source, int stype, int actor) :
    Task(), _source(source), _stype(stype), _actor(actor) {
  }
  ;
  virtual void handle();

private:
  Ice::Long _source;
  int _stype, _actor;
};

class RemoveFeedByParentTask: public MyUtil::Task {
public:
  RemoveFeedByParentTask(Ice::Long psource, int ptype) :
    Task(), _psource(psource), _ptype(ptype) {
  }
  ;
  virtual void handle();

private:
  Ice::Long _psource;
  int _ptype;
};

class ReadMiniTask: public MyUtil::Task {
public:
  ReadMiniTask(int user, Ice::Long feedId = -1) :
    Task(), _user(user), _feedId(feedId) {
  }
  ;
  virtual void handle();

private:
  int _user;
  Ice::Long _feedId;
};

class ReadNewsTask: public MyUtil::Task {
public:
  ReadNewsTask(int user, Ice::Long feedId = -1) :
    Task(), _user(user), _feedId(feedId) {
  }
  ;
  virtual void handle();

private:
  int _user;
  Ice::Long _feedId;
};

class FeedDispatchTask: public MyUtil::Task {
public:
  FeedDispatchTask(const FeedSeedPtr& seed, const FeedReplyPtr& reply = 0) :
    Task(DISPATCH_TASK_LEVEL), _seed(seed), _reply(reply) {
  }
  ;
  virtual void handle();
  void CheckSend2School(const FeedConfigPtr& config);
private:
  FeedSeedPtr _seed;
  FeedReplyPtr _reply;
};

class ReplyDelayDispatchTimer: public MyUtil::Timer {
public:
  ReplyDelayDispatchTimer(const ReplyDataPtr& reply,
      const FeedConfigPtr& config) :
    Timer(2000), _reply(reply), _config(config) {
  }
  ;
  virtual void handle();

private:
  ReplyDataPtr _reply;
  FeedConfigPtr _config;
};

class ReplyDispatchTask: public MyUtil::Task {
public:
  ReplyDispatchTask(Ice::Long source, int stype, int actor,
      const FeedReplyPtr& reply) :
    Task(), _source(source), _stype(stype), _actor(actor), _reply(reply) {
  }
  ;
  virtual void handle();
private:
  Ice::Long _source;
  int _stype, _actor;
  FeedReplyPtr _reply;
};

class ReplyDispatchWithConfigTask: public MyUtil::Task {
public:
  ReplyDispatchWithConfigTask(Ice::Long source, int stype, int actor,
      const FeedReplyPtr& reply,const map<string,string> & reply_config) :
    Task(), _source(source), _stype(stype), _actor(actor), _reply(reply),
        config_(reply_config) {
  }

  virtual void handle();

private:
  int ParseMiniGroupId();
  bool SetFeedMemc();
  bool SetInteraction();
  bool SetReplyBufferTimer();
  bool SendToFeedDispatcher();
  bool SendToPhotoMem();
  bool RefleshMiniGroupTime();
  ReplyDataPtr GetReplyData();

  Ice::Long _source;
  int _stype, _actor;
  FeedReplyPtr _reply;
  map<string, string> config_;
  vector<long> fids_;

  ReplyDataPtr reply_data_;
};

/*class ReplyDispatchAgainTimer: public MyUtil::Timer{
 public:
 ReplyDispatchAgainTimer(Ice::Long source, int stype, int actor, const FeedReplyPtr& reply, Ice::Long feedId):
 Timer(1000, 1),_source(source),_stype(stype),_actor(actor),_reply(reply),_feedId(feedId){
 retry_count = 0;
 };
 virtual void handle();
 private:
 Ice::Long _source;
 int _stype,_actor;
 FeedReplyPtr _reply;
 Ice::Long _feedId;
 int retry_count;
 };*/

class ReplaceXMLTask: public MyUtil::Task {
public:
  ReplaceXMLTask(Ice::Long source, int stype, int actor, const string & xml) :
    Task(), _source(source), _stype(stype), _actor(actor), _xml(xml) {
  }
  ;
  virtual void handle();
private:
  Ice::Long _source;
  int _stype, _actor;
  string _xml;
};

class FeedReplyBufferTimer: public Timer, public MyUtil::Singleton<
    FeedReplyBufferTimer> {
public:
  FeedReplyBufferTimer() :
    Timer(10000) {
  }
  void changeReply(vector<Ice::Long> feeds, const FeedReplyPtr& reply);
  void handle();
private:
  map<Ice::Long, FeedReplyPtr> replys_;
  IceUtil::Mutex mutex_;
};

}
}

#endif /* TASKS_H_ */
