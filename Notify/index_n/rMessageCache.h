#ifndef _RMESSAGECACHE_H_
#define _RMESSAGECACHE_H_


#include "Notify.h"
#include "ObjectCache.h"
#include "ServiceI.h"
#include "Notify/util/PrintUtil.h"
#include "QueryRunner.h"
#include <limits.h>
#include <set>


#include <Ice/Ice.h>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace xce{
namespace notify{


using boost::multi_index_container;
using namespace boost::multi_index;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace Ice;
using std::set;


const int REPLY_TYPE= 1;
const int REQUEST_TYPE= 2;
const int INFORM_TYPE= 3;
const int CACHESIZE= 200;


const int LOAD_STAT_BUSY= 2;
const int LOAD_STAT_FREE = 1;




static string GetIndexTableName(Ice::Int uid) {
	ostringstream os;
	os << "index_" << uid % 100;
	return os.str();
}

static string GetInfoTableName(Ice::Int uid) {
	ostringstream os;
	os << "notify_unread_mark_" << uid % 100;
	return os.str();
}

static string GetDbInstance() {
	return "notify";
}


//用户消息列表基类
class MessageBase : public Ice::Object{
public:

	MessageBase(int first_load_count, int preload_threshold_value, int bigtype, int userid){
		_first_load_count = first_load_count;
		_preload_threshold_value = preload_threshold_value;
		_bigtype = bigtype;
		_userid = userid;
		_evict_threshold_value = 30;
		_unread_count = 0;
		_undeal_count= 0;
		_load_stat = LOAD_STAT_FREE;
		_isload= true;
	}

	//virtual Ice::Long GetMinUnreadId() = 0;//获取最小未读message id
	virtual void GetRemoveByIdSql(Statement& sql, Ice::Long nid) = 0;//获得单独删除的sql
	virtual void GetRemoveByIdsSql(Statement& sql, const MyUtil::LongSeq& nids) = 0;//获得批量删除的sql
	virtual void GetRemoveBySourceSql(Statement& sql) = 0;//获得按照source和type删除的sql
	virtual void GetRemoveBySourceAndSenderSql(Statement& sql, int type,  Ice::Long source, int sender) = 0;//获得按照source,type,sender删除的sql
	virtual void FirstLoad();//创建对象首次加载
	virtual void PreLoad();//预加载
	virtual void Evict();//逐出
	virtual void AppendMessages(const list<NotifyIndex>& seq, bool update_load_stat);//批量添加
	virtual void UpdateUserBigtypeLoadStat(bool isload);
	virtual void AddMessage(const NotifyContentPtr& notify);//添加一条消息
	virtual void RemoveById(Ice::Long nid, int type);
	virtual void RemoveByIds(const MyUtil::LongSeq& notifyids, int type);
	virtual void RemoveBySource(int type, Ice::Long source);
	virtual void RemoveBySourceAndSender(int type, Ice::Long source, int sender);
	NotifyIndexSeq GetSeqByBigtype(int begin, int limit);
	NotifyIndexSeq GetSeqBeforLastNid(int view, Ice::Long nid, int limit);
	NotifyIndexSeq GetSeqBeforMerge(int view, Ice::Long nid, int limit);
	NotifyIndexSeq GetSeqByTypeAndBigtype(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype);
	NotifyIndexSeqMap GetSeqByTypeAndBigtypeAll(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype);
	int GetUnreadCountByBigtype();

	int GetUndealCountByBigtype();
	int GetUndealCountByType(int type);

	void UpdateUndealCount(int type, int value){
		ostringstream os;
	//	os << "UpdateUndealCount--> BEFOR useid : "<< _userid <<　" _isload:" << isload << " bigtype:" << _bigtype << " type:" << type << " update value:" << value << " -- ";
		os << "UpdateUndealCount--> BEFOR useid : " << _userid << " _isload:" << _isload << " bigtype:" << _bigtype << " type:" << type << " update value:" << value << " -- ";
		for(map<int, int>::iterator it = _type_undeal_count.begin(); it != _type_undeal_count.end(); ++it){
			os << " stype:" << it->first << " value:" << it->second << ";";
		} 
		MCE_DEBUG(os.str());
		if(_type_undeal_count.find(type) != _type_undeal_count.end()){
			int v = _type_undeal_count[type];
			v -= value;
			if(v < 0){
				v = 0;
			}
			_type_undeal_count[type] = v;
		}
		_undeal_count -= value;
		if(_undeal_count < 0){
			_undeal_count = 0;
		}
		os.str("");
		os << "UpdateUndealCount--> AFTER useid : " << _userid << " bigtype:" << _bigtype << " type:" << type << " update value:" << value << " -- ";
		for(map<int, int>::iterator it = _type_undeal_count.begin(); it != _type_undeal_count.end(); ++it){
			os << " stype:" << it->first << " value:" << it->second << ";";
		} 
		MCE_DEBUG(os.str());
	}

	NotifyIndexSeq LoadSeqFromDBNow(const set<int>& types, int begin,int wantsize);
	NotifyIndexSeq LoadSeqFromDBNowMerge(int begin,int wantsize);

	void CheckAndLoadFromDB(int leftvalue, const set<Ice::Long>& blacklist_nids);

	Ice::Long GetMinMessageId(){//获取cache内最小的NotifyId
		if(!_messagelist.empty()){
			return _messagelist.back().id;
		}else{
			return LLONG_MAX;
		}
	}

	list<NotifyIndex> _messagelist;//用户消息队列
	Ice::Long _min_unread_id;//最小未读 message id

	int _bigtype;//标记是 提醒 申请 消息 的大类型
	int _preload_threshold_value;//db 预加载值
	int _first_load_count;//第一次加载的数量
	int _userid;
	int _evict_threshold_value; //消息列表长度,逐出限制值
	int _unread_count; // 未读条数
	int _undeal_count; //所有未处理数
	int _load_stat; //db加载状态
	bool _isload; //是否已经加载到这个用户的最后一条，如果是，isload = false 代表不需要再加载了。逐出后isload = true;
	map<int, int> _type_undeal_count;//按照type分类的未处理条数  sum it == _undeal_count
};

class ReplyMessage : public MessageBase{
public:
	ReplyMessage(int first_load_count, int preload_threshold_value, int bigtype, int userid) : MessageBase(first_load_count, preload_threshold_value, bigtype, userid){
	}
	//virtual Ice::Long GetMinUnreadId(){};	
	virtual void GetRemoveByIdSql(Statement& sql, Ice::Long nid){
		sql << "delete from " << GetIndexTableName(_userid) << " where to_id = " << _userid << " and id = " << nid;
	};//获得单独删除的sql
	virtual void GetRemoveByIdsSql(Statement& sql, const MyUtil::LongSeq& nids){
		sql << "DELETE FROM " << GetIndexTableName(_userid)
			<< " WHERE to_id = " << _userid << " AND id IN (";
		bool first = true;
		for(MyUtil::LongSeq::const_iterator it = nids.begin(); it != nids.end(); ++it){
			Ice::Long nid = *it;
			if(!first){
				sql << ',';
			}
			sql << nid;
			first = false;
		}
		sql << ')';

	};//获得单独删除的sql
	virtual void GetRemoveBySourceSql(Statement& sql){
	};//获得按照source,type删除的sql
	virtual void GetRemoveBySourceAndSenderSql(Statement& sql, int type, Ice::Long source, int sender){
		sql << "DELETE FROM " << GetIndexTableName(_userid) << " WHERE to_id = " << _userid << " AND type = " << type << " AND source = " << source << " AND from_id = " << sender;

	};//获得按照source,type,sender删除的sql
	
};

class RequestMessage : public MessageBase{
public:
	RequestMessage(int first_load_count, int preload_threshold_value, int bigtype, int userid) : MessageBase(first_load_count, preload_threshold_value, bigtype, userid){
	}
	//virtual Ice::Long GetMinUnreadId(){};	
	virtual void GetRemoveByIdSql(Statement& sql, Ice::Long nid){
		sql << "delete from " << GetIndexTableName(_userid) << " where to_id = " << _userid << " and id = " << nid;
	};//获得单独删除的sql
	virtual void GetRemoveByIdsSql(Statement& sql, const MyUtil::LongSeq& nids){
		sql << "DELETE FROM " << GetIndexTableName(_userid)
			<< " WHERE to_id = " << _userid << " AND id IN (";
		bool first = true;
		for(MyUtil::LongSeq::const_iterator it = nids.begin(); it != nids.end(); ++it){
			Ice::Long nid = *it;
			if(!first){
				sql << ',';
			}
			sql << nid;
			first = false;
		}
		sql << ')';

	};//获得单独删除的sql
	virtual void GetRemoveBySourceSql(Statement& sql){};//获得按照source,type删除的sql
	virtual void GetRemoveBySourceAndSenderSql(Statement& sql, int type, Ice::Long source, int sender){
		sql << "DELETE FROM " << GetIndexTableName(_userid) << " WHERE to_id = " << _userid << " AND type = " << type << " AND source = " << source << " AND from_id = " << sender;
	};//获得按照source,type,sender删除的sql

};

class InformMessage : public MessageBase{
public:
	InformMessage(int first_load_count, int preload_threshold_value, int bigtype, int userid) : MessageBase(first_load_count, preload_threshold_value, bigtype, userid){
	}
	//virtual Ice::Long GetMinUnreadId(){};	
	virtual void GetRemoveByIdSql(Statement& sql, Ice::Long nid){
		sql << "delete from " << GetIndexTableName(_userid) << " where to_id = " << _userid << " and id = " << nid;
	};//获得单独删除的sql
	virtual void GetRemoveByIdsSql(Statement& sql, const MyUtil::LongSeq& nids){
		sql << "DELETE FROM " << GetIndexTableName(_userid)
			<< " WHERE to_id = " << _userid << " AND id IN (";
		bool first = true;
		for(MyUtil::LongSeq::const_iterator it = nids.begin(); it != nids.end(); ++it){
			Ice::Long nid = *it;
			if(!first){
				sql << ',';
			}
			sql << nid;
			first = false;
		}
		sql << ')';

	};//获得单独删除的sql
	virtual void GetRemoveBySourceSql(Statement& sql){};//获得按照source,type删除的sql
	virtual void GetRemoveBySourceAndSenderSql(Statement& sql, int type, Ice::Long source, int sender){
		sql << "DELETE FROM " << GetIndexTableName(_userid) << " WHERE to_id = " << _userid << " AND type = " << type << " AND source = " << source << " AND from_id = " << sender;
		};//获得按照source,type,sender删除的sql
};

typedef IceUtil::Handle<MessageBase> MessageBasePtr;
typedef IceUtil::Handle<ReplyMessage> ReplyMessagePtr;
typedef IceUtil::Handle<RequestMessage> RequestMessagePtr;
typedef IceUtil::Handle<InformMessage> InformMessagePtr;

class UserMessage : public Ice::Object{
public:

	UserMessage(int userid){
		_reply_message = new ReplyMessage(20, 10, 1, userid);
		//_reply_message = new ReplyMessage(120,60, 1, userid);
		_request_message = new RequestMessage(20, 8, 2, userid);
		_inform_message = new InformMessage(20, 8, 3, userid);
		_userid = userid;
	}
	void FirstLoad(){
		_reply_message->FirstLoad();
		_request_message->FirstLoad();
		_inform_message->FirstLoad();
	}
	MessageBasePtr GetList(int bigtype){
		if(REPLY_TYPE == bigtype){
			return _reply_message;
		}
		if(REQUEST_TYPE == bigtype){
			return _request_message;
		}
		if(INFORM_TYPE == bigtype){
			return _inform_message;
		}
		return NULL;
	}

	int GetMinMessageId(int bigtype){
		int ans = 0;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetMinMessageId();
		}
		return ans;
	}

	void AddMessage(const NotifyContentPtr& content);//添加一条消息

	void AppendMessages(const list<NotifyIndex>& seq, int bigtype, bool update_load_stat){
		MessageBasePtr list = GetList(bigtype);
		if(list){
			list->AppendMessages(seq, update_load_stat);
		}
	}
	void UpdateUserBigtypeLoadStat(int bigtype, bool isload){
		MessageBasePtr list = GetList(bigtype);
		if(list){
			list->UpdateUserBigtypeLoadStat(isload);
		}
	}
	void RemoveById(Ice::Long notifyid, int type, int bigtype){
		MessageBasePtr list = GetList(bigtype);
		if(list){
			list->RemoveById(notifyid, type);
		}
	}

	void RemoveByIds(const MyUtil::LongSeq& notifyids, int type, int bigtype){
		MessageBasePtr list = GetList(bigtype);
		if(list){
			list->RemoveByIds(notifyids, type);
		}
	}
	void RemoveBySource(Ice::Int type, Ice::Long source, int bigtype){
		MessageBasePtr list = GetList(bigtype);
		if(list){
			list->RemoveBySource(type, source);
		}
	}
	void RemoveBySourceAndSender(Ice::Int type, Ice::Long source, int sender, int bigtype){
		MessageBasePtr list = GetList(bigtype);
		if(list){
			list->RemoveBySourceAndSender(type, source, sender);
		}
	}
	NotifyIndexSeq GetSeqByBigtype(int begin, int limit, int bigtype){
		NotifyIndexSeq ans;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetSeqByBigtype(begin, limit);
		}
		return ans;
	}
	
	NotifyIndexSeq GetSeqBeforLastNid(int view, Ice::Long nid, int limit, int bigtype){
		NotifyIndexSeq ans;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetSeqBeforLastNid(view, nid, limit);
		}
		return ans;
	}

	NotifyIndexSeq GetSeqBeforMerge(int view, Ice::Long nid, int limit, int bigtype){
		NotifyIndexSeq ans;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetSeqBeforMerge(view, nid, limit);
		}
		return ans;
	}

	NotifyIndexSeq GetSeqByTypeAndBigtype(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype){
		NotifyIndexSeq ans;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetSeqByTypeAndBigtype(userid, types, begin, limit, bigtype);
		}
		return ans;
	}
	NotifyIndexSeqMap GetSeqByTypeAndBigtypeAll(int userid, const MyUtil::IntSeq& types,int begin, int limit, int bigtype){
		NotifyIndexSeqMap ans;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetSeqByTypeAndBigtypeAll(userid, types, begin, limit, bigtype);
		}
		return ans;
	}

	int GetUnreadCountByBigtype(int bigtype){
		int ans = 0;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetUnreadCountByBigtype();
		}
		return ans;
	}

	int GetUndealCountByBigtype(int bigtype){
		int ans = 0;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetUndealCountByBigtype();
		}
		return ans;
	}
	int GetUndealCountByType(int type, int bigtype){
		int ans = 0;
		MessageBasePtr list = GetList(bigtype);
		if(list){
			ans = list->GetUndealCountByType(type);
		}
		return ans;
	}
	void Evict(){
		_reply_message->Evict();
		_request_message->Evict();
		_inform_message->Evict();
	}

	ReplyMessagePtr _reply_message;	
	RequestMessagePtr _request_message;	
	InformMessagePtr _inform_message;	
	
	int _userid;
};
typedef IceUtil::Handle<UserMessage> UserMessagePtr;




typedef multi_index_container<
  UserMessagePtr,
	indexed_by<
		sequenced<>,
		hashed_unique< member<UserMessage, int, &UserMessage::_userid> >
	>
> UserMessageCache;

typedef UserMessageCache::nth_index<0>::type SequencedIndex;
typedef UserMessageCache::nth_index<1>::type UserIdIndex;





class UserMessagePool : public Ice::Object{
public:

	void AddUserMessage(const UserMessagePtr& usermessage);
	UserMessagePtr LocateUserMessage(int userid);
	UserMessagePtr FindUserMessage(int userid);

//================================================================================
	void AddMessage(const NotifyContentPtr& r, const MyUtil::IntSeq& target);
	void UpdateUserBigtypeLoadStat(int userid, int bigtype, bool isload);
	void AppendMessages(const list<NotifyIndex>& seq, int userid, int bigtype, bool update_load_stat);

	void RemoveById(Ice::Int uid, Ice::Long notifyId, int type, int bigtype);
	void RemoveByIds(Ice::Int uid, const MyUtil::LongSeq & nids, int type, int type);
	void RemoveBySource(Ice::Int uid, Ice::Int type, Ice::Long source, int bigtype);
	void RemoveBySourceAndSender(Ice::Int uid, Ice::Int type, Ice::Long source, Ice::Int sender, int bigtype);

	void UserMessageEvict(int userid);
	void DoEvict();

	NotifyIndexSeq GetSeqByBigtype(int userid, int begin, int limit, int bigtype);//按照bigtype获取列表里面的内容 begin and limit
	NotifyIndexSeq GetSeqBeforLastNid(int userid, int view, Ice::Long nid, int limit, int bigtype);//按照上次获取的nid来获取notify没有合并
	NotifyIndexSeq GetSeqBeforMerge(int userid, int view, Ice::Long nid, int limit, int bigtype);//按照上次获取的nid来获取notify,有合并
	NotifyIndexSeq GetSeqByTypeAndBigtype(int userid, const MyUtil::IntSeq& types,int begin, int limit, int bigtype);//按照type来获取内存中的所有的某个type
	NotifyIndexSeqMap GetSeqByTypeAndBigtypeAll(int userid, const MyUtil::IntSeq& types,int begin, int limit, int bigtype);//按照type来获取内存中的所有的某个type

	int GetUndealCountByBigtype(int uid, int bigtype);
	int GetUndealCountByType(int uid, int type, int bigtype);

	int GetUnreadCountByBigtype(int userid, int bigtype);
	IceUtil::RWRecMutex _rwmutex;
	IceUtil::Mutex _cachemutex;
	UserMessageCache _cache;
	map<int,int> _user_time;
};
typedef IceUtil::Handle<UserMessagePool> UserMessagePoolPtr;

//===================================================================================

class RemoveDBTask : public MyUtil::Task{
public:
	RemoveDBTask(const Statement& sql, int userid):_sql(sql), _userid(userid){
	}
	virtual void handle(){
		try {
			QueryRunner(GetDbInstance(), CDbWServer, GetIndexTableName(_userid)).store(_sql);
		} catch (mysqlpp::Exception& e) {
			MCE_WARN("RemoveBase::RemoveInDb " /*<<  GetString()*/ << " err: " << e.what() << " at " << __FILE__ << ":" << __LINE__);
		} catch (...) {
			MCE_WARN("RemoveBase::RemoveInDb " /*<<  GetString()*/ <<" unknown error at " << __FILE__ << ":" << __LINE__);
		}
	};
private:
	Statement _sql;
	int _userid;
};

class UpdateMinUnreadIdTask : public MyUtil::Task{
public:
	UpdateMinUnreadIdTask(int userid, Ice::Long min_unread_id):_userid(userid), _min_unread_id(min_unread_id){
	}
	virtual void handle();
private:
	int _userid;
	Ice::Long _min_unread_id;
};

class LoadMessageTask : public MyUtil::Task{
public:
	LoadMessageTask(int userid, int bigtype, int loadcount, Ice::Long min_message_id, const set<Ice::Long>& blacklist_nids):_userid(userid), _bigtype(bigtype), _loadcount(loadcount), _min_message_id(min_message_id), _blacklist_nids(blacklist_nids){
	}
	virtual void handle();
private:
	int _userid;
	int _bigtype;
	Ice::Long _min_message_id;
	int _loadcount;
	set<Ice::Long> _blacklist_nids;
};

class MessageEvictTimer : public MyUtil::Timer{
public:
	MessageEvictTimer() : Timer(1000 * 60){
	}
	virtual void handle();
};


};
};
#endif //_RMESSAGECACHE_H_
