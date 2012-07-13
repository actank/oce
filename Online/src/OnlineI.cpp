#include "OnlineI.h"

using namespace std;
using namespace MyUtil;
using namespace xce::online;
using namespace Ice;
using namespace xce::mediator::onlineevent;

void MyUtil::initialize()  {	
	
	// 添加服务
	ServiceI& service = ServiceI::instance();
	PropertiesPtr prop = service.getCommunicator()->getProperties();
	long duration = atol(prop->getPropertyWithDefault("EvictDuration", "1200").c_str());
	long freq = atol(prop->getPropertyWithDefault("EvictFreq", "60").c_str());
	OnlineManagerI::instance().initialize(duration);
	service.getAdapter()->add(&OnlineManagerI::instance(), service.createIdentity("M",""));

	// 启动线程
	ThreadPoolConfig cfg(1,1);
	TaskManager::instance().config(TASK_LEVEL_NORMAL, cfg);
	TaskManager::instance().execute( new EvictTask(OnlineManagerI::instance().getEvictor(), freq));
}

void OnlineManagerI::initialize(time_t duration) {
	_evictor.duration(duration);
}

void OnlineManagerI::UserActivity(const ::MyUtil::IntSeq& userIds, const ::Ice::Current&) {
	
	vector<int> vecUsersOn;
	for ( ::MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); it++ ) {
		if ( _evictor.add((*it), (*it)) ) {
			vecUsersOn.push_back(*it);
		}
	}
	if ( vecUsersOn.size() > 0 )  {
		UsersOnline(vecUsersOn);
	}
	MCE_DEBUG("Activity " << userIds.size() << " Users");
}
 
::MyUtil::Int2IntMap OnlineManagerI::UserOnlineStat(const ::MyUtil::IntSeq& userIds, const ::Ice::Current&) {

	::MyUtil::Int2IntMap mapStat;
	for ( ::MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); it++ ) {

		if ( _evictor.has(*it) ) {
			mapStat.insert( pair<int, int>((*it), CStatOnline) );
		}
		else {
			mapStat.insert( pair<int, int>((*it), CStatOffline) );
		}
	}
	MCE_DEBUG("Get " << mapStat.size() << " Users' Status");
	return mapStat;
}

OnlineEvictor& OnlineManagerI::getEvictor() {
	return _evictor;
}

void OnlineManagerI::UsersOnline(const vector<int>& userIds) {

	map<int, int> mapOnline;
	for ( vector<int>::const_iterator it = userIds.begin(); it != userIds.end(); it++ ) {
		mapOnline.insert(pair<int, int>((*it), ONLINEOTHER));
	}
	OnlineEventMediatorAdapter::instance().sendOnlineEvent(mapOnline);
	MCE_DEBUG("OnlineEventMediator : Online " << mapOnline.size() << " Users");
}

void OnlineManagerI::UsersOffline(const vector<int>& userIds) {
}

void EvictTask::handle() {

	while (true) {
		vector<int> vecUsersOff;			
		_evictor.evict(vecUsersOff);
		if ( vecUsersOff.size() > 0 )	{
			OnlineManagerI::instance().UsersOffline(vecUsersOff);
		}

		sleep(_freq);
	}
}

void OnlineEvictor::evict(vector<int>& vecUserIds) {
	if (this->_duration> 0) {
		time_t lastTime = time(NULL) - this->_duration;
		while (!_queue.empty()) {
			TimedEvictor<int, int>::EvictorQueue::reverse_iterator p = _queue.rbegin();
			TimedEvictor<int, int>::EvictorMap::iterator pos = *p;
			if (!pos->second.expired(lastTime)) {
				break;
			}
			vecUserIds.push_back(pos->first);
			this->_queue.erase(pos->second.pos);
			this->_map.erase(pos);
		}
	}
}

bool OnlineEvictor::add(int obj, int id) {
	IceUtil::Mutex::Lock lock(this->_mutex);

	TimedEvictor<int, int>::Entry entry;
	entry.timestamp = time(NULL);
	entry.obj = obj;
	bool bNew = true;
	while (true) {
		pair<TimedEvictor<int, int>::EvictorMap::iterator, bool> result =
		this->_map.insert(make_pair(id, entry));
		if (result.second) {
			// entry.pos = this->_queue.insert(this->_queue.begin(), result.first);
			(*result.first).second.pos = this->_queue.insert(this->_queue.begin(), result.first);
			break;
		}
		bNew = false;
		TimedEvictor<int, int>::EvictorMap::iterator pos = this->_map.find(id);
		if (pos != this->_map.end()) {
			this->_queue.erase(pos->second.pos);
			this->_map.erase(pos);
		}
	}
	return bNew;
}

