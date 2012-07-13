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

namespace xce {
namespace feed {

using namespace MyUtil;

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

class FeedDispatchTask : public MyUtil::Task{
public:
	FeedDispatchTask(const FeedSeedPtr& seed, const FeedReplyPtr& reply = 0): Task(), _seed(seed),_reply(reply){};
	virtual void handle();
private:
	FeedSeedPtr _seed;
	FeedReplyPtr _reply;
};

class ReplyDelayDispatchTimer: public MyUtil::Timer{
public:
	ReplyDelayDispatchTimer(const ReplyDataPtr& reply,const FeedConfigPtr& config):Timer(2000),_reply(reply),_config(config){};
	virtual void handle();

private:
	ReplyDataPtr _reply;
	FeedConfigPtr _config;
};

class ReplyDispatchTask: public MyUtil::Task{
public:
	ReplyDispatchTask(Ice::Long source, int stype, int actor, const FeedReplyPtr& reply):Task(),_source(source),_stype(stype),_actor(actor),_reply(reply){};
	virtual void handle();
private:
	Ice::Long _source;
	int _stype,_actor;
	FeedReplyPtr _reply;
};

class LoadBlockUserTimer: public MyUtil::Timer{
public:
	LoadBlockUserTimer():Timer(1000*60){};
	virtual void handle();
};



}
}

#endif /* TASKS_H_ */
