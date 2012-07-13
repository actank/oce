#ifndef __GROUPCHAT_I_H__
#define __GROUPCHAT_I_H__

#include <ctime>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include <IceUtil/Shared.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <boost/foreach.hpp>
#include "TaskManager.h"
#include "Singleton.h"
#include "Util.h"
#include "Notify.h"
#include "DispatchNI.h"

namespace xce {
namespace ugcchat {

using namespace std;
using namespace MyUtil;
using namespace xce::notify;

const int SCANTIME = 2 * 60 * 1000; // 定时扫描群的时间间隔
const int GROUP_TIMEOUT = 60 * 5; //不活动群的生存时期
const int GROUPPOOLSIZE = 1000;

static string PrintGroupIndex(const GroupIndex & index){
	ostringstream os;
	os << " actor:" << index.actor << " source:" << index.source << " stype:" << index.stype;
	return os.str();
}
class ScanGroupMapTask: public MyUtil::Timer {
public:
	ScanGroupMapTask() : MyUtil::Timer(SCANTIME) { }
	virtual void handle();
};
class UgcMessage {
public:
	UgcMessage(const int & s, const string & c) : sender(s), content(c) { }
private:
	Ice::Int sender;
	string content;
};
struct GroupKey {
	GroupKey(Ice::Int a, Ice::Long s, Ice::Int t) :
		feed_actor(a), feed_source(s), feed_stype(t) {
	}
	Ice::Int feed_actor;
	Ice::Long feed_source;
	Ice::Int feed_stype;
	bool operator <(const GroupKey & key) const {
		if (key.feed_actor != feed_actor)  return feed_actor < key.feed_actor;
		if (key.feed_source != feed_source)  return feed_source < key.feed_source;
		return feed_stype < key.feed_stype;
	}
};
struct GroupMember {
	GroupMember(Ice::Int i, const string & n, const string & t) :
		id(i), name(n), tiny_url(t) {
	}
	Ice::Int id;
	string name;
	string tiny_url;
	bool operator <(const GroupMember & mem) const { return id < mem.id; }
};
// TODO : Group和GroupKey可以合并起来
class Group {
public:
	//Group(Ice::Int a, Ice::Long s, Ice::Int t);
	Group() : update_time_(time(NULL)) { }
	bool Expired() const;
	void Join(Ice::Int uid);
	void JoinBatch(set<int> & uids) ;
	bool Kick(int m);
	int BroadcastNotify(const GroupKey & key) const;
	bool SendMessage(Ice::Long nid, Ice::Int from_uid,
			const MyUtil::Str2StrMap& props, NotifyInfo & ninfo);
	const set<int> & GetUids()const{ return uids_; }
	void Quit(int uid);
	string ToString() const;
private:
	set<int> uids_;
	time_t update_time_;
};

class UgcChatController: public Singleton<UgcChatController> {
public:
	// 如果需要发送notify, 则返回true, 否则返回false.
	bool SendMessage(Ice::Long nid, const MyUtil::Str2StrMap& props,
			NotifyInfoPtr & ninfo);
	void ScanGroupMap();
	void QuitChat(int uid, const GroupIndexSeq & groups);
	void JoinChat(const vector<int> & uids, const GroupIndex & gindex) ;
private:
	bool Join(Ice::Int feed_actor, Ice::Long feed_source, Ice::Int feed_stype,
			Ice::Int userId);
	bool Kick(Ice::Int feed_actor, Ice::Long feed_source, Ice::Int feed_stype,
			Ice::Int user_id);
	void AddFromIdToGroup(Ice::Long nid, NotifyInfoPtr & ninfo);
	void AddTargetsToGroup(Ice::Long nid, NotifyInfoPtr & ninfo);
	void AddUgcMembersToNotifyInfo(NotifyInfoPtr & ninfo);
	//	set<int> GetNotStoreTargets(Ice::Long nid, NotifyInfoPtr & ninfo);
	// 以 feed_actor % GROUPPOOLSIZE 作为pool index
	map<GroupKey, Group> group_pool_[GROUPPOOLSIZE];
	IceUtil::Mutex mutex_pool_[GROUPPOOLSIZE];
};

}
}

#endif
