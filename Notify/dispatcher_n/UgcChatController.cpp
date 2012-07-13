#include "UgcChatController.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <IceUtil/IceUtil.h>
#include <IceUtil/RWRecMutex.h>

#include "DispatchNI.h"
#include "FeedContentAdapter.h"
#include "NotifyDispatcherAdapter.h"
#include "NotifyDispatcherNAdapter.h"
#include "pugixml.hpp"
#include "UserCacheAdapter.h"
#include "../util/PrintUtil.h"

using namespace std;
using namespace xce::notify;
using namespace xce::ugcchat;
using namespace xce::usercache;
using xce::notify::PrintHelper;

bool Group::Expired() const { return time(NULL) - update_time_ > GROUP_TIMEOUT; }

void Group::Join(Ice::Int id) {
	uids_.insert(id);
	update_time_ = time(NULL);
}

void Group::JoinBatch(set<int> & uids) {
	MCE_INFO("Group::JoinBatch. olduids:" << PrintHelper::Print(uids_) << " added uids:" << PrintHelper::Print(uids));
	uids_.insert(uids.begin(), uids.end());
	update_time_ = time(NULL);
}

// input : uid列表，逗号作为分隔符
// return : uid 列表，Int
//void ParseToId(const string& to, set<Ice::Int> & id_set) {
//	MCE_DEBUG("toIdParser : to=" << to);
//	std::vector<std::string> idSeq;
//	boost::split(idSeq, to, boost::is_any_of(", "), boost::token_compress_on);
//
//	Ice::Int target;
//	BOOST_FOREACH(string & id,idSeq){
//		try {
//			target = boost::lexical_cast<Ice::Int>(id);
//		} catch (boost::bad_lexical_cast &) {
//			MCE_WARN("ParseToId error. to:" << to);
//			continue;
//		}
//		if (target > 0) {
//			id_set.insert(target);
//		}
//	}
//}

bool Group::SendMessage(Ice::Long nid, Ice::Int from_uid,
		const MyUtil::Str2StrMap& props, NotifyInfo & ninfo) {
	return true;
	//
	//	set<Ice::Int> to_ids;
	//	ParseToId(props["to"], to_ids);
	//
	//	stringstream real_time_ss;
	//	int real_time_count = 0;
	//	//如果这个临时群中的某个成员不在to_ids中，那么把他加到realtime_props的to中，to_ids和realtime_props没有交集
	//	for (set<GroupMember>::iterator i = members_.begin(); i != members_.end(); ++i) {
	//		// 发给自己的pager
	//		//if (i->id == from_uid) {
	//		//  continue;
	//		//}
	//		if (to_ids.find(i->id) != to_ids.end()) {
	//			continue;
	//		}
	//		if (real_time_count++ > 0) {
	//			real_time_ss << ',';
	//		}
	//		real_time_ss << i->id;
	//	}
	//
	//	MyUtil::Str2StrMap realtime_props = props;
	//	realtime_props["to"] = real_time_ss.str(); //realtime_props包含时化的id
	//
	//
	//	//在临时群里的，但是不在to里，不存储
	//	if (real_time_count > 0) {
	//		MCE_INFO("NotifyDispatcherNAdapter to pager(no store) : " << realtime_props["to"] << " nid:" << nid
	//				<< " source:" << realtime_props["feed_source"] << " from:" << realtime_props["from"] << " type:" << props.find("type")->second);
	//		TaskManager::instance().execute(new DispatchTask(kTaskLevelUgcChat, nid,
	//				realtime_props, false));
	//	}
	//	//在to里的，存储
	//	MCE_INFO("NotifyDispatcherNAdapter to notify(do store): " << to_it->second << "-" << to_ids.size() << " nid:"
	//			<< nid << " source:" << realtime_props["feed_source"] << " from:" << realtime_props["from"] << " type:" << props.find("type")->second);
	//	if (!to_ids.empty()) {
	//		TaskManager::instance().execute(new DispatchTask(kTaskLevelUgcChat, nid,
	//				props, true));
	//	}
	//	return true;
}
bool Group::Kick(int m) {  return true; }//TODO 暂时不实现
void Group::Quit(int m) {
	if (!uids_.erase(m))  MCE_INFO("member no in group, m:" << m);
	update_time_ = time(NULL);
}
string Group::ToString() const {
	ostringstream os;
	os << " uids_:" << PrintHelper::Print(uids_) << " update_time_:" << update_time_;
	return os.str();
}
int Group::BroadcastNotify(const GroupKey & key) const {
	return 0; // 暂时关闭该功能
	//	MCE_DEBUG("BroadcastNotify() feed_actor=" << key.feed_actor);
	//	if (members_.size() < 3) {
	//		return 0;
	//	}
	//
	//	xce::feed::FeedDataPtr feed_data = 0;
	//	try {
	//		//feed_data = xce::feed::FeedContentAdapter::instance().getFeed(key.feed_source, key.feed_stype, key.feed_actor);
	//	} catch (Ice::Exception& e) {
	//		MCE_WARN("FeedContentAdapter getFeed() err : " << e << " feed_actor=" << key.feed_actor);
	//	}
	//	if (!feed_data) {
	//		return -1;
	//	}
	//	pugi::xml_document doc;
	//	MCE_DEBUG("xml : " << feed_data->xml);
	//	if (!doc.load(feed_data->xml.c_str()))
	//		return -1;
	//	pugi::xml_node status = doc.child("f").child("status");
	//
	//	MyUtil::Str2StrMap props;
	//	props["schema_id"] = "80";
	//	props["type"] = "80";
	//	stringstream ss_source;
	//	ss_source << key.feed_source;
	//	props["source"] = ss_source.str();
	//	stringstream ss_owner;
	//	ss_owner << key.feed_actor;
	//	props["owner"] = ss_owner.str();
	//
	//	props["from"] = "0";
	//	stringstream t;
	//	t << time(NULL);
	//	props["time"] = t.str();
	//	props["doing_id"] = status.child("id").first_child().value();
	//
	//	string content = status.child("title").first_child().value();
	//	boost::replace_all(content, "&apos;", "&quot;");
	//	props["doing_content"] = content;
	//
	//	stringstream to;
	//	set<GroupMember>::const_iterator i = members_.begin();
	//	for (; i != members_.end(); ++i) {
	//		if (i->id == key.feed_actor) {
	//			continue;
	//		}
	//		if (!to.str().empty()) {
	//			to << ',';
	//		}
	//		to << i->id;
	//	}
	//	props["to"] = to.str();
	//	MCE_INFO("BroadcastNotify() to=" << to.str());
	//
	//	try {
	//		NotifyDispatcherAdapter::instance().dispatch(props);
	//	} catch (Ice::Exception& e) {
	//		MCE_WARN("NotifyDispatcherAdapter::dispatch() err : " << e.what());
	//	}
	//	return members_.size();
}

//----------------------------------------------------------
bool UgcChatController::SendMessage(Ice::Long nid,
		const MyUtil::Str2StrMap& props, NotifyInfoPtr & ninfo) {
	AddFromIdToGroup(nid, ninfo);
	AddTargetsToGroup(nid, ninfo);
	if (!ninfo->is_at_) { //不是@，都需要把临时群群员加到目标中
		AddUgcMembersToNotifyInfo(ninfo);
	}
	TaskManager::instance().execute(new DispatchTask(kTaskLevelUgcChat, nid,
			props, ninfo));
	return true;
	//	return group.SendMessage(nid, from_uid, props,ninfo);
}

bool UgcChatController::Join(Ice::Int feed_actor, Ice::Long feed_source,
		Ice::Int feed_stype, Ice::Int user_id) {
	return true;
}

bool UgcChatController::Kick(Ice::Int feed_actor, Ice::Long feed_source,
		Ice::Int feed_stype, Ice::Int user_id) {
	return true;
}

string GetValue(const MyUtil::Str2StrMap& props, const string &key) {
	MyUtil::Str2StrMap::const_iterator it = props.find(key);
	string value;
	if (it != props.end()) {
		value = it->second;
	}
	return value;
}

template<class ResultType>
ResultType GetTypedValue(const MyUtil::Str2StrMap & props, const string & key) {
	string v = GetValue(props, key);
	ResultType r = -1;
	if (!v.empty()) {
		try {
			r = boost::lexical_cast<ResultType>(v);
		} catch (boost::bad_lexical_cast & e) {
			MCE_WARN("parse error : " << key << '-' << v << '-' << e.what());
		}
	}
	return r;
}

void UgcChatController::AddFromIdToGroup(Ice::Long nid, NotifyInfoPtr & ninfo) {
	int index = ninfo->feed_source_ % GROUPPOOLSIZE;
	IceUtil::Mutex::Lock lock(mutex_pool_[index]);
	//	MCE_INFO("UgcChatController::SendMessage. ugc chat. nid=" << nid << " from=" << from_uid << " is_at:" << is_at << " to: " << to);
	Group & group = group_pool_[index][GroupKey(ninfo->feed_actor_,
			ninfo->feed_source_, ninfo->feed_stype_)];
	group.Join(ninfo->from_uid_);
}
void UidToGroupMember(set<int> uids, map<int, UserCachePtr> & result, set<
		GroupMember> & res) {
	BOOST_FOREACH(int uid,uids){
		map<int,UserCachePtr>::iterator mit = result.find(uid);
		if(mit != result.end()) {
			GroupMember gm(uid,mit->second->name(),mit->second->tinyurl());
			res.insert(gm);
		}
	}
}
void UgcChatController::AddTargetsToGroup(Ice::Long nid, NotifyInfoPtr & ninfo) {
	int index = ninfo->feed_source_ % GROUPPOOLSIZE;
	IceUtil::Mutex::Lock lock(mutex_pool_[index]);
	Group & group = group_pool_[index][GroupKey(ninfo->feed_actor_,
			ninfo->feed_source_, ninfo->feed_stype_)];
//	set<int> targets;
//	targets.insert(ninfo->uids_.begin(), ninfo->uids_.end());
//	targets.insert(ninfo->minigroup_members_.begin(),
//			ninfo->minigroup_members_.end());
	group.JoinBatch(ninfo->uids_);
}
void UgcChatController::AddUgcMembersToNotifyInfo(NotifyInfoPtr & ninfo) {
	int index = ninfo->feed_source_ % GROUPPOOLSIZE;
	IceUtil::Mutex::Lock lock(mutex_pool_[index]);
	Group & group = group_pool_[index][GroupKey(ninfo->feed_actor_,
			ninfo->feed_source_, ninfo->feed_stype_)];
	const set<int> & ugc_uids = group.GetUids();
	//MCE_INFO("UgcChatController::AddUgcMembersToNotifyInfo.nid:" << ninfo->nid_
	//		<< " ugc_uids:" << PrintHelper::Print(ugc_uids));
	ninfo->AddUgcTargets(ugc_uids);
}

void UgcChatController::ScanGroupMap() {
	MCE_INFO("UgcChatController::ScanGroupMap() --> Start scanning the groupMap");
	int left = 0, erased = 0;
	for (int i = 0; i < GROUPPOOLSIZE; ++i) {
		IceUtil::Mutex::Lock lock(mutex_pool_[i]);
		map<GroupKey, Group>::iterator it = group_pool_[i].begin();
		while (it != group_pool_[i].end()) {
			if (it->second.Expired()) {
				++erased;
				MCE_INFO("UgcChatController::ScanGroupMap() --> group :"
						<< it->first.feed_actor << '-'
						<< it->first.feed_source << '-'
						<< it->first.feed_stype << " has expired.");
				it->second.BroadcastNotify(it->first);
				group_pool_[i].erase(it++);
			} else {
				++left;
				++it;
			}
		}
	}
	MCE_INFO("UgcChatController::ScanGroupMap() complete. " << erased << " erased, " << left << " left.");
}

void UgcChatController::QuitChat(int uid, const GroupIndexSeq & groups) {
	MCE_DEBUG("UgcChatController::QuitChat uid:" << uid << ", quit groups size:" << groups.size());
	BOOST_FOREACH(const GroupIndex & gi,groups){
		int index = gi.source % GROUPPOOLSIZE;
		IceUtil::Mutex::Lock lock(mutex_pool_[index]);
		map<GroupKey, Group>::iterator itr = group_pool_[index].find(GroupKey(gi.actor, gi.source, gi.stype));
		if (itr != group_pool_[index].end())  itr->second.Quit(uid);
		else MCE_INFO("group not exist  src:"<< gi.source << " stype:"<< gi.stype << " actor:"<< gi.actor );
	}
}
void UgcChatController::JoinChat(const vector<int> & uids, const GroupIndex & gindex) {
	int idx = gindex.source % GROUPPOOLSIZE;
	IceUtil::Mutex::Lock lock(mutex_pool_[idx]);
	Group & group = group_pool_[idx][GroupKey(gindex.actor,gindex.source,gindex.stype)];
	set<int> uid_set(uids.begin(),uids.end());
	group.JoinBatch(uid_set);
	MCE_INFO("UgcChatController::JoinChat: gindex " << PrintGroupIndex(gindex) << " uids:" << PrintHelper::Print(uids) );
}
void ScanGroupMapTask::handle() {
	MCE_DEBUG("ScanGroupMapTask::handle()");
	UgcChatController::instance().ScanGroupMap();
}
//void UgcChatController::QuitChat(int uid, const GroupIndexSeq & groups) {
//	MCE_DEBUG("UgcChatController::QuitChat uid:" << uid << ", quit groups size:" << groups.size());
//	for (size_t i = 0; i != groups.size(); ++i) {
//		const GroupIndex & gi = groups.at(i);
//		int index = gi.source % GROUPPOOLSIZE;
//		IceUtil::Mutex::Lock lock(mutex_pool_[index]);
//		map<GroupKey, Group>::iterator itr = group_pool_[index].find(GroupKey(
//				gi.actor, gi.source, gi.stype));
//		if (itr != group_pool_[index].end()) {
//			itr->second.Quit(uid);
//		} else {
//			MCE_INFO("group not exist  src:"<< gi.source << " stype:"<< gi.stype << " actor:"<< gi.actor )
//		}
//	}
//}


