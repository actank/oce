#ifndef XIAONEI_MONITORI_H
#define XIAONEI_MONITORI_H

#include "XceMonitor.h"
#include "TopicI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceStorm/IceStorm.h>
#include "Channel.h"
#include "ServiceI.h"

namespace xce {
namespace monitor {

class Analyzer: virtual public IceUtil::Shared {
public:
	Analyzer();
	virtual void analyze(const MyUtil::Str2StrMap& logmap) = 0;
	virtual void removeSubjects(const MyUtil::StrSeq& subjects) = 0;
	virtual ~Analyzer();
};
typedef IceUtil::Handle<Analyzer> AnalyzerPtr;

class XiaoNeiMonitorI: virtual public XiaoNeiMonitor, IceUtil::Thread {
public:
	XiaoNeiMonitorI();
	void report(const MyUtil::Str2StrMap&, const Ice::Current&);
	void disable(const MyUtil::StrSeq&, const Ice::Current& = Ice::Current());
	void reload(const Ice::Current& = Ice::Current());
	void run();
private:
	AnalyzerPtr _analyzer;
	xce::storm::TopicIPtr _topic;
	time_t lastincoke_;
};
}
}
#endif

