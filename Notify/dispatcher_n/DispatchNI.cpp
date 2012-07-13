#include "DispatchNI.h"
#include "QueryRunner.h"
#include "Notify/util/schema_manager.h"
#include "UgcChatController.h"
#include "NotifyContentAdapter.h"
#include "NotifyIndexAdapter.h"
#include "NotifyIndexNAdapter.h"
#include "Notify/util/rmessage_util.h"
#include "NotifyGateAdapter.h"
#include "FeedGroupAdapter.h"
#include "../util/PrintUtil.h"
#include "../util/time_stat.h"
#include "boost/tokenizer.hpp"
#include "boost/typeof/typeof.hpp"
using namespace xce::notify;
using namespace xce::ugcchat;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using xce::feed::FeedGroupAdapter;
using xce::notify::PrintHelper;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&DispatchNManagerI::instance(),
			service.createIdentity("M", ""));
	ThreadPoolConfig activity_cfg(1, 6);
	TaskManager::instance().config(kTaskLevelActivity, activity_cfg);
	ThreadPoolConfig ugcchat_cfg(4, 20);
	TaskManager::instance().config(kTaskLevelUgcChat, ugcchat_cfg);
	//  ThreadPoolConfig insertdb_cfg(4, 20);
	//  TaskManager::instance().config(insertdb_cfg, activity_cfg);
	TaskManager::instance().scheduleRepeated(new ScanGroupMapTask());
}

void DispatchNManagerI::EnableChat(bool enable, const Ice::Current&) {
	MCE_INFO("DispatchManagerI::EnableChat() : " << enable);
	enable_chat_ = enable;
}
void DispatchNManagerI::SpecialProcess(const MyUtil::Str2StrMap& props,
		int & task_level, bool& store) {
	if (props.find("schema_id")->second == "48") { task_level = kTaskLevelActivity;} //干旱捐款活动，应该没用了
	props.find("schema_id")->second;
	Str2StrMap::const_iterator i = props.find("schema_id");
	if (i != props.end() && i->second == "193") {
		store = false;//193是A在群回复B(无气泡，通知用)
	}
}
bool DispatchNManagerI::reloadSchema(const Ice::Current&) {
	MCE_INFO("DispatchNManagerI::reloadSchema");
	return SchemaManager::instance().reload();
}
void DispatchNManagerI::dispatchN(Ice::Long nid, bool store,
		const MyUtil::Str2StrMap& props, const Ice::Current&) {
	NotifyInfoPtr ninfo = new NotifyInfo;
	ninfo->Init(nid, props, store);
	if (enable_chat_ && ninfo->CanChat()) {
		UgcChatController::instance().SendMessage(nid, props, ninfo);
		MCE_INFO("DispatchNManagerI::dispatchN.Do ugc chat. nid=" << nid
				<<  ninfo->ToString());
		return;
	}
	int task_level = 0;
	SpecialProcess(props, task_level, store);
	ninfo->SetStore(store);
	TaskManager::instance().execute(new DispatchTask(task_level, nid, props,
			ninfo));
	MCE_INFO("DispatchNManagerI::dispatchN.No ugc chat. nid=" << nid
			<< ninfo->ToStringSimple() << " tmp store:" << store);
}
void DispatchNManagerI::QuitChat(::Ice::Int uid, const GroupIndexSeq& groups,
		const ::Ice::Current&) {
	if (!enable_chat_) {
		MCE_INFO("DispatchNManagerI::QuitChat chat is already disabled,uid:"<< uid);
		return;
	}
	UgcChatController::instance().QuitChat(uid, groups);
}
void DispatchNManagerI::JoinChat(const vector<int> & uids, const GroupIndex& gindex,
		const ::Ice::Current&) {
	if (!enable_chat_) {
		MCE_INFO("DispatchNManagerI::QuitChat chat is already disabled,gindex " << PrintGroupIndex(gindex) << " uids:" << PrintHelper::Print(uids) );
		return;
	}
	UgcChatController::instance().JoinChat(uids, gindex);
	MCE_INFO("DispatchNManagerI::JoinChat: gindex " << PrintGroupIndex(gindex) << " uids:" << PrintHelper::Print(uids) );
}
void DispatchTask::ToNotifyGate(NotifyContentPtr & content) {
	//MCE_INFO("@@DispatchTask::ToNotifyGate. nid=" << content->id);
	set<int> instant_targets = ninfo_->GetInstantTargets();
	set<int> pager_targets = ninfo_->GetWebpagerTargets();
	try {
		NotifyGateAdapter::instance().instantNotify(content, instant_targets);
		NotifyGateAdapter::instance().deliverWebpager(content, pager_targets);
	} catch (Ice::Exception & e) {
		MCE_WARN("NotifyGateAdapter instantNotify error : " << e);
	} catch (...) {
		MCE_WARN("NotifyGateAdapter instantNotify unknown error.");
	}
	MCE_INFO("DispatchTask::ToNotifyGate:nid=" << content->id
			<< " type:" << ((content->type) & 65535) << " from:" << content->from
			<< " instant_targets:" << PrintHelper::Print(instant_targets)
		<< " pager_targets:" << PrintHelper::Print(pager_targets)
	);
}
void DispatchTask::ToIndexAndContent(NotifyContentPtr & content) {
	try {
		NotifyContentAdapter::instance().addNotify(content);
	} catch (Ice::Exception & e) {
		MCE_WARN("DispatchTask::DispatchStore err : " << e);
		return; //Content失败后就不能再调用index服务
	}
	set<int> to_uidset = ninfo_->GetStoreUids();
	to_uidset.erase(ninfo_->from_uid_);
	//MCE_INFO("tmplog.DispatchTask::ToIndexAndContent: nid:" << content->id << " all targets:" << PrintHelper::Print(to_uidset,1000000));
	try {
		MyUtil::IntSeq to_uids(to_uidset.begin(), to_uidset.end());
		MCE_DEBUG("DispatchTask::ToIndexAndContent--> Call NotifyIndex.addNotify");
		int type = RMessageUtil::instance().Util_GetRMessageType(content->type);
		if(247 == type || 256 == type || 258 == type || 259 == type || 260 == type || 261 == type || 262 == type || 264 == type || 265 == type || 266 == type || 269 == type || 279 == type){
			NotifyIndexNAdapter::instance().addNotify(content, to_uids);
		}else{
			NotifyIndexAdapter::instance().addNotify(content, to_uids);
			NotifyIndexNAdapter::instance().addNotify(content, to_uids);
		}
		MCE_DEBUG("DispatchTask::ToIndexAndContent->id:" << content->id << "type:" << content->type );
	} catch (Ice::Exception & e) {
		MCE_WARN("index adapter addNotify err : " << e.what());
		return;
	} catch (...) {
		MCE_WARN("index adapter addNotify unknown err");
		return;
	}
	TaskManager::instance().execute(new DbInsertTask(kTaskLevelInsertDb,to_uidset, content));
}

//Create Table: CREATE TABLE `content_11` (
//  `id` bigint(20) NOT NULL AUTO_INCREMENT,
//  `source` bigint(20) NOT NULL DEFAULT '0',
//  `owner` int(11) NOT NULL DEFAULT '0',
//  `from_id` int(11) NOT NULL DEFAULT '0',
//  `type` mediumint(8) NOT NULL DEFAULT '0',
//  `props` varchar(2048) COLLATE utf8_bin NOT NULL DEFAULT '',
//  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
//  PRIMARY KEY (`id`),
//  KEY `time` (`time`)
//) ENGINE=InnoDB AUTO_INCREMENT=19109598212 DEFAULT CHARSET=utf8 COLLATE=utf8_bin
NotifyContentPtr DispatchTask::InitContent() {
	NotifyContentPtr  content = new NotifyContent;
	content->id = notify_id_;
	try {
		content->source = boost::lexical_cast<Ice::Long>(props_["source"]);
		props_.erase("source"); // 必须erase
		content->owner = boost::lexical_cast<int>(props_["owner"]);
		props_.erase("owner");
		content->from = boost::lexical_cast<int>(props_["from"]);
		props_.erase("from");
		//content->type = boost::lexical_cast<int>(props_["type"]);
		//add by guoqing.liu add bigtype 
		int type = boost::lexical_cast<int>(props_["type"]);
		int bigtype = SchemaManager::instance().GetBigtypeBytype(type);
		if(bigtype == -1){
			bigtype = 0;
		}
		MCE_DEBUG("DispatchTask::InitContent-->type:" << type << " bigtype:" << bigtype);
		type |= (bigtype << 16);
		MCE_DEBUG("DispatchTask::InitContent-->result type:" << type);
		content->type = type;
		props_.erase("type");
	} catch (boost::bad_lexical_cast & e) {
		MCE_WARN("DispatchTask::InitContent.keys lexical_cast error : " << e.what());
		return 0;
	}
	int sid = 0;
	try {
		sid = boost::lexical_cast<int>(props_["schema_id"]);
	} catch (boost::bad_lexical_cast &) {
		MCE_WARN("DispatchTask::InitContent.schemaid is_reply parse error : sid=" << sid << " nid:" << notify_id_);
		return 0;
	}
	content->isreply = SchemaManager::instance().isReply(sid);
	content->time = time(NULL);
	ostringstream os;
	static const unsigned max_len = 1024;
	for (MyUtil::Str2StrMap::iterator it = props_.begin(); it != props_.end(); ++it) {
		if (it != props_.begin()) { os << "\t"; }
		for (size_t i = 0; i < it->second.size(); ++i) {
			if (it->second[i] == '\t') it->second[i] = ' ';
		}
		//限制长度
		string key = it->first;
		string value = it->second;
		if (key == "doing_content" && value.size() > max_len) {
			MCE_DEBUG("DispatchTask::handle. props value > " << max_len);
			value = value.substr(0, max_len);
		}
		os << key << '\t' << value;
	}
	content->props = os.str();
	return content;
}
struct TimeRecord{
	string ToString(){
		ostringstream os;
		os << " idxcnt:" << idxcnt << " gate:" << gate;
		return os.str();
	}
	float idxcnt;
	float gate;
};
void DispatchTask::handle() {
	if(ninfo_->GetInstantTargets().empty() && ninfo_->GetWebpagerTargets().empty()){
		MCE_INFO("DispatchTask::handle. target empty. nid:" << ninfo_->nid_ << " from:" << ninfo_->from_uid_);
		return;
	}

	TimeRecord tc;
	TimeStat st;
	NotifyContentPtr content = InitContent();
	if (!content) {
		MCE_WARN("DispatchTask::handle. init content error. nid:" << ninfo_->nid_ );
		return;
	}
	if (ninfo_->ShouldStore()) {
		ToIndexAndContent(content);
	}
	tc.idxcnt = st.getTime();
	ToNotifyGate(content);
	tc.gate = st.getTime();
	MCE_INFO("DispatchTask::handle: nid:" << ninfo_->nid_ << " from:" << ninfo_->from_uid_ <<" cost. " << tc.ToString());
}
// input : uid列表，逗号作为分隔符
// return : uid 列表，Int
//MyUtil::IntSeq DispatchTask::toIdParser(const string& to, Ice::Int from,
//		int type) {
//	MCE_DEBUG("toIdParser : to=" << to << " line:" << __LINE__);
//	MyUtil::IntSeq idSeq;
//	set<int> id_set;
//	size_t begin = 0;
//	size_t end;
//	int target;
//	// TODO : use boost::split()
//	while ((end = to.find(',', begin)) != string::npos) {
//		try {
//			target = boost::lexical_cast<int>(to.substr(begin, end - begin));
//		} catch (boost::bad_lexical_cast &) {
//			MCE_WARN("toIdParser parse error : to=" << to << " line:" << __LINE__);
//			return idSeq;
//		}
//		if (target > 0 && target != from ) { id_set.insert(target); }
//		begin = end + 1;
//	}
//	try {
//		target = boost::lexical_cast<int>(to.substr(begin, to.length() - begin));
//		if (target > 0 && target != from ) { id_set.insert(target); }
//	} catch (boost::bad_lexical_cast &) {
//		MCE_WARN("toIdParser parse error : to=" << to<< " line:" << __LINE__);
//	}
//	return idSeq.assign(id_set.begin(),id_set.end());
//}
bool DbInsertTask::InsertContent() {
	MCE_DEBUG("DbInsertTask::InsertContent : nid=" << content_->id);
	string table = GetContentTable(content_->id);
	Statement sql;
	sql << "insert into " << table
			<< " (id,source, owner, from_id,type,props,time) values (" << content_->id
			<< " , " << content_->source << "," << content_->owner << ","
			<< content_->from << "," << ((content_->type) & 65535) << "," << mysqlpp::quote
			<< content_->props << ", from_unixtime(" << content_->time << ") )";
	//content_中的isreply没有写入Content表
	try {
		mysqlpp::SimpleResult res = QueryRunner(DB_INSTANCE, CDbWServer, table).execute(sql);
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("insertNotifyContent sql error --> " << e.what());
		return false;
	} catch (...) {
		MCE_WARN("insertNotifyContent  error --> "__FILE__ << ":" << __LINE__);
		return false;
	}
	return true;
}
bool DbInsertTask::InsertIndexBatch() {
	if(targets_.empty()) return true;
	map<int,set<int> > idx_toids_map;
	BOOST_FOREACH(int toid,targets_){
		idx_toids_map[toid % TABLE_COUNT].insert(toid);//TODO
	}
	typedef map<int,set<int> >::value_type vt;
	BOOST_FOREACH(vt & p,idx_toids_map){
		string table = GetIndexTable(p.first);
		Statement sql;
		sql << " insert into " << table << " (id, to_id, type, source, from_id, time, is_reply, bigtype) values ";
		bool first = true;
		BOOST_FOREACH(int toid,p.second){
			if(first) first = false;
			else sql << ",";
			sql << " ( " << content_->id << "," << toid << "," << RMessageUtil::instance().Util_GetRMessageType(content_->type) << ","
					<< content_->source << ", " << content_->from << ", from_unixtime("
					<< content_->time << "), " << content_->isreply << ", " << RMessageUtil::instance().Util_GetRMessageBigtype(content_->type) << ")";
		}
		try {
			QueryRunner(DB_INSTANCE, CDbWServer, table).execute(sql);
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("DbInsertTask::InsertIndexBatch.nid:" << content_->id << " table:" << table << " " << e.what());
			return false;
		} catch (...) {
			MCE_WARN("DbInsertTask::InsertIndexBatch.nid:" << content_->id << " table:" << table << " unknown err");
			return false;
		}
	}
	return true;
}
bool DbInsertTask::InsertIndex(int toid) {
	if (toid <= 0)  return true;
	MCE_DEBUG("insertNotifyIndex : toid=" << toid);
	ostringstream table;
	table << "index_" << toid % TABLE_COUNT;
	Statement sql;
	sql << "insert into " << table.str()
			<< " (id, to_id, type, source, from_id, time, is_reply) values ("
			<< content_->id << "," << toid << "," << ((content_->type) & 65535) << ","
			<< content_->source << ", " << content_->from << ", from_unixtime("
			<< content_->time << "), " << content_->isreply << ")";
	try {
		QueryRunner(DB_INSTANCE, CDbWServer, table.str()).execute(sql);
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("insertNotifyContent error : " << e.what());
		return false;
	} catch (...) {
		MCE_WARN("insertNotifyContent unknown error");
		return false;
	}
	return true;
}
void DbInsertTask::handle() {
	float cnt,idx;
	TimeStat ts;

	if(!InsertContent()) return;//失败之后不能调用InsertIndex
	cnt = ts.getTime();

	InsertIndexBatch();
	idx = ts.getTime();

	MCE_INFO("DbInsertTask::handle. nid:" <<content_->id
			<< " from:" << content_->from
			<<" db cost, content:" << cnt << " index:" << idx);
	//	BOOST_FOREACH(int toid,targets_){
	//		InsertIndex(toid);
	//	}
}
//GetStoreUids和GetInstantTargets现在是相同的
set<int> NotifyInfo::GetStoreUids() {
	set<int> res = uids_;
	res.erase(from_uid_);
	return res;
}
set<int> NotifyInfo::GetInstantTargets(){//推给webpager,im,3g的
	set<int> res = uids_;
	res.erase(from_uid_);
	return res;
}
set<int> NotifyInfo::GetWebpagerTargets(){//只推给webpager
	set<int> res = ugc_uids_;
	BOOST_FOREACH(int i,uids_){
		res.erase(i);
	}
	res.erase(from_uid_);
	return res;
}
string NotifyInfo::ToString() const {
	ostringstream os;
	os << " store_:" << store_ << " to_str_:" << to_str_ << " tominigroup_str_:"
			<< tominigroup_str_ << " from_uid_:" << from_uid_ << " feed_actor_:"
			<< feed_actor_ << " feed_source_:" << feed_source_ << " feed_stype_:"
			<< feed_stype_ << " from_name_:" << from_name_ << " from_pic_:"
			<< from_pic_ << " ugc_content_:" << ugc_content_ << " schema_id_:"
			<< schema_id_ << " is_at_:" << is_at_ << " is_whisper_:" << is_whisper_
			<< " uids_:" << PrintHelper::Print(uids_)
			<< " minigroups_:"<< PrintHelper::Print(minigroups_)
			<< " minigroup_members_:"	<< PrintHelper::Print(minigroup_members_)
			<< " ugc_uids_:" << PrintHelper::Print(ugc_uids_)
			;
	return os.str();
}
string NotifyInfo::ToStringSimple() const {
	ostringstream os;
	os << " store_:" << store_ << " to_str_:" << to_str_ << " tominigroup_str_:"
			<< tominigroup_str_ << " from_uid_:" << from_uid_
			<< " schema_id_:"		<< schema_id_
			<< " uids_:" << PrintHelper::Print(uids_)
			<< " minigroups_:"<< PrintHelper::Print(minigroups_)
			<< " minigroup_members_:"			<< PrintHelper::Print(minigroup_members_)
			<< " ugc_uids_:" << PrintHelper::Print(ugc_uids_)
			;
	return os.str();
}
void NotifyInfo::AddUgcTargets(const set<int> & ugc_uids) {
	ugc_uids_ = ugc_uids;
}
bool IsAt(int type){
	return type == 196 || type == 197 || type == 203;//TODO 只是实时化里的@,普通@可以不处理
}

void NotifyInfo::GetMiniGroupMembers(){
	try {
		string orig_uids = PrintHelper::Print(uids_);
		vector<int> tmp(minigroups_.begin(), minigroups_.end());
		map<int, vector<int> > gid2uids =
				FeedGroupAdapter::instance().GetMembersInGroups(tmp);
		typedef map<int, vector<int> >::value_type P;
		BOOST_FOREACH(P & p,gid2uids){
			minigroup_members_.insert(p.second.begin(),p.second.end());
			uids_.insert(p.second.begin(),p.second.end());
			MCE_INFO("@@NotifyInfo::GetMiniGroupMembers.minigroupid:" << p.first << " members:" << PrintHelper::Print(p.second,1000));
		}
		MCE_INFO("NotifyInfo::GetMiniGroupMembers. orig_uids:" << orig_uids
					<< " minigroupmembers:" << PrintHelper::Print(minigroup_members_)
					<< " final_uids:" <<  PrintHelper::Print(uids_) );
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyInfo::GetMiniGroupMembers. err : " << e << " nid:"<< nid_);
	}
}
void NotifyInfo::Init(Ice::Long nid, const MyUtil::Str2StrMap& props,
		bool store) {
	store_ = store;
	nid_ = nid;
	from_uid_ = IdParser::GetTypedValue<Ice::Int>(props, "from");
	feed_actor_ = IdParser::GetTypedValue<Ice::Int>(props, "feed_actor");
	feed_source_ = IdParser::GetTypedValue<Ice::Long>(props, "feed_source");
	feed_stype_ = IdParser::GetTypedValue<Ice::Int>(props, "feed_stype");
	int type = IdParser::GetTypedValue<Ice::Int>(props, "type");
	is_at_ = IsAt(type);
	from_name_ = IdParser::GetValue(props, "from_name");
	from_pic_ = IdParser::GetValue(props, "from_pic");
	ugc_content_ = IdParser::GetValue(props, "reply_content");
	schema_id_ = IdParser::GetTypedValue<Ice::Int>(props, "schema_id");
	is_whisper_ = IdParser::GetTypedValue<Ice::Int>(props, "is_whisper");
	to_str_ = IdParser::GetValue(props, "to");
	tominigroup_str_ = IdParser::GetValue(props, "tominigroup");
	uids_ = IdParser::Parse(to_str_);
	minigroups_ = IdParser::Parse(tominigroup_str_);

	if(!minigroups_.empty()) GetMiniGroupMembers();
}

//set<int> IdParser::Parse(const string & to) {
//	set<int> idSet;
//	if (to.empty()) {
//		return idSet;
//	}
//	MCE_DEBUG("toIdParser : to=" << to);
//	MyUtil::IntSeq idSeq;
//	size_t begin = 0;
//	size_t end;
//	int target;
//	// TODO : use boost::split()
//	while ((end = to.find(',', begin)) != string::npos) {
//		try {
//			target = boost::lexical_cast<int>(to.substr(begin, end - begin));
//		} catch (boost::bad_lexical_cast &) {
//			MCE_WARN("IdParser::Parse. parse error : to=" << to);
//			return idSet;
//		}
//		if (target > 0) {
//			idSet.insert(target);
//		}
//		begin = end + 1;
//	}
//	try {
//		target = boost::lexical_cast<int>(to.substr(begin, to.length() - begin));
//		if (target > 0) {
//			idSet.insert(target);
//		}
//	} catch (boost::bad_lexical_cast &) {
//		MCE_WARN("IdParser::Parse. parse error : to=" << to);
//	}
//	return idSet;
//}
set<int> IdParser::Parse(const string & to) {
	set<int> idSet;
	if (to.empty()) 	return idSet;
	using namespace boost;
	try {
		tokenizer<char_separator<char> > tknzer(to,char_separator<char>(","));
		typedef tokenizer<char_separator<char> >::value_type vt;
		for(BOOST_AUTO(pos,tknzer.begin());pos != tknzer.end();++pos){
			int target = boost::lexical_cast<int>(*pos);
			if(target > 0) idSet.insert(target);
		}
	}catch (boost::bad_lexical_cast &) {
		MCE_WARN("IdParser::Parse. parse error : to=" << to );
	}
	return idSet;
}
string IdParser::GetValue(const MyUtil::Str2StrMap& props, const string &key) {
	MyUtil::Str2StrMap::const_iterator it = props.find(key);
	if (it != props.end()) return it->second;
	else return "";
}
template<class ResultType>
ResultType IdParser::GetTypedValue(const MyUtil::Str2StrMap & props,
		const string & key) {
	string v = GetValue(props, key);

	if (v.empty()) return -1;
	try {
		return  boost::lexical_cast<ResultType>(v);
	} catch (boost::bad_lexical_cast & e) {
		MCE_WARN("IdParser::GetTypedValue parse error : " << key << '-' << v << '-' << e.what());

	}
	return -1;
}

