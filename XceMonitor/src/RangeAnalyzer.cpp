#include "RangeAnalyzer.h"
#include "XceMonitor.h"
#include "XiaoNeiMonitorI.h"
#include "MonitorAdapter.h"
#include "ServiceI.h"
#include <sstream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace monitor;
using namespace MyUtil;
using namespace boost;
using namespace xce::monitor;

RangeAnalyzer::RangeAnalyzer() :
	Analyzer() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr prop = service.getCommunicator()->getProperties();
	_configMap = prop->getPropertiesForPrefix("RangeAnalyzer");
	_min = prop->getPropertyAsIntWithDefault("RangeAnalyzer.Min", 0);
	_max = prop->getPropertyAsIntWithDefault("RangeAnalyzer.Max", 100);
	_moremax = prop->getPropertyAsIntWithDefault("RangeAnalyzer.MoreMax", 200);
	_moremin = prop->getPropertyAsIntWithDefault("RangeAnalyzer.MoreMin", 0);
	_unit=prop->getPropertyWithDefault("RangeAnalyzer.Unit","");
	_deviation=prop->getPropertyAsIntWithDefault("RangeAnalyzer.Deviation",100);
}
void RangeAnalyzer::analyze(const Str2StrMap& logmap) {
	MCE_INFO("RangeAnalyzer::analyze logmap.size = " << logmap.size());
	time_t now = time(0);
	Str2StrMap::const_iterator lognameiterator = logmap.find("name");
	if (lognameiterator == logmap.end()) {
		return;
	}
	string name = lognameiterator->second;

	Str2StrMap::const_iterator logvalueiterator = logmap.find("value");
        if (logvalueiterator == logmap.end()) {
                return;
        }
        int value = lexical_cast<int>(logvalueiterator->second);

	IceUtil::Mutex::Lock lock(_mutex);

	if (_map.find(name) == _map.end()) {
		_map[name] = new RangeNode(name, now, 0, 0);
		/*IceUtil::ThreadPtr mt = new RangeThread(_map[name], _interval, _waitcount, _alertmap);
		_threads.push_back(mt);
		mt->start(65535).detach();*/
		return;
	}
	_map[name]->lastpush = now;

	MCE_DEBUG("name: " << name << " value: " << value << _unit);

	stringstream maxstring;
	maxstring << "RangeAnalyzer."<<name<<".Max";
	stringstream minstring;
	minstring << "RangeAnalyzer."<<name<<".Min";
	stringstream moremaxstring;
	moremaxstring << "RangeAnalyzer."<<name<<".MoreMax";
	stringstream moreminstring;
	moreminstring << "RangeAnalyzer."<<name<<".MoreMin";

	ServiceI& service = ServiceI::instance();

	float __max=service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(maxstring.str(), (int)_max);
	float __min=service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(minstring.str(), (int)_min);
	float __moremin=service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(moreminstring.str(), (int)_moremin);
	float __moremax=service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(moremaxstring.str(), (int)_moremax);

	if (value > __max || value < __min) {
		MCE_DEBUG("alert: name: " << name << "value: " << value <<_unit);
		string message = "";
		Str2StrMap::const_iterator logmessageiterator = logmap.find("message");
		if (logmessageiterator == logmap.end()) {
			Str2StrMap::const_iterator configmessageiterator = _configMap.find(
					"RangeAnalyzer.Message");
			if (configmessageiterator == _configMap.end()) {
				return;
			}
			message = configmessageiterator->second;
		} else {
			message = logmessageiterator->second;
		}
		stringstream ss;
		ss << name << ": " << message << " "<<value<< _unit;
		if (now - _map[name]->lastalert >= 1800) {
			_map[name]->lastalert = now;
			if (_map[name]->alermtimes<1 || value < __moremin || value > __moremax) {
				_map[name]->alermtimes++;
				//向主题name发送信息ss.str()
				std::vector<string> subjects;
				subjects.push_back(name);
        MonitorAlerterAdapter::instance().alert(name, ss.str(), ALERTERALL, 900);
			}
		}
	}else{
		if(value>=__min+_deviation && value<=__max-_deviation){
			_map[name]->alermtimes=0;
		}
	}
}

void RangeAnalyzer::removeSubjects(const StrSeq& subjects) {
}
/*void RangeThread::run(){
	int lastalart = 0;
	while (true){
		int now = time(0);
		MCE_DEBUG("Overtime: " << now - _node->lastpush);
		if (now - _node->lastpush > 120 && now - lastalart >= 120){
			lastalart = now;
			stringstream stream;
			stream << _node->name << " overtime";
			map<string, Alert>::const_iterator alertmapiterator = _alertmap.find(_node->name);
			if (alertmapiterator == _alertmap.end()) {
                                _alertmap["mobile"].sendSMS(stream.str());
                        } else {
                                _alertmap[_node->name].sendSMS(stream.str());
                        }
		}
		sleep(_interval);
	}
}
*/
