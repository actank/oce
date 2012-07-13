#ifndef LIVE_ANALYZER_H
#define LIVE_ANALYZER_H

#include "XiaoNeiMonitorI.h"
#include "XceMonitor.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Thread.h>
#include "TopicI.h"
#include <IceStorm/IceStorm.h>
#include "Channel.h"

namespace xce {
namespace monitor {

class LiveNode:virtual public IceUtil::Shared{
public:
	LiveNode(std::string& name, int lastpush, int lastalert):_name(name),_lastpush(lastpush),_lastalert(lastalert){}
	std::string _name;
	int _lastpush;
        int _lastalert;
};

typedef IceUtil::Handle<LiveNode> LiveNodePtr;
class LiveAnalyzer;
class LiveThread: public IceUtil::Thread{
public:
	LiveThread(const LiveNodePtr& n, int interval, int waitcount, LiveAnalyzer* live):_interval(interval),_waitcount(waitcount),_live(live),_node(n), stopped_(false) {}
	virtual void run();
	void stop();
private:
        int _interval;
        int _waitcount;
		LiveAnalyzer* _live;
        LiveNodePtr _node;
		IceUtil::Mutex mutex_;
		bool stopped_;
};

typedef IceUtil::Handle<LiveThread> LiveThreadPtr;

class LiveAnalyzer:virtual public Analyzer{
public:
	LiveAnalyzer();
	virtual void analyze(const MyUtil::Str2StrMap& logmap);
	virtual void removeSubjects(const MyUtil::StrSeq& subjects);
private:
	IceUtil::Mutex _mutex;
	std::map<std::string, LiveNodePtr> _map;
	std::map<std::string, LiveThreadPtr> _threads;
	int _interval;
	int _waitcount;
};

};
};


#endif
