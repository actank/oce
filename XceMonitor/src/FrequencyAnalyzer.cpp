#include "FrequencyAnalyzer.h"
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

FrequencyAnalyzer::FrequencyAnalyzer() :
	Analyzer() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr prop = service.getCommunicator()->getProperties();
	_configMap = prop->getPropertiesForPrefix("FrequencyAnalyzer");

	/*stringstream stream(prop->getProperty("Mobile"));
	string mobile;
	while (stream >> mobile) {
		_alert->addMobile(mobile);
	}*/
	_time = prop->getPropertyAsIntWithDefault("FrequencyAnalyzer.Time", 60);
	_size = prop->getPropertyAsIntWithDefault("FrequencyAnalyzer.Size", 6);
}
void FrequencyAnalyzer::analyze(const Str2StrMap& logmap) {
	time_t now = time(0);
	/*Str2StrMap::const_iterator logvalueiterator = logmap.find("value");
	 if (logvalueiterator == logmap.end())
	 {
	 return;
	 }
	 long now = lexical_cast<long>(logvalueiterator->second);*/

	Str2StrMap::const_iterator lognameiterator = logmap.find("name");
	if (lognameiterator == logmap.end()) {
		return;
	}
	string name = lognameiterator->second;

	IceUtil::Mutex::Lock lock(_mutex);

	_map[name].times.push(now);
	if (_map[name].times.size() == 1) {
		_map[name].lastalert = 0;
		return;
	}

	long last = _map[name].times.front();

	MCE_DEBUG("name: " << name << " size=" << _size << " mapsize="
			<< _map[name].times.size() << " last=" << last << " now=" << now << " lastalerm=" << _map[name].lastalert);

	if (now - last <= _time && (int) (_map[name].times.size()) >= _size) {
		string message = "";
		Str2StrMap::const_iterator logmessageiterator = logmap.find("message");
		if (logmessageiterator == logmap.end()) {
			Str2StrMap::const_iterator configmessageiterator = _configMap.find(
					"FrequencyAnalyzer.Message");
			if (configmessageiterator == _configMap.end()) {
				return;
			}
			message = configmessageiterator->second;
		} else {
			message = logmessageiterator->second;
		}
		stringstream ss;
		ss << name << ": " << message;
		if (now - _map[name].lastalert >= 900) {
			_map[name].lastalert = now;
			string key;
			stringstream stream(message);
			stream >> key;
			//向主题key发送信息ss.str()
			std::vector<string> subjects;
			subjects.push_back(key);
      MonitorAlerterAdapter::instance().alert(key, ss.str(), ALERTERALL, 900);
		}
	}

	while ((int) (_map[name].times.size()) > _size) {
		_map[name].times.pop();
	}
}

void FrequencyAnalyzer::removeSubjects(const MyUtil::StrSeq& subjectjs) {
}
