#include "LiveAnalyzer.h"
#include "XceMonitor.h"
#include "XiaoNeiMonitorI.h"
#include "ServiceI.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "MonitorAdapter.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace monitor;
using namespace std;
using namespace MyUtil;
using namespace boost;
using namespace xce::monitor;

LiveAnalyzer::LiveAnalyzer():Analyzer(){
	ServiceI& service = ServiceI::instance();
        Ice::PropertiesPtr prop = service.getCommunicator()->getProperties();
        _interval = prop->getPropertyAsIntWithDefault("LiveAnalyzer.Interval", 10);
        _waitcount = prop->getPropertyAsIntWithDefault("LiveAnalyzer.Waitcount", 3);
}

void LiveAnalyzer::analyze(const Str2StrMap& logmap) {
	time_t now = time(0);
	Str2StrMap::const_iterator lognameiterator = logmap.find("name");
	if (lognameiterator == logmap.end()) {
		return;
	}
	string name = lognameiterator->second;
	IceUtil::Mutex::Lock lock(_mutex);

	if (_map.find(name) == _map.end()) {
		_map[name] = new LiveNode(name, now, 0);
		LiveThreadPtr mt = new LiveThread(_map[name], _interval, _waitcount, this);
		_threads.insert(make_pair(name, mt));
		mt->start(65535).detach();
		return;
	}
	_map[name]->_lastpush = now;
}

void LiveAnalyzer::removeSubjects(const StrSeq& subjects) {
	for(StrSeq::const_iterator it = subjects.begin(); it != subjects.end(); ++it) {
		IceUtil::Mutex::Lock lock(_mutex);
		if(_map.find(*it) != _map.end()) {
			_map.erase(*it);
			_threads[*it]->stop();
			_threads.erase(*it);
		}		
	}	
}

void LiveThread::stop() {
	IceUtil::Mutex::Lock lock(mutex_);
	stopped_ = true;
}

bool canConnect(const string& host, int port) {
        struct sockaddr_in pin;
        struct hostent* nlp_host;
        while ((nlp_host=gethostbyname(host.c_str()))==0){
				MCE_INFO("LiveThread::run gethostby  name error host = " << host);
                return false;
        }
        bzero(&pin, sizeof(pin));
        pin.sin_family = AF_INET;
        pin.sin_addr.s_addr = htonl(INADDR_ANY);
        pin.sin_addr.s_addr=((struct in_addr *)(nlp_host->h_addr))->s_addr;
        pin.sin_port=htons(port);

        int sd = socket(AF_INET,SOCK_STREAM,0);

        //建立连接
        while (connect(sd,(struct sockaddr*)&pin,sizeof(pin))==-1){
				MCE_INFO("LiveThread::run connect error host = " << host);
                close(sd);
                return false;
        }
        close(sd);
        return true;
}

void LiveThread::run() {
	int lastalart = 0;
	Ice::PropertiesPtr prop = ServiceI::instance().getCommunicator()->getProperties();
	string message = prop->getPropertyWithDefault("LiveAnalyzer.Message", "overtime");
	while (true) {
		{				  
			IceUtil::Mutex::Lock lock(mutex_);
			if(stopped_) {
				MCE_INFO("LiveThread::run exit");
				std::vector<string> subjects;
				subjects.push_back("UnsubscribeSubject");
        MonitorAlerterAdapter::instance().alert("UnsubscribeSubject", _node->_name + ": has been unsubscribed, check it!", ALERTEREMAIL, 900);
				break;
			}
		}
		int now = time(0);
		MCE_DEBUG(_node->_name <<" overtime: " << now - _node->_lastpush);
		if (now - _node->_lastpush > 120 && now - lastalart >= 120){
			MCE_INFO("LiveThread::run report alert");
			lastalart = now;
			stringstream stream;
			stream << _node->_name <<" " <<message;
			//向主题_node->_name 发送信息message
			std::vector<string> subjects;
			if(canConnect(_node->_name, 22)) {
				subjects.push_back(_node->_name);
        MonitorAlerterAdapter::instance().alert(_node->_name, _node->_name + ": is alive, but not send heart beat, please check it", ALERTERALL, 900);
			} else {
				subjects.push_back(_node->_name);
        MonitorAlerterAdapter::instance().alert(_node->_name, _node->_name + ":" + message, ALERTERALL,  900);

			}
		}
		sleep(_interval);
	}
}

