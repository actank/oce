#include "XiaoNeiMonitorI.h"
#include <boost/lexical_cast.hpp>
#include "ServiceI.h"
#include "TopicI.h"
#include "FrequencyAnalyzer.h"
#include "RangeAnalyzer.h"
#include "LiveAnalyzer.h"

using namespace xce::monitor;
using namespace std;
using namespace MyUtil;
using namespace xce::storm;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->addWithUUID(new XiaoNeiMonitorI);
}

Analyzer::Analyzer() {
}

Analyzer::~Analyzer(){

}

XiaoNeiMonitorI::XiaoNeiMonitorI() {
	lastincoke_ = time(0);
	reload();
	start();
}

void XiaoNeiMonitorI::run() {
	while(true) {
		time_t lastinvoke = lastincoke_;
		try {
			sleep(20);
			MCE_DEBUG("XiaoNeiMonitorI::run check lastinvoke");
			if (lastincoke_ == lastinvoke) {
				MCE_WARN("XiaoNeiMonitorI::run reload");
				//reload();
				exit(0);
			}
		} catch (...) {
			MCE_WARN("XiaoNeiMonitorI::run unknown exception");
		}
	}
}

void XiaoNeiMonitorI::reload(const Ice::Current& current) {
	string name = ServiceI::instance().getName();
	MCE_DEBUG("Server Name : "<<name);
	Ice::PropertiesPtr prop = Ice::createProperties();
	prop->load("/data/xce/XiaoNei/etc/templates/"+name+".config");

	string configTopic = prop->getPropertyWithDefault("Topic", "");
	MCE_DEBUG("configTopic " <<configTopic);
	if (configTopic == "") {
		MCE_WARN("No topic. abort.");
		abort();
	}
	_topic = new TopicI("XceMonitorStorm", configTopic);
  _topic->startKeepSubscribeThread();

	string configAnalyzer = prop->getPropertyWithDefault("Analyzer", "");
	if (configAnalyzer == "") {
		MCE_WARN("No analyzer. abort");
		abort();
	}

	MCE_INFO("XceMonitorStorm " << configTopic << " with Analyzer " << configAnalyzer);

	if (configAnalyzer == "FrequencyAnalyzer") {
		if(_analyzer==0){
		    _analyzer = new FrequencyAnalyzer();
		    _topic->subscribe(MyUtil::Str2StrMap(), this);
		}
	} else if (configAnalyzer == "RangeAnalyzer"){
		if(_analyzer==0){
		    _analyzer = new RangeAnalyzer();
		    _topic->subscribe(MyUtil::Str2StrMap(), this);
		}
	} else if (configAnalyzer == "LiveAnalyzer"){
		if(_analyzer==0){
		    _analyzer = new LiveAnalyzer();
		    _topic->subscribe(MyUtil::Str2StrMap(), this);
		}
	}
	MCE_INFO("end reload");
}

void XiaoNeiMonitorI::report(const Str2StrMap& m, const Ice::Current&) {
	lastincoke_ = time(0);
	_analyzer->analyze(m);
}

void XiaoNeiMonitorI::disable(const MyUtil::StrSeq& subjects, const Ice::Current& current) {
	_analyzer->removeSubjects(subjects);
}
