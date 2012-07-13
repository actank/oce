#ifndef RANGE_ANALYZER_H
#define RANGE_ANALYZER_H

#include "XceMonitor.h"
#include "XiaoNeiMonitorI.h"
#include "TopicI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Thread.h>
#include <IceStorm/IceStorm.h>
#include "Channel.h"

namespace xce {
namespace monitor {
class RangeNode : virtual public IceUtil::Shared{
public:
	RangeNode(std::string& name,int lastpush, int lastalert, int alermtimes):name(name),lastpush(lastpush),lastalert(lastalert),alermtimes(alermtimes){}
	std::string name;
	int lastpush;
	int lastalert;
	int alermtimes;
};

typedef IceUtil::Handle<RangeNode> RangeNodePtr;

/*class RangeThread : public IceUtil::Thread{
public:
	RangeThread(const RangeNodePtr& n, int _interval, int _waitcount, const std::map<string, Alert>& _alertmap):_interval(_interval), _waitcount(_waitcount), _alertmap(_alertmap), _node(n){}
	virtual void run();
private:
	int _interval;
	int _waitcount;
	std::map<string, Alert> _alertmap;
	RangeNodePtr _node;
};

typedef IceUtil::Handle<RangeThread> RangeThreadPtr;*/

class RangeAnalyzer: virtual public Analyzer{
public:
	RangeAnalyzer();
	virtual void analyze(const MyUtil::Str2StrMap& logmap);
	virtual void removeSubjects(const MyUtil::StrSeq& subjects);
	
private:
	MyUtil::Str2StrMap _configMap;

	IceUtil::Mutex _mutex;
	std::map<std::string, RangeNodePtr> _map;
	std::vector<IceUtil::ThreadPtr> _threads;

	float _min;
	float _max;
	float _moremax;
	float _moremin;
	float _deviation;
	int _interval;
	int _waitcount;
	std::string _unit;
};


}
}
#endif
