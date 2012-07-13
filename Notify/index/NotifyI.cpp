#include "NotifyI.h"

#include "Notify/util/MergeConfig.h"
#include "Notify/util/PrintUtil.h"
#include "NotifyContentAdapter.h"
#include "IqMsgHandlerAdapter.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "Notify/util/schema_manager.h"
#include "Notify/util/rmessage_util.h"
#include "Notify/util/time_stat.h"
#include "NotifyStormAdapter.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace Ice;
using namespace xce::notify;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace boost;

static const Ice::Int kMaxGetLimit = 200;
static const Ice::Int kMinGetLimit = 5;
static string kLimit = " and YEAR(time) >= 2011";
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	NotifyListFactoryI* factory = new NotifyListFactoryI();
	for (size_t i = 0; i < 10; ++i) {
		ostringstream id;
		id << "RL" << i;
		service.registerObjectCache(i, id.str(), factory);
	}

	MergeConfig::instance().Init();
	service.getAdapter()->add(&NotifyIndexManagerI::instance(),
			service.createIdentity("M", ""));
  MyUtil::TaskManager::instance().scheduleRepeated(new EvictTimer());
  NotifyIndexManagerI::instance().AddTester(0);
//  NotifyIndexManagerI::instance().AddTester(359630546);
//  NotifyIndexManagerI::instance().AddTester(359630771);
//  NotifyIndexManagerI::instance().AddTester(326688331);
//  NotifyIndexManagerI::instance().AddTester(272996921);
//  NotifyIndexManagerI::instance().AddTester(273007991);
//  NotifyIndexManagerI::instance().AddTester(220678672);
}


void RemoveBase::CallIqMsgHandlerAndNotifyStrom(){
	if(remove_notifies_.empty()){
		return;
	}

	ostringstream os;
	typedef pair<long, int> longint;
	try {
		bool insertbefore = false;
		BOOST_FOREACH(longint & p,remove_notifies_)
		{
			talk::adapter::IqMsgHandlerAdapter::instance().RemoveNotifyById(uid_,
				p.first);
			if(insertbefore)
				os << ",";
			else
				insertbefore = true;
			os << p.first << "_" << p.second;
		}
	} catch(Ice::Exception& e) {
		MCE_WARN("IqMsgHandlerAdapter::RemoveNotifyById() "
				<< GetString()
				<<" error:" << e);
	}

	map<string,string> m;
	m["user"] = lexical_cast<string>(uid_);
	m["notifies"] = os.str();
	try{
		MCE_DEBUG("RemoveBase::CallIqMsgHandlerAndNotifyStrom. msg:" << os.str() << " uid:" << 	m["user"]);
		NotifyStormAdapter::instance().notify("REMOVE_NOTIFY",m);
	}catch(Ice::Exception& e) {
		MCE_WARN("NotifyStormAdapter::notify() "
				<< GetString()
				<< " error:" << e);
	}
}
void RemoveBase::RemoveInDb(){
	Statement sql;
	GetSql(sql);
	try {
		//MCE_INFO("@@@@@@@@@@@@@@@@@@@@@@@");
		//QueryRunner(NotifyDbHelper::getDbInstance(), CDbWServer, NotifyDbHelper::getIndexTableName(uid_)).store(sql);
		mysqlpp::StoreQueryResult res = QueryRunner(NotifyDbHelper::getDbInstance(), CDbWServer, NotifyDbHelper::getIndexTableName(uid_)).store(sql);
		MCE_INFO("RemoveBase::RemoveInDb--> res:" << res.num_rows());
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("RemoveBase::RemoveInDb "
				<< 	GetString()
				<< " err: " << e.what() << " at " << __FILE__ << ":"
				<< __LINE__);
	} catch (...) {
		MCE_WARN("RemoveBase::RemoveInDb "
				<< 	GetString()
				<<" unknown error at " << __FILE__ << ":" << __LINE__);
	}
}
void RemoveBySource::GetSql(Statement & sql){
	// TODO : table 的primary key, 是单用to_id, 还是用组合index?
	sql << "DELETE FROM " << NotifyDbHelper::getIndexTableName(uid_)
			<< " WHERE to_id = " << uid_ << " AND type = " << type_
			<< " AND source = " << source_;
}

void RemoveBySourceSender::GetSql(Statement & sql){
	sql << "DELETE FROM " << NotifyDbHelper::getIndexTableName(uid_)
			<< " WHERE to_id = " << uid_ << " AND type = " << type_
			<< " AND source = " << source_ << " AND from_id = " << sender_;
}

void RemoveByNids::GetSql(Statement & sql){
	sql << "DELETE FROM " << NotifyDbHelper::getIndexTableName(uid_)
			<< " WHERE to_id = " << uid_ << " AND id IN (";
	bool first = true;
	BOOST_FOREACH(long nid, nids_){
		if(!first){
			sql << ',';
		}
		sql << nid;
		first = false;
	}
	sql << ')';
}

void RemoveByNid::GetSql(Statement & sql){
	sql << "delete from " << NotifyDbHelper::getIndexTableName(uid_)
			<< " where to_id = " << uid_ << " and id = " << nid_;
}
bool RemoveBySource::WouldRemove(const NotifyIndex & idx){
	return RMessageUtil::instance().Util_GetRMessageType(idx.type) ==type_ && idx.source == source_;
}

bool RemoveBySourceSender::WouldRemove(const NotifyIndex & idx){
	return RMessageUtil::instance().Util_GetRMessageType(idx.type) == type_ && idx.source == source_ && idx.from == sender_;
}

bool RemoveByNids::WouldRemove(const NotifyIndex & idx){
	return nids_.count(idx.id);
}

bool RemoveByNid::WouldRemove(const NotifyIndex & idx){
	return idx.id == nid_;
}

bool RemoveByNid::RemoveOneAndStop(){
	return true;
}

void IndexList::_load(LOAD_TYPE type, size_t begin_pos) {
	IceUtil::Mutex::Lock lock(_mutex);
	string ltypestr = type == LTYPE_ALL ? "LTYPE":"LTYPE_REPLY";
	if (_loadAllState == LSTATE_NOMORE) {// 此时loadReplyState也不需要加载了
		MCE_INFO("IndexList::_load. uid:" << _uid
				<< " LOAD_TYPE:" <<  ltypestr
				<<" load nomore");
		return;
	}
	if (type == LTYPE_ALL && _loadAllState == LSTATE_READY && (all_notify_size()
			- begin_pos < _load_all_threshold)) {
		_loadAllState = LSTATE_LOADING;
		MCE_INFO("IndexList::_load. uid:" << _uid
				<< " LOAD_TYPE:" <<  ltypestr
				<< " _load_all_threshold:" << _load_all_threshold
				<< " all_notify_size:" << all_notify_size()
				<< " begin_pos:" << begin_pos);
		TaskManager::instance().execute(new LoadNotifyListTask(LTYPE_ALL, this));
		return;
	}

	if (_loadReplyState == LSTATE_READY && (_reply_list.size() - begin_pos
			< _load_reply_threshold)) {
		_loadReplyState = LSTATE_LOADING;
		MCE_INFO("IndexList::_load. uid:" << _uid
				<< " LOAD_TYPE:" <<  ltypestr
				<< " _load_all_threshold:" << _load_reply_threshold
				<< " replylist_size:" << _reply_list.size()
				<< " begin_pos:" << begin_pos);
		TaskManager::instance().execute(new LoadNotifyListTask(LTYPE_REPLY, this));
		return;
	}
	MCE_INFO("IndexList::_load. not load,uid:" << _uid << " loadtype:" << PrintEnum(type)
			<< " _loadAllState:" << PrintEnum(_loadAllState)
			<< " _loadReplyState:" << PrintEnum(_loadReplyState)
			<< " all_notify_size:" << all_notify_size()
	);
}

Ice::Int IndexList::getCountByType(const MyUtil::IntSeq& types) {
	//  if (types.empty()) {
	//    MCE_DEBUG("getCount empty types : uid=" << _uid);
	//    return 0;
	//  }
	bool all = false;
	if (types.size() == 1 && types.at(0) == -1) {
		all = true;
	}

	{
		IceUtil::Mutex::Lock lock(_mutex);
		if (_loadAllState == LSTATE_NOMORE) {
			if (all) {
				int cnt = _reply_list.size() + _other_list.size();
				MCE_INFO("getCount from memory : all types. uid=" << _uid);
				return cnt;
			}

			set<Ice::Int> type_set;
			for (size_t i = 0; i < types.size(); ++i) {
				type_set.insert(types[i]);
			}
			int count = 0;

			list<NotifyIndex>::iterator it = _reply_list.begin();
			for (; it != _reply_list.end(); ++it) {
				int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
				if (type_set.find(type) != type_set.end()) {
					++count;
				}
			}

			for (it = _other_list.begin(); it != _other_list.end(); ++it) {
				int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
				if (type_set.find(type) != type_set.end()) {
					++count;
				}
			}

			//      MCE_INFO("getCount from memory : uid=" << _uid << " type size:" << type_set.size());
			MCE_INFO("IndexList::getCountByType.get in mem. uid:" << _uid << " count:" << count);
			return count;
		}
	}

	//  MCE_INFO("getCount from database : uid=" << _uid);

	Statement sql;
	sql << "SELECT COUNT(*) AS _count FROM "
			<< NotifyDbHelper::getIndexTableName(_uid) << " WHERE" << " to_id = "
			<< _uid;
	//sql << " and  (time < timestamp(\"2010-4-1 00:00:00\") or time > timestamp(\"2010-4-1 15:00:00\") )";
	sql << " and ( id < 13903218553  or id > 13915001511)" << kLimit;
	if (!all) {
		for (size_t i = 0; i < types.size(); ++i) {
			if (i == 0) {
				sql << " AND type IN (";
			}
			sql << types.at(i);
			if (i != (types.size() - 1))
				sql << ",";
			else
				sql << ")";
		}
	}

	try {
		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbRServer,
				NotifyDbHelper::getIndexTableName(_uid)).store(sql);
		if (!res || res.num_rows() != 1) {
			MCE_WARN("IndexList::getCountByType.getCount error. uid:" << _uid);
			return 0;
		}

		mysqlpp::Row row = res.at(0);
		int count = (Ice::Int) row["_count"];
		MCE_INFO("IndexList::getCountByType.get in db. uid:" << _uid << " count:" << count);
		return count;
	} catch (std::exception& e) {
		MCE_WARN("IndexList::getCountByType.getCount() err :  uid:" << _uid << e.what());
	} catch (...) {
		MCE_WARN("IndexList::getCountByType.getCount() unkown exception uid:" << _uid << " at:" << __FILE__ << ":" << __LINE__);
	}
	return 0;
}

Ice::Int IndexList::getUnreadCount( const vector<int> & types,bool bytype,
		int view) {//view=-1代表所有的
	Ice::Int count = 0;
	ostringstream oss;
	oss << " view: " << view << " _min_unread_nid: " << _min_unread_nid
			<< " _min_notify_id: " << _min_notify_id;
	set<int> type_set(types.begin(),types.end());
	if (_min_unread_nid < _min_notify_id) {

		try {
			Statement sql;
			sql << "SELECT COUNT(*) AS unread_count from "
					<< NotifyDbHelper::getIndexTableName(_uid) << " WHERE to_id = "
					<< _uid << " AND id >=" << _min_unread_nid;
			//sql << " and  (time < \"2010-4-1 00:00:00\" or time > \"2010-4-1 15:00:00\" )";
			sql << " and ( id < 13903218553  or id > 13915001511)" << kLimit;
			if (view == 0) {
				sql << " and is_reply = 1";
			} else if (view == 1) {
				sql << " and is_reply = 0";
			}
			if(bytype && !types.empty()){
				sql << " and type in " << PrintHelper::Print(types,10000);
			}

			mysqlpp::StoreQueryResult res = QueryRunner(
					NotifyDbHelper::getDbInstance(), CDbRServer,
					NotifyDbHelper::getIndexTableName(_uid)).store(sql);

			if (res && !res.empty()) {
				mysqlpp::Row row = res.at(0);
				MCE_INFO("getUnreadCount from database. uid=" << _uid << " count:" << (Ice::Int) row["unread_count"] << oss.str());
				return (Ice::Int) row["unread_count"];
			}
		} catch (std::exception& e) {
			MCE_WARN("loading unread count err .uid:" << _uid << " " << e.what());
		} catch (...) {
			MCE_WARN("Unkown exception loading unread count uid:" << _uid );
		}
	}

	{
		IceUtil::Mutex::Lock lock(_mutex);

		list<NotifyIndex>::iterator it = _reply_list.begin();
		if (view == -1 || view == 0) {
			while (it != _reply_list.end()) {
				int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
				if (it->id < _min_unread_nid){
					break;
				}
				if(bytype && type_set.count(type)){
					++count;
				}else{
					++count;
				}
				++it;
			}
		}
		if (view == -1 || view == 1) {
			it = _other_list.begin();
			while (it != _other_list.end()) {
				int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
				if (it->id < _min_unread_nid){
					break;
				}
				if(bytype && type_set.count(type)){
					++count;
				}else{
					++count;
				}
				++it;
			}
		}
	}
	MCE_INFO("IndexList::getUnreadCount from memory. uid=" << _uid << " count:" << count  << oss.str() << " types:" << types.size());
	return count;
}

string IndexList::dump() {
	IceUtil::Mutex::Lock lock(_mutex);
	stringstream ss;
	ss << "uid=" << _uid << '\n';

	int sz = 0;
	list<NotifyIndex>::iterator it = _reply_list.begin();
	for (; it != _reply_list.end(); ++it) {
		int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
		//int bigtype = RMessageUtil::instance().Util_GetRMessageBigtype(it->type);
		int bigtype = RMessageUtil::instance().Util_GetRMessageBigtype(it->type);
		ss << it->id << "(source=" << type << '-' << it->isreply << '-'
				<< it->source << "-" << bigtype << "),";
		++sz;
	}
	ss << " reply-size=" << sz << '\n';

	sz = 0;
	it = _other_list.begin();
	for (; it != _other_list.end(); ++it) {
		int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
		int bigtype = RMessageUtil::instance().Util_GetRMessageBigtype(it->type);
		ss << it->id << "(source=" << it->type << '-' << it->isreply << '-'
				<< it->source << "-" << bigtype << "),";
		++sz;
	}
	ss << " other-size=" << sz << '\n';

	ss << " _min_notify_id=" << _min_notify_id;
	ss << " _add_count=" << _add_count;
	ss << " _min_unread_nid=" << _min_unread_nid;

	return ss.str();
}


void IndexList::append(list<NotifyIndex>& notifies) {
	IceUtil::Mutex::Lock lock(_mutex);
	list<NotifyIndex>::iterator it = notifies.begin();
	for (; it != notifies.end(); ++it) {
		if (it->isreply > 0) {
			if (_reply_list.empty() || it->id < _reply_list.back().id) {
				_reply_list.push_back(*it);
			}
		} else {
			if (_other_list.empty() || it->id < _other_list.back().id) {
				_other_list.push_back(*it);
			}
		}
		if (it->id < _min_notify_id) {
			_min_notify_id = it->id;
		}
	}
	MCE_INFO("IndexList::appendReply.preload uid:" << _uid << " nids size:" << notifies.size());
	CheckNeedEvict();
}

// 将src合并到list的尾部.
// dest 和 src 的元素是按照id 降序排列的
void IndexList::_backSplice(list<NotifyIndex>& dest, list<NotifyIndex>& src) {
	list<NotifyIndex>::iterator it = src.begin();
	if (!dest.empty()) {
		while (true) {
			if (it == src.end())
				return;
			if (dest.back().id > it->id)
				break;
			++it;
		}
	}

	dest.splice(dest.end(), src, it, src.end());
}

void IndexList::appendReply(list<NotifyIndex>& notifies) {
	_backSplice(_reply_list, notifies);
	MCE_INFO("IndexList::appendReply.Preload uid=" << _uid << " nids size:" << notifies.size());
	CheckNeedEvict();
}

void IndexList::addNotify(const NotifyContentPtr& content) {
	NotifyIndex index;
	index.id = content->id;
	index.source = content->source;
	index.type = content->type;
	index.isreply = content->isreply;
	index.from = content->from;

	MCE_DEBUG("addNotify() isreply = " << content->isreply)

	list<NotifyIndex> & notifies = (index.isreply > 0) ? _reply_list
			: _other_list;

	IceUtil::Mutex::Lock lock(_mutex);
	list<NotifyIndex>::iterator it = notifies.begin();
	for (; it != notifies.end(); ++it) {
		if (it->id < index.id) {
			notifies.insert(it, index);
			break;
		}
	}
	if (it == notifies.end()) {
		notifies.push_back(index);
	}

	if (_min_notify_id > index.id) {
		_min_notify_id = index.id;
	}

	if (++_add_count > kMaxGetLimit) {
		_evictNotify();
		MCE_DEBUG("list size after eviction : reply=" << _reply_list.size() << " other=" << _other_list.size());
	}

	if (_min_unread_nid > content->id) { //如果新增的nid比原来的小，就更新
		_min_unread_nid = content->id;
		TaskManager::instance().execute(new UpdateUnreadNidTask(this));
	}
}

void IndexList::CheckNeedEvict(){
	size_t total_size = _reply_list.size() + _other_list.size();
	if(total_size > 2 * kMaxGetLimit){//TODO
		NotifyIndexManagerI::instance().NeedEvict(_uid,time(NULL));
		MCE_INFO("IndexList::CheckNeedEvict. uid:" << _uid << " size:" << total_size << " need evict");
	}
}

void IndexList::_evictNotify() {
	// 先检查数目是否需要删除
	_add_count = 0;

	size_t total_size = _reply_list.size() + _other_list.size();
	MCE_DEBUG("list size before eviction : reply=" << _reply_list.size() << " other=" << _other_list.size());

	if (total_size <= 2 * kMaxGetLimit)
		return;

	// 删除额外加载的reply元素(小于 _min_notify_id 的元素)
	while (!_reply_list.empty() && _reply_list.back().id < _min_notify_id) {
		_reply_list.pop_back();

		setLoadState(LTYPE_REPLY, LSTATE_READY);

		if (total_size-- <= 2 * kMaxGetLimit)
			return;
	}

	// 删除大于等于 _min_notify_id 的元素
	while (total_size-- > 2 * kMaxGetLimit) {
		if (!_reply_list.empty()) {
			if (_other_list.empty() || _other_list.back().id > _reply_list.back().id) {
				_reply_list.pop_back();
			} else {
				_other_list.pop_back();
			}
		} else {
			if (_other_list.empty()) {
				//assert(0);
				break;
			}
			_other_list.pop_back();
		}
	}

	// 更新 _min_notify_id
	if (!_reply_list.empty()) {
		if (_other_list.empty() || _other_list.back().id > _reply_list.back().id) {
			_min_notify_id = _reply_list.back().id;
		} else {
			_min_notify_id = _other_list.back().id;
		}
	} else {
		if (_other_list.empty()) {
			//assert(0);
			MCE_WARN("index list evict error -> both list empty!,uid:" << _uid);
			_min_notify_id = LLONG_MAX;
		} else {
			_min_notify_id = _other_list.back().id;
		}
	}
	setLoadState(LTYPE_ALL, LSTATE_READY);
}

bool IndexList::_RemoteNotifyFromList(
		RemoveBase & rm_base,
		list<NotifyIndex> & nlist){
	bool suc = false;
	for (list<NotifyIndex>::iterator it = nlist.begin(); it
			!= nlist.end(); ) {
		suc = rm_base.WouldRemove(*it);
		if(suc){
			int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
			rm_base.AddRemovedNotify(it->id,type);
			_other_list.erase(it++);
			if(rm_base.RemoveOneAndStop())
					break;
		} else {
			++it;
		}
	}
	return suc;
}
void IndexList::removeInCache(RemoveBase & rm_base){
	bool suc = false;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		suc = _RemoteNotifyFromList(rm_base,_other_list);
		if(!suc){
			suc = _RemoteNotifyFromList(rm_base,_reply_list);
		}
	}
}


Ice::Long IndexList::getMinReplyId() {
	if (_reply_list.empty()) {
		return LLONG_MAX;
	} else {
		return _reply_list.back().id;
	}
}

// 判断类型t是否是合法的类型
static bool isLegalType(Ice::Int t, const MyUtil::IntSeq& types) {
	if (types.empty()) //类型为空时，表示不限制类型
		return true;

	//TODO : 比较大的时候，不使用顺序查找, 可以借助一个std::set来查找
	//if(types.size() > 20)
	//{
	//}
	for (MyUtil::IntSeq::const_iterator i = types.begin(); i != types.end(); ++i) {
		if (*i == t) {
			return true;
		}
	}
	return false;
}

MyUtil::LongSeq IndexList::getNotifyIdSeq(Ice::Long nid, Ice::Int limit,
		const MyUtil::IntSeq& types) {
	MyUtil::LongSeq ids;

	if (limit <= 0)
		return ids;
	if (limit > kMaxGetLimit)
		limit = kMaxGetLimit;

	MCE_INFO("IndexList::getNotifyIdSeq " << nid << ',' << limit);
	size_t count = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);

		list<NotifyIndex>::iterator reply_it = _reply_list.begin();
		list<NotifyIndex>::iterator other_it = _other_list.begin();
		list<NotifyIndex>::iterator * cur_it;
		while (ids.size() < (size_t) limit) {
			if (reply_it != _reply_list.end()) {
				if (other_it != _other_list.end() && reply_it->id < other_it->id) {
					cur_it = &other_it;
				} else {
					cur_it = &reply_it;
				}
			} else {
				if (other_it != _other_list.end())
					cur_it = &other_it;
				else
					break;
			}
			int type = RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type);
//			MCE_DEBUG("get from mem : " << (*cur_it)->type << ','	<< (*cur_it)->id);
			if (isLegalType(type, types) && (nid <= 0 || (*cur_it)->id
					< nid)) {
				if ((_min_notify_id != LLONG_MAX) && ((*cur_it)->id < _min_notify_id))
					break;
				ids.push_back((*cur_it)->id);
			}
			++(*cur_it);
			++count;
		}

		_min_unread_nid = LLONG_MAX;//表示全部展示过
		TaskManager::instance().execute(new UpdateUnreadNidTask(this));
	}

	// 此处的加载策略, 假设types 为空
	_load_all_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_ALL, count);
	MCE_DEBUG("return notify id seq size: " << ids.size());
	return ids;
}

MyUtil::LongSeq IndexList::getReplyIdSeq(Ice::Long nid, Ice::Int limit) {
	MyUtil::LongSeq ids;
	if (limit <= 0) {
		return ids;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	size_t count = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		for (list<NotifyIndex>::iterator i = _reply_list.begin(); i
				!= _reply_list.end(); ++i) {
			// assert(i->isreply > 0);
			if (nid <= 0 || i->id < nid) {
				ids.push_back(i->id);
				if (ids.size() >= (size_t) limit)
					break;
			}
			++count;
		}
	}

	_load_reply_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_REPLY, count);
	return ids;
}

MyUtil::IntSeq IndexList::getFromIdSeqByType(const MyUtil::IntSeq& types,
		Ice::Int,// 该参数忽略掉
		Ice::Int limit) {
	MyUtil::IntSeq ids;
	if (limit <= 0) {
		return ids;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	set<Ice::Int> id_set;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		if (_loadAllState == LSTATE_NOMORE) {
			set<Ice::Int> type_set;
			for (size_t i = 0; i < types.size(); ++i) {
				type_set.insert(types[i]);
			}

			list<NotifyIndex>::iterator it = _reply_list.begin();
			for (; it != _reply_list.end(); ++it) {
				int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
				if (type_set.find(type) != type_set.end()) {
					id_set.insert(it->from);
				}
			}

			for (it = _other_list.begin(); it != _other_list.end(); ++it) {
				int type = RMessageUtil::instance().Util_GetRMessageType(it->type);
				if (type_set.find(type) != type_set.end()) {
					id_set.insert(it->from);
				}
			}

			for (set<Ice::Int>::iterator i = id_set.begin(); i != id_set.end(); ++i) {
				ids.push_back(*i);
			}
			MCE_INFO("getFromIdSeq from memory : uid=" << _uid);

			return ids;
		}
	}

	MCE_INFO("getFromIdSeq from database : uid=" << _uid);
	Statement sql;
	sql << "SELECT DISTINCT from_id FROM " << NotifyDbHelper::getIndexTableName(
			_uid) << " WHERE";
	sql << " to_id = " << _uid;

	for (size_t i = 0; i < types.size(); ++i) {
		if (i == 0) {
			sql << " AND type IN (";
		}
		sql << types.at(i);
		if (i != (types.size() - 1)) {
			sql << ",";
		} else {
			sql << ")";
		}
	}
	sql << " and ( id < 13903218553  or id > 13915001511)" << kLimit;

	sql << " ORDER BY id LIMIT " << limit;

	try {
		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbRServer,
				NotifyDbHelper::getIndexTableName(_uid)).store(sql);
		if (!res || res.num_rows() <= 0) {
			return ids;
		}

		for (size_t i = 0; i < res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			ids.push_back(static_cast<Ice::Int> (row["from_id"]));
		}
	} catch (std::exception& e) {
		MCE_WARN("Load notify err uid:" << _uid  << e.what()  );
	} catch (...) {
		MCE_WARN("Unkown exception uid:"<< _uid << " at " << __FILE__ << ":" << __LINE__);
	}
	return ids;
}

MyUtil::LongSeq IndexList::getIdSeqByType(const MyUtil::IntSeq& types,
		Ice::Long nid, Ice::Int limit) {
	MyUtil::LongSeq ids;

	if (limit <= 0) {
		return ids;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	Statement sql;
	sql << "SELECT id FROM " << NotifyDbHelper::getIndexTableName(_uid)
			<< " WHERE";
	if (nid > 0) {
		sql << " id < " << nid << " AND";
	}
	sql << " to_id = " << _uid;

	for (size_t i = 0; i < types.size(); ++i) {
		if (i == 0) {
			sql << " AND type IN (";
		}
		sql << types.at(i);
		if (i != (types.size() - 1)) {
			sql << ",";
		} else {
			sql << ")";
		}
	}
	sql << " and ( id < 13903218553  or id > 13915001511)" << kLimit;
	sql << " ORDER BY id DESC LIMIT " << limit;

	try {
		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbRServer,
				NotifyDbHelper::getIndexTableName(_uid)).store(sql);
		if (!res || res.num_rows() <= 0) {
			return ids;
		}

		for (size_t i = 0; i < res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			ids.push_back(static_cast<Ice::Long> (row["id"]));
		}
	} catch (std::exception& e) {
		MCE_WARN("Load notify err uid:" << _uid  << e.what()  << __FILE__ << ":" << __LINE__ );
	} catch (...) {
		MCE_WARN("Unkown exception uid:"<< _uid << " at " << __FILE__ << ":" << __LINE__);
	}
	return ids;
}

MyUtil::LongSeq IndexList::getNotifyIdSeq2(Ice::Int begin, Ice::Int limit,
		const MyUtil::IntSeq& types) {
	MyUtil::LongSeq ids;

	if (limit <= 0) {
		return ids;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	if (begin < 0) {
		begin = 0;
	}

	size_t count = 0;
	Ice::Int pos = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		list<NotifyIndex>::iterator reply_it = _reply_list.begin();
		list<NotifyIndex>::iterator other_it = _other_list.begin();
		list<NotifyIndex>::iterator * cur_it;
		while (ids.size() < (size_t) limit) {
			if (reply_it != _reply_list.end()) {
				if (other_it != _other_list.end() && reply_it->id < other_it->id) {
					cur_it = &other_it;
				} else {
					cur_it = &reply_it;
				}
			} else {
				if (other_it != _other_list.end()) {
					cur_it = &other_it;
				} else {
					break;
				}
			}

//			MCE_DEBUG("get from mem : " << (*cur_it)->type << ','
//					<< (*cur_it)->id);
			int type = RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type);
			if (isLegalType(type, types)) {
				if (++pos > begin) {
					ids.push_back((*cur_it)->id);
				}
			}
			++(*cur_it);
			++count;
		}
	}

	// 此处的加载策略, 假设types 为空
	_load_all_threshold = max(limit,kMinGetLimit);
	{
		IceUtil::Mutex::Lock lock(_mutex);
		MCE_INFO("IndexList::getNotifyIdSeq2. nids:" << ids.size()
			<< " count:" << count << " _load_all_threshold:" << _load_all_threshold
			<<	" _reply_list:" << _reply_list.size() << " _other_list:" << _other_list.size() );
	}
	_load(LTYPE_ALL, count);
	return ids;
}

MyUtil::LongSeq IndexList::getReplyIdSeq2(Ice::Int begin, Ice::Int limit) {
	MyUtil::LongSeq ids;

	if (limit <= 0) {
		return ids;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	if (begin < 0) {
		begin = 0;
	}

	size_t count = 0;
	Ice::Int pos = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		for (list<NotifyIndex>::iterator i = _reply_list.begin(); i
				!= _reply_list.end(); ++i) {
			// assert(i->isreply > 0);
			if (++pos > begin) {
				ids.push_back(i->id);
				if (ids.size() >= (size_t) limit) {
					break;
				}
			}
			++count;
		}
	}

	_load_reply_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_REPLY, count);
	return ids;
}

MyUtil::LongSeq IndexList::getIdSeqByType2Test(const MyUtil::IntSeq& types,
		Ice::Int begin, Ice::Int limit) {
	vector<long> res = getNotifyIdSeq2(begin,limit,types);
	MCE_INFO("IndexList::getIdSeqByType2Test.uid:" << _uid << " begin:" << begin << " limit:" << limit << " res:" << res.size());
	return res;
}


MyUtil::LongSeq IndexList::getIdSeqByType2(const MyUtil::IntSeq& types,
		Ice::Int begin, Ice::Int limit) {
	MyUtil::LongSeq ids;

	if (limit <= 0) {
		return ids;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	if (begin < 0) {
		begin = 0;
	}

	Statement sql;
	sql << "SELECT id FROM " << NotifyDbHelper::getIndexTableName(_uid)
			<< " WHERE";
	sql << " to_id = " << _uid;

	for (size_t i = 0; i < types.size(); ++i) {
		if (i == 0) {
			sql << " AND type IN (";
		}
		sql << types.at(i);
		if (i != (types.size() - 1)) {
			sql << ",";
		} else {
			sql << ")";
		}
	}
	sql << " and ( id < 13903218553  or id > 13915001511)" << kLimit;
	sql << " ORDER BY id DESC LIMIT " << begin + limit;

	try {
		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbRServer,
				NotifyDbHelper::getIndexTableName(_uid)).store(sql);
		if (!res || res.num_rows() <= 0) {
			return ids;
		}

		MCE_DEBUG("num_rows() :  " << res.num_rows() << " begin=" << begin
				<< " limit=" << limit);

		for (size_t i = begin; i < res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			ids.push_back(static_cast<Ice::Long> (row["id"]));
		}

		MCE_DEBUG("num_rows() :  " << res.num_rows() << " begin=" << begin
				<< " limit=" << limit << " ids.size()=" << ids.size());

	} catch (std::exception& e) {
		MCE_WARN("IndexList::getIdSeqByType2 Load notify err uid:" << _uid << " " << e.what());
	} catch (...) {
		MCE_WARN("IndexList::getIdSeqByType2 Unkown exception uid:" << _uid << " " << __FILE__ << ":" << __LINE__);
	}
	MCE_INFO("IndexList::getIdSeqByType2 .uid:" << _uid << " begin:" << begin << " limit:" << limit << " res:" << ids.size());
	return ids;
}
//////////////////////////////////////////////////////
MergeMap IndexList::getNotifyIdSeqMerge(Ice::Long nid, Ice::Int limit,
		const MyUtil::IntSeq& types, MyUtil::LongSeq & ids) {

	MergeMap res;

	if (limit <= 0)
		return res;
	if (limit > kMaxGetLimit)
		limit = kMaxGetLimit;

	MCE_INFO("IndexList::getNotifyIdSeq " << nid << ',' << limit);
	size_t count = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);

		list<NotifyIndex>::iterator reply_it = _reply_list.begin();
		list<NotifyIndex>::iterator other_it = _other_list.begin();
		list<NotifyIndex>::iterator * cur_it;

		while (1) {
			if (reply_it != _reply_list.end()) {
				if (other_it != _other_list.end() && reply_it->id < other_it->id) {
					cur_it = &other_it;
				} else {
					cur_it = &reply_it;
				}
			} else {
				if (other_it != _other_list.end())
					cur_it = &other_it;
				else
					break;
			}
			MCE_DEBUG("get from mem : " << (*cur_it)->id << (*cur_it)->type << ',' << (*cur_it)->source);
			if (isLegalType(RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type), types) && (nid <= 0 || (*cur_it)->id
					< nid)) {
				if ((_min_notify_id != LLONG_MAX) && ((*cur_it)->id < _min_notify_id)) {
					break;
				}
				Long nid = (*cur_it)->id;
				Int type = RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type);
				Long source = (*cur_it)->source;
				Int uid = (*cur_it)->from;
				MCE_DEBUG(type << "," << source << "," << uid << "," << nid);
				if (MergeConfig::instance().MustMergeType(type)) {
					MergeMap::iterator mit = res.find(make_pair(type, source));
					if (mit != res.end()) {
						mit->second.push_back(make_pair(uid, nid));
					} else {
						if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
							break;
						}
						pair<int, Long> k(type, source);
						vector<pair<Int, Long> > v;
						v.push_back(make_pair(uid, nid));
						pair<MergeMap::iterator, bool> p = res.insert(make_pair(k, v));
					}
				} else {
					if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
						break;
					}
					ids.push_back(nid);
				}
			}

			++(*cur_it);
			++count;
		}

		_min_unread_nid = LLONG_MAX; //表示全部显示过
		TaskManager::instance().execute(new UpdateUnreadNidTask(this));
	}

	// 此处的加载策略, 假设types 为空
	_load_all_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_ALL, count);
	MCE_DEBUG("return notify res seq size: " << res.size() << ",not merge ids size :" << ids.size());
	return res;
}

MergeMap IndexList::getReplyIdSeqMerge(Ice::Long nid, Ice::Int limit,
		MyUtil::LongSeq & ids) {
	MergeMap res;
	if (limit <= 0) {
		return res;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	size_t count = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		for (list<NotifyIndex>::iterator i = _reply_list.begin(); i
				!= _reply_list.end(); ++i) {
			// assert(i->isreply > 0);
			if (nid <= 0 || i->id < nid) {
				Long nid = i->id;
				Int uid = i->from;
				Long source = i->source;
				Int type = RMessageUtil::instance().Util_GetRMessageType(i->type);

				if (MergeConfig::instance().MustMergeType(type)) {
					MergeMap::iterator mit = res.find(make_pair(type, source));
					if (mit != res.end()) {
						mit->second.push_back(make_pair(uid, nid));
					} else {
						if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
							break;
						}
						res[make_pair(type, source)].push_back(make_pair(uid, nid));
					}
				} else {
					if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
						break;
					}
					ids.push_back(nid);
				}
			}
			++count;
		}
	}

	_load_reply_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_REPLY, count);
	MCE_DEBUG("return reply res seq size: " << res.size() << ",not merge ids size :" << ids.size());
	return res;
}
//////////////////////////////////////////////////////
MergeMap IndexList::getNotifyIdSeqX(Ice::Long nid, Ice::Int view,
		Ice::Int limit, const MyUtil::IntSeq& types, MyUtil::LongSeq & ids) {

	MergeMap res;

	if (limit <= 0)
		return res;
	if (limit > kMaxGetLimit)
		limit = kMaxGetLimit;

	MCE_INFO("IndexList::getNotifyIdSeqX. nid:" << nid << ' limit:' << limit);
	size_t count = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);

		list<NotifyIndex>::iterator reply_it = _reply_list.begin();
		list<NotifyIndex>::iterator other_it = _other_list.begin();
		list<NotifyIndex>::iterator * cur_it;

		while (1) {
			if (reply_it != _reply_list.end()) {
				if (other_it != _other_list.end() && reply_it->id < other_it->id) {
					cur_it = &other_it;
				} else {
					cur_it = &reply_it;
				}
			} else {
				if (other_it != _other_list.end())
					cur_it = &other_it;
				else
					break;
			}
			/*
			if(!SchemaManager::instance().getIndexViewCheck(view, *(*cur_it))){
				MCE_DEBUG("IndexList::getNotifyIdSeqX-->view:" << view << " type:" << (*cur_it)->type << " id:" << (*cur_it)->id);
				continue;
			}
			*/
			MCE_DEBUG("IndexList::getNotifyIdSeqX get from mem : " << (*cur_it)->id << RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type) << ',' << (*cur_it)->source);
			if (isLegalType(RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type), types) && (nid <= 0 || (*cur_it)->id < nid)) {
				if ((_min_notify_id != LLONG_MAX) && ((*cur_it)->id < _min_notify_id)) {
					break;
				}
				Long nid = (*cur_it)->id;
				Int type = RMessageUtil::instance().Util_GetRMessageType((*cur_it)->type);
				Long source = (*cur_it)->source;
				Int uid = (*cur_it)->from;
				MCE_DEBUG(type << "," << source << "," << uid << "," << nid);

				if (SchemaManager::instance().MustMerge(type, view)) {
					MergeMap::iterator mit = res.find(make_pair(type, source));
					if (mit != res.end()) {
						mit->second.push_back(make_pair(uid, nid));
					} else {
						if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
							break;
						}
						pair<int, Long> k(type, source);
						vector<pair<Int, Long> > v;
						v.push_back(make_pair(uid, nid));
						pair<MergeMap::iterator, bool> p = res.insert(make_pair(k, v));
					}
				} else {
					if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
						break;
					}
					ids.push_back(nid);
				}
			}

			++(*cur_it);
			++count;
		}

		_min_unread_nid = LLONG_MAX; //表示全部显示过
		TaskManager::instance().execute(new UpdateUnreadNidTask(this));
	}

	// 此处的加载策略, 假设types 为空
	_load_all_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_ALL, count);
	MCE_DEBUG("IndexList::getNotifyIdSeqX.  return notify res seq size: " << res.size() << ",not merge ids size :" << ids.size());
	return res;
}

MergeMap IndexList::getReplyIdSeqX(Ice::Long nid, Ice::Int view,
		Ice::Int limit, MyUtil::LongSeq & ids) {
	MergeMap res;
	if (limit <= 0) {
		return res;
	}
	if (limit > kMaxGetLimit) {
		limit = kMaxGetLimit;
	}

	size_t count = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		for (list<NotifyIndex>::iterator i = _reply_list.begin(); i
				!= _reply_list.end(); ++i) {
			// assert(i->isreply > 0);
			if (nid <= 0 || i->id < nid) {
				Long nid = i->id;
				Int uid = i->from;
				Long source = i->source;
				Int type = RMessageUtil::instance().Util_GetRMessageType(i->type);

				if (SchemaManager::instance().MustMerge(type, view)) {
					MergeMap::iterator mit = res.find(make_pair(type, source));
					if (mit != res.end()) {
						mit->second.push_back(make_pair(uid, nid));
					} else {
						if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
							break;
						}
						res[make_pair(type, source)].push_back(make_pair(uid, nid));
					}
				} else {
					if (res.size() + ids.size() == (size_t) limit) {//马上就要超过limit
						break;
					}
					ids.push_back(nid);
				}
			}
			++count;
		}
	}

	_load_reply_threshold = limit > kMinGetLimit ? limit : kMinGetLimit;
	_load(LTYPE_REPLY, count);
	MCE_DEBUG("IndexList::getReplyIdSeqX.return reply res seq size: " << res.size()
			<< ",not merge ids size :" << ids.size());
	return res;
}
//////////////////////////////////////////////////////
void UpdateUnreadNidTask::handle() {
	Ice::Int uid = _notifyList->uid();
	try {
		Statement sql;
		sql << "INSERT INTO " << NotifyDbHelper::getInfoTableName(uid)
				<< "(id, min_unread_nid)  VALUES (" << uid << ", "
				<< _notifyList->min_unread_nid()
				<< ") ON DUPLICATE KEY UPDATE min_unread_nid="
				<< _notifyList->min_unread_nid();

		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbWServer,
				NotifyDbHelper::getInfoTableName(uid)).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("UpdateUnreadNidTask uid=" << uid << " " << e.what());
	} catch (...) {
		MCE_WARN("UpdateUnreadNidTask unkown exception.uid:" << uid);
	}
}

void LoadNotifyListTask::handle() {
	if (!_notifyList) {
		MCE_INFO("LoadNotifyListTask::handle: _notifyList is empty");
		return;
	}
	MCE_INFO("LoadNotifyListTask::handle. load notify list : uid=" << _notifyList->uid());
	list<NotifyIndex> replySeq;
	bool no_more = false;
	try {
		// 只加载更老的 notify index 数据
		Statement sql;
		sql << "select id, type, source, from_id, is_reply from "
				<< NotifyDbHelper::getIndexTableName(_notifyList->uid())
				<< " where to_id = " << _notifyList->uid(); //取出id
		//    sql << " and (TO_DAYS(now()) - TO_DAYS(time) <  90) "  ;//只加载90天内的
		sql << " and type not in (247, 256, 258, 259, 260, 261, 262, 264, 265, 266, 269, 279) ";
		if (_loadType == LTYPE_ALL) {
			IceUtil::Mutex::Lock lock(_notifyList->mutex());
			sql << " and id < " << _notifyList->min_notify_id(); //内存中最小的nid
			sql << " and ( id < 13903218553 or id > 13915001511 )" << kLimit;
			sql << " order by id desc limit " << 2 * _notifyList->loadAllThreshold();
		} else {
			sql << " and is_reply > 0";
			IceUtil::Mutex::Lock lock(_notifyList->mutex());
			sql << " and id < " << _notifyList->getMinReplyId();
			sql << " and ( id < 13903218553 or id > 13915001511 )";
			sql << " order by id desc limit " << 2
					* _notifyList->loadReplyThreshold();
		}
		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbRServer,
				NotifyDbHelper::getIndexTableName(_notifyList->uid())).store(sql);
		// TODO : 如果出错，是否关闭下次继续查询?
		if (!res) {
			MCE_WARN("db select result null.uid:" << _notifyList->uid() );
			goto LOAD_END;
		}

		// 此处不需要加锁
		if (_loadType == LTYPE_ALL) {
			if (res.num_rows() < 2 * _notifyList->loadAllThreshold()) { //如果加载的数据少于limit，说明数据库已经没有更老的了
				no_more = true;
			}
		} else {
			if (res.num_rows() < 2 * _notifyList->loadReplyThreshold()) { //同上
				no_more = true;
			}
		}

		NotifyIndex index;
		for (size_t i = 0; i < res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			index.id = (Ice::Long) row["id"];
			index.type = (Ice::Int) row["type"];
			index.source = (Ice::Long) row["source"];
			index.from = (Ice::Int) row["from_id"];
			index.isreply = (Ice::Int) row["is_reply"];
			replySeq.push_back(index);
		}
	} catch (std::exception& e) {
		MCE_WARN("LoadNotifyListTask::handle load index err, "
				<< " uid:" << _notifyList->uid()
				<< e.what());
		goto LOAD_END;
	} catch (...) {
		MCE_WARN("LoadNotifyListTask::handle unkown load index err, "
				<< " uid:" << _notifyList->uid());
		goto LOAD_END;
	}

	if (!replySeq.empty()) {
		if (_loadType == LTYPE_ALL)
			_notifyList->append(replySeq);
		else
			_notifyList->appendReply(replySeq);
	}

	try {
		Statement sql;
		sql << "select min_unread_nid from " << NotifyDbHelper::getInfoTableName(
				_notifyList->uid()) << " where id = " << _notifyList->uid();
		//		sql << " and ( id < 13903218553 or id > 13915001511)";

		mysqlpp::StoreQueryResult res = QueryRunner(
				NotifyDbHelper::getDbInstance(), CDbRServer,
				NotifyDbHelper::getInfoTableName(_notifyList->uid())).store(sql);

		if (!res || res.empty()) {
			goto LOAD_END;
		}

		mysqlpp::Row row = res.at(0);
		_notifyList->set_min_unread_id((Ice::Long) row["min_unread_nid"]);
	} catch (std::exception& e) {
		MCE_WARN("LoadNotifyListTask::handle load min_unread_nid err, "
				<< " uid:" << _notifyList->uid()
				<< e.what());
		goto LOAD_END;
	} catch (...) {
		MCE_WARN("LoadNotifyListTask::handle unkown load min_unread_nid err, "
				<< " uid:" << _notifyList->uid());
		goto LOAD_END;
	}
	LOAD_END: _notifyList->setLoadState(_loadType, no_more ? LSTATE_NOMORE
			: LSTATE_READY);
	MCE_INFO("LoadNotifyListTask::handle.uid:"<< _notifyList->uid()
			<< " db res:" << replySeq.size()
			<< " _loadType:" << PrintEnum(_loadType)
			<< " _loadState:" << PrintEnum(no_more ? LSTATE_NOMORE	: LSTATE_READY)
	);
}

void NotifyIndexManagerI::addNotify(const NotifyContentPtr& content,
		const MyUtil::IntSeq& uids, const Ice::Current&) {
	
	NotifyListPtr notifyList;
	for (size_t i = 0; i < uids.size(); ++i) {
		if (uids[i] <= 0) {
			continue;
		}
//		locateNotifyList(uids.at(i));
		notifyList = findNotifyList(uids.at(i));
		if (notifyList) {
			MCE_INFO("addNotify(user in memory): nid=" << content->id
					<< " source=" << RMessageUtil::instance().Util_GetRMessageType(content->type) << "- bigtype-" << RMessageUtil::instance().Util_GetRMessageBigtype(content->type) << '-' << content->isreply << '-' << content->source
					<< " uid=" << uids.at(i));
			notifyList->addNotify(content);
		} else {
			MCE_INFO("addNotify(user not loaded): nid=" << content->id
					<< " source=" << RMessageUtil::instance().Util_GetRMessageType(content->type) << "-bigtype-" << RMessageUtil::instance().Util_GetRMessageBigtype(content->type) << '-' << content->isreply << '-' << content->source
					<< " uid=" << uids.at(i));
		}
	}
}

string NotifyIndexManagerI::dumpList(Ice::Int uid, const Ice::Current&) {
	NotifyListPtr notifyList = locateNotifyList(uid);
	if (notifyList) {
		return notifyList->dump();
	} else {
		MCE_INFO("DUMP - uid not in memeory : " << uid);
		return "not im memory";
	}
}

void ReplaceChar(string & s) {
	size_t found = s.find('\n');
	if (found != string::npos) {
		s[found] = ' ';
	}
}

void NotifyIndexManagerI::removeImpl(RemoveBase & rq) {
	NotifyListPtr notifyList = locateNotifyList(rq.GetUid());
	if (notifyList) {
		notifyList->removeInCache(rq);
		rq.RemoveInDb();
		rq.CallIqMsgHandlerAndNotifyStrom();
	}
	MCE_DEBUG("NotifyIndexManagerI::removeImpl." << rq.GetString());
}

void NotifyIndexManagerI::removeById(Ice::Int uid, Ice::Long replyId,
		const Ice::Current& current) {
	//string ip = current.con->toString();
	//ReplaceChar(ip);
	//MCE_INFO("NotifyIndexManagerI::removeById : uid="<< uid << " nid=" << replyId << " " << ip);
	MCE_INFO("NotifyIndexManagerI::removeById : uid="<< uid << " nid=" << replyId);
	RemoveByNid rq(uid,replyId);
	removeImpl(rq);
}

void NotifyIndexManagerI::removeByIds(Ice::Int uid,
		const MyUtil::LongSeq & nids, const Ice::Current&) {
	MCE_INFO("NotifyIndexManagerI::removeByIds : uid="<< uid << " nids.size()=" << nids.size() << " nids:" << PrintHelper::Print(nids));
	RemoveByNids rq(uid,nids);
	removeImpl(rq);
}

void NotifyIndexManagerI::removeBySource(Ice::Int uid, Ice::Int type,
		Ice::Long source, const Ice::Current&) {
	MCE_INFO("NotifyIndexManagerI::removeBySource : uid=" << uid << " source=" << type << '-' << source);
	RemoveBySource rq(uid,type,source);
	removeImpl(rq);
}

void NotifyIndexManagerI::removeBySourceAndSender(Ice::Int uid, Ice::Int type,
		Ice::Long source, Ice::Int sender, const Ice::Current&) {
	MCE_INFO("NotifyIndexManagerI::removeBySourceAndSender : uid=" << uid << " source=" << type << '-' << source << " sender=" << sender);
	RemoveBySourceSender rq(uid,type,source,sender);
	removeImpl(rq);
}

NotifyContentSeq NotifyIndexManagerI::getNotifySeq(Ice::Int uid, Ice::Long nid,
		Ice::Int limit, const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);

	MyUtil::LongSeq ids;
	if (notifyList) {
		ids = notifyList->getNotifyIdSeq(nid, limit, MyUtil::IntSeq());
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
	}


		MCE_INFO("getNotifySeq uid=" << uid << " nid=" << nid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

NotifyContentSeq NotifyIndexManagerI::getReplySeq(Ice::Int uid, Ice::Long nid,
		Ice::Int limit, const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);

	MyUtil::LongSeq ids;
	if (notifyList) {
		ids = notifyList->getReplyIdSeq(nid, limit);
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
	} else {
		MCE_WARN("getReplySeq uid=" << uid << " locate list NULL. uid:" << uid);
	}

		MCE_INFO("getReplySeq uid=" << uid << " nid=" << nid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

MyUtil::IntSeq NotifyIndexManagerI::getFromIdSeq(Ice::Int uid,
		const MyUtil::IntSeq & types, Ice::Int begin, Ice::Int limit,
		const Ice::Current&) { // 还没有被调用
	MyUtil::IntSeq from_ids;
	NotifyListPtr notifyList = locateNotifyList(uid);

	if (notifyList) {
		from_ids = notifyList->getFromIdSeqByType(types, begin, limit);
	}
	MCE_INFO("getFromIdSeq uid=" << uid
			<< " begin:" << begin << " limit:" << limit
			<< " types:" << PrintHelper::Print(types)
			<< " from_ids:" << PrintHelper::Print(from_ids));

	return from_ids;
}

NotifyContentSeq NotifyIndexManagerI::getByType(Ice::Int uid,
		const MyUtil::IntSeq& types, Ice::Long nid, Ice::Int limit,
		const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);

	MyUtil::LongSeq ids;
	if (notifyList) {
		ids = notifyList->getIdSeqByType(types, nid, limit);
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
	}
	MCE_INFO("NotifyIndexManagerI::getByType. uid=" << uid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

NotifyContentSeq NotifyIndexManagerI::getNotifySeq2(Ice::Int uid,
		Ice::Int begin, Ice::Int limit, const Ice::Current&) {
	//  NotifyContentSeq contents;
	//  TimeStat ts,tstotal;
	//  NotifyListPtr notifyList = locateNotifyList(uid);
	//  float locate_t = ts.getTime();
	//  ts.reset();
	//
	//  float getid_t = 0.0f;
	//  float getcontent_t = 0.0f;
	//  MyUtil::LongSeq ids;
	//  set<Ice::Long> fail_ids;
	//
	//  if (notifyList) {
	//    ids = notifyList->getNotifyIdSeq2(begin, limit, MyUtil::IntSeq());
	//    getid_t = ts.getTime();
	//    ts.reset();
	//    _getContentsByIds(uid,ids, contents, fail_ids);
	//    getcontent_t = ts.getTime();
	//    ts.reset();
	//  }
	//
	//  ostringstream oss;
	//  oss << " locate:" << locate_t << " getid:" << getid_t << " getcontent_t:" << getcontent_t << " total:" << tstotal.getTime();
	//  if (contents.size() < ids.size()) {
	//    MCE_WARN("getNotifySeq2 uid=" << uid << " begin:" << begin << " limit:" << limit <<  " ids.size()=" << ids.size()
	//        << " contents.size()=" << contents.size() << " #COST# " << oss.str());
	//  } else {
	//    MCE_INFO("getNotifySeq2 uid=" << uid << " begin:" << begin << " limit:" << limit  << " ids.size()=" << ids.size()
	//        << " contents.size()=" << contents.size()  << " #COST# " << oss.str());
	//  }
	//
	//  return contents;


	TimeStat t, total;
	float locate = 0.0f, list = 0.0f, content = 0.0f;
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);
	locate = t.getTime();
	MyUtil::LongSeq ids;

	if (notifyList) {
		ids = notifyList->getNotifyIdSeq2(begin, limit, MyUtil::IntSeq());
		list = t.getTime();
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
		content = t.getTime();
	}
	MCE_INFO("NotifyIndexManagerI::getNotifySeq2.uid=" << uid << " begin:" << begin
			<< " limit:" << limit << " ids.size()=" << ids.size()
			<< " contents.size()=" << contents.size() << " #cost# locate:" << locate
			<< " list:" << list << " content:" << content << " total:"
			<< total.getTime());
	return contents;
}

NotifyContentSeq NotifyIndexManagerI::getReplySeq2(Ice::Int uid,
		Ice::Int begin, Ice::Int limit, const Ice::Current&) {
	TimeStat t, total;
	float locate = 0.0f, list = 0.0f, content = 0.0f;
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);
	locate = t.getTime();
	MyUtil::LongSeq ids;

	if (notifyList) {
		ids = notifyList->getReplyIdSeq2(begin, limit);
		list = t.getTime();
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
		content = t.getTime();
	}
	MCE_INFO( "NotifyIndexManagerI::getReplySeq2.uid=" << uid << " begin:" << begin
			<< " limit:" << limit << " ids.size()=" << ids.size()
			<< " contents.size()=" << contents.size() << " #cost# locate:" << locate
			<< " list:" << list << " content:" << content << " total:"
			<< total.getTime());
	return contents;
}

NotifyContentSeq NotifyIndexManagerI::getByType2(Ice::Int uid,
		const MyUtil::IntSeq& types, Ice::Int begin, Ice::Int limit,
		const Ice::Current&) {
	TimeStat t, total;
	float locate = 0.0f, list = 0.0f, content = 0.0f;
	NotifyContentSeq contents;

	NotifyListPtr notifyList = locateNotifyList(uid);
	locate = t.getTime();
	MyUtil::LongSeq ids;
	if (notifyList) {
		ids = notifyList->getIdSeqByType2Test(types, begin, limit);
//		if(uid / 10 % 2 == 0){
//		bool newway = false;
//		if(testers_.count(uid)){
//			newway = true;
//			MCE_INFO("NotifyIndexManagerI::getByType2: uid:" << uid << " is tester");
//		}
//		if( uid % 10 < level_){ newway = true;}
//		if(newway){
//			ids = notifyList->getIdSeqByType2Test(types, begin, limit);
//		}else{
//			ids = notifyList->getIdSeqByType2(types, begin, limit);
//		}
		list = t.getTime();
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
		content = t.getTime();
	}
	MCE_INFO( "NotifyIndexManagerI::getByType2.uid=" << uid << " types size:"
			<< types.size() << " begin:" << begin << " limit:" << limit
			<< " ids.size()=" << ids.size() << " contents.size()=" << contents.size()
			<< " #cost# locate:" << locate << " list:" << list << " content:"
			<< content << " total:" << total.getTime() << " types:"
			<< PrintHelper::Print(types));
	return contents;
}

////////////////////////////////////////

NotifyContentSeq NotifyIndexManagerI::getNotifySeqMerge(Ice::Int uid,
		Ice::Long nid, Ice::Int limit, const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);

	MyUtil::LongSeq ids;
	MergeMap merge_res;
	if (notifyList) {
		merge_res = notifyList->getNotifyIdSeqMerge(nid, limit, MyUtil::IntSeq(),
				ids);
		for (MergeMap::iterator mit = merge_res.begin(); mit != merge_res.end(); ++mit) {
			for (vector<pair<Int, Long> >::iterator i = mit->second.begin(); i
					!= mit->second.end(); ++i) {
				ids.push_back(i->second);
			}
		}
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
	}

		MCE_INFO("getNotifySeqMerge uid=" << uid << " nid=" << nid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

NotifyContentSeq NotifyIndexManagerI::getReplySeqMerge(Ice::Int uid,
		Ice::Long nid, Ice::Int limit, const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);
	MyUtil::LongSeq ids;
	MergeMap merge_res;
	if (notifyList) {
		merge_res = notifyList->getReplyIdSeqMerge(nid, limit, ids);
		for (MergeMap::iterator mit = merge_res.begin(); mit != merge_res.end(); ++mit) {
			for (vector<pair<Int, Long> >::iterator i = mit->second.begin(); i
					!= mit->second.end(); ++i) {
				ids.push_back(i->second);
			}
		}
		set<Ice::Long> tmp;
		_getContentsByIds(uid,ids, contents, tmp);
	}

		MCE_INFO("getReplySeqMerge uid=" << uid << " nid=" << nid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

////////////////////////////////////////

NotifyContentSeq NotifyIndexManagerI::getNotifySeqX(Ice::Int uid,
		Ice::Int view, Ice::Long nid, Ice::Int limit, const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);

	MyUtil::LongSeq ids;
	MergeMap merge_res;
	set<Ice::Long> fail_ids;
	if (notifyList) {
		merge_res = notifyList->getNotifyIdSeqX(nid, view, limit, MyUtil::IntSeq(),
				ids);
		for (MergeMap::iterator mit = merge_res.begin(); mit != merge_res.end(); ++mit) {
			for (vector<pair<Int, Long> >::iterator i = mit->second.begin(); i
					!= mit->second.end(); ++i) {
				ids.push_back(i->second);
			}
		}
		_getContentsByIds(uid,ids, contents, fail_ids);
	}else{
		MCE_INFO("NotifyIndexManagerI::getNotifySeqX. NotifyList empty");
	}

		MCE_INFO("getNotifySeqX uid=" << uid << " nid=" << nid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

NotifyContentSeq NotifyIndexManagerI::getReplySeqX(Ice::Int uid, Ice::Int view,
		Ice::Long nid, Ice::Int limit, const Ice::Current&) {
	NotifyContentSeq contents;
	NotifyListPtr notifyList = locateNotifyList(uid);
	MyUtil::LongSeq ids;
	MergeMap merge_res;
	set<Ice::Long> fail_ids;
	if (notifyList) {
		merge_res = notifyList->getReplyIdSeqX(nid, view, limit, ids);
		for (MergeMap::iterator mit = merge_res.begin(); mit != merge_res.end(); ++mit) {
			for (vector<pair<Int, Long> >::iterator i = mit->second.begin(); i
					!= mit->second.end(); ++i) {
				ids.push_back(i->second);
			}
		}
		_getContentsByIds(uid,ids, contents, fail_ids);
	}

		MCE_INFO("getReplySeqX uid=" << uid << " nid=" << nid << " ids.size()=" << ids.size()
				<< " contents.size()=" << contents.size());

	return contents;
}

////////////////////////////////////////

Ice::Int NotifyIndexManagerI::getCount(Ice::Int uid,
		const MyUtil::IntSeq& types, const Ice::Current&) {
	NotifyListPtr notifyList = locateNotifyList(uid);

	int count = 0;
	if (notifyList) {
		count = notifyList->getCountByType(types);
	}
	MCE_INFO("NotifyIndexManagerI::getCount. uid:" << uid << " count:" << count << " types size:" << types.size() << " types:" << PrintHelper::Print(types,7));
	return count;
}

Ice::Int NotifyIndexManagerI::getUnreadCount(Ice::Int uid, const Ice::Current&) {
	NotifyListPtr notifyList = locateNotifyList(uid);
	int view = -1;
	if (notifyList) {
		vector<int> types;
		return notifyList->getUnreadCount(types, false,view);
	}
	return 0;
}

Ice::Int NotifyIndexManagerI::getUnreadCountByView(Ice::Int uid, Ice::Int view,
		const Ice::Current&) {
	NotifyListPtr notifyList = locateNotifyList(uid);
	if (notifyList) {
		vector<int> types;
		return notifyList->getUnreadCount(types, false,view);
	}
	return 0;
}

Ice::Int NotifyIndexManagerI::getUnreadCountByTypes(Ice::Int uid,
		 const vector<int> & types, const Ice::Current&) {
	NotifyListPtr notifyList = locateNotifyList(uid);
	if (notifyList) {
		return notifyList->getUnreadCount(types,true, -1);
	}
	return 0;
}

Ice::Int NotifyIndexManagerI::getUnreadCountByTypesAndMaxReadId(
		Ice::Int uid,const vector<int> & types, Ice::Int MaxReadId,const Ice::Current& ){
	return 0;
}

bool NotifyIndexManagerI::_getContentsByIds(int uid,const MyUtil::LongSeq & ids,
		NotifyContentSeq & contents, set<Ice::Long> & fail_ids) {
	if (ids.empty()) // 如果notify id 列表为空，直接返回.
		return true;
	fail_ids.insert(ids.begin(), ids.end());
	NotifyContentDict contentDict;
	try {
		contentDict = NotifyContentAdapter::instance().getNotifyDict(ids);
	} catch (Ice::Exception& e) {
		MCE_WARN("NotifyContentAdapter::getNotifyDict() error:" << e);
		return false;
	}
	vector<long> not_in_db_nids;
	NotifyContentPtr reply;
	NotifyContentDict::iterator iter;
	BOOST_FOREACH(long id, ids){
		iter = contentDict.find(id);
		if (iter != contentDict.end()) {
			if(!iter->second){
				not_in_db_nids.push_back(id);
				continue;
			}
			contents.push_back(iter->second);
			fail_ids.erase(id);
		}
	}
	if(!not_in_db_nids.empty()){
		MCE_INFO("NotifyIndexManagerI::_getContentsByIds: remove. uid:" << uid
				<< ",not_in_db_nids:" << PrintHelper::Print(not_in_db_nids));
		removeByIds(uid,not_in_db_nids);
	}
	return true;
}
bool NotifyIndexManagerI::reloadSchema(const Ice::Current&) {
	MCE_INFO("NotifyIndexManagerI::reloadSchema()");
	return SchemaManager::instance().reload();
}

void NotifyIndexManagerI::Evict(){
	map<int,int> uid_time;
	int beforesize = 0,aftersize = 0;
	{
		IceUtil::Mutex::Lock lock(evict_mutex_);
		beforesize = uid_time_.size();
		uid_time.swap(uid_time_);
	}
	set<int> uids;
	int cnt = 0,miss = 0;
	typedef map<int,int>::value_type Int2Int;
	BOOST_FOREACH(Int2Int & p,uid_time){
		if(time(NULL)-p.second > 1*60){
			NotifyListPtr nlist = locateNotifyList(p.first);
			if(nlist){
				int oldlistsize = nlist->all_notify_size();
				nlist->_evictNotify();
				MCE_INFO("NotifyIndexManagerI::Evict. uid:"<<p.first
						<< " old size:" << oldlistsize
						<< " new size:" << nlist->all_notify_size());
			}
			++cnt;
		} else {
			NeedEvict(p.first,p.second);
			++miss;
		}
	}
	{
		IceUtil::Mutex::Lock lock(evict_mutex_);
		aftersize = uid_time_.size();
	}
	MCE_INFO("NotifyIndexManagerI::Evict. cnt:"<<cnt << " miss:" << miss
			<< " before:" << beforesize <<" after:" << aftersize);
}

