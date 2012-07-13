#include "GroupChatI.h"
#include "ServiceI.h"
#include "IceLogger.h"
#include <IceUtil/IceUtil.h>
#include <IceUtil/RWRecMutex.h>

using namespace xce::talk::groupchat;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&GroupChatManagerI::instance(),
			service.createIdentity("M", ""));
	TaskManager::instance().scheduleRepeated(new ScanGroupMapTask());
}

Group::Group(const Ice::Long & f, const int & u) :
	feedId(f), _timeLastModify(time(NULL)) {
	_members.insert(u);
}

bool Group::expired() {
	time_t t = time(NULL);
	MCE_DEBUG("Group::expired() --> check whether a group is expired. current time --> " << t << " .last modify time --> " << _timeLastModify);

	MCE_DEBUG("Group::expired() --> minus result " << (t - _timeLastModify));
	if (time(NULL) - _timeLastModify > GROUPLIFE) {
		return true;
	} else {
		return false;
	}
}
void Group::join(const int & m) {
	MCE_DEBUG("Group::join --> user:" << m << "join a group");

	if (!existMember(m)) {
		_members.insert(m);
		_timeLastModify = time(NULL);
	}
}
set<int> & Group::getMembers() {
	MCE_DEBUG("Group::getMembers --> get a set of members");
	return _members;
}

deque<Msg> & Group::getMsgHistory() {
	return _msgHistory;
}

void Group::insertMsg(const int & userId, const string & msg) {

	_msgHistory.push_back(Msg(userId, msg));
	while (_msgHistory.size() > 10) {
		_msgHistory.pop_front();
	}
	_timeLastModify = time(NULL);
}
bool Group::existMember(const int & m) {
	MCE_DEBUG("In Group::existMember()");
	int i = _members.count(m);
	MCE_DEBUG("_members.count(m) = " << i);
	return i;
}
bool Group::kickMember(const int & m) {

	if (existMember(m)) {
		_members.erase(m);
		_timeLastModify = time(NULL);
		return true;
	} else {
		return false;
	}
}

void GroupChatManagerI::init() {
	//_groupPool.resize(GROUPPOOLSIZE);
	//_mutexPool.resize(GROUPPOOLSIZE);

}

bool GroupChatManagerI::join(Ice::Long feedId, Ice::Int userId,
		const Ice::Current&) {
	MCE_DEBUG("GroupChatManagerI::join() --> feedId: " << feedId << " userId: " << userId);
	int index = _groupMod(feedId);

	IceUtil::Mutex::Lock lock(_mutexPool[index]);

	MCE_DEBUG("GroupChatManagerI::join() --> find, feedId: " << feedId);

	map<Ice::Long, Group>::iterator it = _groupPool[index].find(feedId);
	MCE_DEBUG("GroupChatManagerI::join() --> find, feedId: " << feedId);
	if (it != _groupPool[index].end()) { //如果存在这个groupId
		MCE_DEBUG("In if statement");
		it->second.join(userId);
		return true;
	} else { //如果没有这个groupId
		MCE_DEBUG("GroupChatManagerI::join() --> in else statement");
		Group g(feedId, userId);
		_groupPool[index].insert(make_pair(feedId, g));
		return false;
	}
}

bool GroupChatManagerI::kickMember(Ice::Long feedId, Ice::Int userId,
		const Ice::Current&) {
	int index = _groupMod(feedId);
	IceUtil::Mutex::Lock lock(_mutexPool[index]);
	map<Ice::Long, Group>::iterator group_itr = _groupPool[index].find(feedId);
	if (group_itr != _groupPool[index].end()) {
		return group_itr->second.kickMember(userId);
	} else {
		return false;
	}
}

bool GroupChatManagerI::sendMsg(Ice::Long feedId, Ice::Int userId,
		const std::string& msgContent, const Ice::Current&) {

	int index = _groupMod(feedId);
	IceUtil::Mutex::Lock lock(_mutexPool[index]);
	map<Ice::Long, Group>::iterator group_itr = _groupPool[index].find(feedId);

	//MCE_DEBUG("groupMap size" << _groupMap.size());
	if (group_itr != _groupPool[index].end() && group_itr->second.existMember(
			userId)) {
		MCE_DEBUG("GroupChatManagerI::sendMsg --> if statement ");
		group_itr->second.insertMsg(userId, msgContent);
		set<int> & members = group_itr->second.getMembers();
		set<int>::iterator it = members.begin();
		MCE_DEBUG("Sender :" << userId);
		for (; it != members.end(); ++it) {
			//			TODO:发送消息给每个成员
			MCE_DEBUG("GroupChatManagerI::sendMsg -->Group: " << group_itr->first << " User :" << userId << " send message:" << msgContent << " to user:" << *it);
		}
		MCE_DEBUG("Group id:" << feedId << " size = " << members.size());
	} else {
		MCE_DEBUG("GroupChatManagerI::sendMsg --> else statement ")
		return false;
	}
}

int GroupChatManagerI::_groupMod(const Ice::Long & i) {
	return i % GROUPPOOLSIZE;
}
void GroupChatManagerI::scanGroupMap() {
	MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> Start scanning the groupMap");
	for (int i = 0; i < GROUPPOOLSIZE; ++i) {
		MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> SCAN the groupMap : " << i << " !!!");
		IceUtil::Mutex::Lock lock(_mutexPool[i]);
		map<Ice::Long, Group>::iterator it = _groupPool[i].begin();
		for (; it != _groupPool[i].end();) {
			if (it->second.expired()) {
				MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> group :" << it->first << " has been expired");
				map<Ice::Long, Group>::iterator tmp = it;
				tmp++;
				_groupPool[i].erase(it);
				it = tmp;
				MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> group :" << it->first << " is deleted, the groupMap size :" << _groupPool[i].size());

			} else {
				++it;
				MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> group :" << it->first << " is not expired");
			}
		}
	}

	/*	MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> SCAN the groupMap");
	 map<long, Group>::iterator it = _groupMap.begin();
	 for (; it != _groupMap.end();) {
	 MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> check group :" << it->first);
	 if (it->second.expired()) {
	 MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> group :" << it->first << " has been expired");
	 map<long, Group>::iterator tmp = it;
	 tmp++;
	 _groupMap.erase(it);
	 it = tmp;
	 MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> group :" << it->first << " is deleted, the groupMap size :" << _groupMap.size());

	 } else {
	 ++it;
	 MCE_DEBUG("GroupChatManagerI::scanGroupMap() --> group :" << it->first << " is not expired");
	 }
	 }*/
}

void ScanGroupMapTask::handle() {
	MCE_DEBUG("ScanGroupMapTask::handle()");
	GroupChatManagerI::instance().scanGroupMap();
}
