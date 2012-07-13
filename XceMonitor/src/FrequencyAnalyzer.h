#ifndef FREQUENCY_ANALYZER_H
#define FREQUENCY_ANALYZER_H

#include "XceMonitor.h"
#include "XiaoNeiMonitorI.h"
#include "TopicI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceStorm/IceStorm.h>
#include "Channel.h"
#include <queue>

namespace xce {
namespace monitor {
class FrequencyNode {
public:
	int lastalert;
	std::queue<time_t> times;
};

class FrequencyAnalyzer: virtual public Analyzer {
public:
	FrequencyAnalyzer();
	virtual void analyze(const MyUtil::Str2StrMap& logmap);
	virtual void removeSubjects(const MyUtil::StrSeq& subjects);

private:
	struct FrequencyNode {
		int lastalert;
		bool alertflag;
		std::queue<time_t> times;
	};

private:
	MyUtil::Str2StrMap _configMap;
	IceUtil::Mutex _mutex;
	std::map<std::string, FrequencyNode> _map;

	int _time;
	int _size;
};
}
}
#endif
