#ifndef _NOTIFYI_H_
#define _NOTIFYI_H_

#include "Notify.h"
#include "ObjectCache.h"
#include "ServiceI.h"
#include "Notify/util/PrintUtil.h"
//#include "Notify/util/rmessage_util.h"
#include "QueryRunner.h"
#include <limits.h>
#include <set>
#include "rMessageCache.h"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace Ice;
using std::set;
//const Ice::Int USER_POOL_SIZE = 5003;
const Ice::Int USER_POOL_SIZE = 5003;
//const Ice::Int NOTIFY_CACHE_SIZE = 35000000;
//const Ice::Int LOCK_POOL_SIZE = 500000;
// reply 指的是 id < 100000 的 notify

/*
const int BIGTYPE_LEFTMOVE_IndexN = 16;
const int BIGTYPE_ANDVALUE_IndexN = 16711680;
const int TYPE_ANDVALUE_IndexN = 65535;

const int GetRMessageType_IndexN(const int type){
	return type & TYPE_ANDVALUE_IndexN;
}

const int GetRMessageBigtype_IndexN(const int type){
	int result = ((type & BIGTYPE_ANDVALUE_IndexN) >> BIGTYPE_LEFTMOVE_IndexN);
	//MCE_INFO("GetRMessageBigtype-->type:" << type << " result:" << result);
	return result;
}
*/

typedef map<pair<Int,Long>,vector<pair<Int,Long> > > MergeMap;


class NotifyDbHelper {
public:
  static string getIndexTableName(Ice::Int uid) {
    ostringstream os;
    os << "index_" << uid % 100;
    return os.str();
  }

  static string getInfoTableName(Ice::Int uid) {
    ostringstream os;
    os << "notify_unread_mark_" << uid % 100;
    return os.str();
  }

  static string getDbInstance() {
    return "notify";
  }
};

enum LOAD_TYPE {
  LTYPE_ALL,  // 加载所有
  LTYPE_REPLY // 只加载回复提示
};
enum LOAD_STATE {
  LSTATE_READY,   // 可以加载数据
  LSTATE_LOADING, // 正在加载数据
  LSTATE_NOMORE   // 数据已经全部加载
};

static string PrintEnum(LOAD_TYPE type){
	if (type == LTYPE_ALL ) return "LTYPE_ALL";
	if (type == LTYPE_REPLY ) return "LTYPE_REPLY";
	return "ERRPARAM";
}

static string PrintEnum(LOAD_STATE state){
	if (state == LSTATE_READY ) return "LSTATE_READY";
	if (state == LSTATE_LOADING ) return "LSTATE_LOADING";
	if (state == LSTATE_NOMORE ) return "LSTATE_NOMORE";
	return "ERRPARAM";
}

class RemoveBase{
public:
	RemoveBase(int uid):uid_(uid){}
	virtual ~RemoveBase() {};
	void RemoveInDb();
	void CallIqMsgHandlerAndNotifyStrom();
	virtual bool WouldRemove(const NotifyIndex & idx) = 0;
	virtual void GetSql(Statement &st) = 0;
	virtual bool RemoveOneAndStop(){
		return false;
	}
	int GetUid(){
		return uid_;
	}
	virtual string GetString() = 0;
	void AddRemovedNotify(long nid,int type){
		MCE_INFO("RemoveBase:AddRemovedNotify. uid:" << uid_<< " nid:" << nid
				<< " type:" << type);
		remove_notifies_.push_back(make_pair(nid,type));
	}
protected:
	int uid_;
	vector<pair<long,int> > remove_notifies_;
};
class RemoveBySource:public RemoveBase{
public:
	RemoveBySource(int uid,int type,long source):RemoveBase(uid),type_(type),source_(source){}
	virtual ~RemoveBySource() {}
	virtual void GetSql(Statement &st);
	virtual bool WouldRemove(const NotifyIndex & idx);
	virtual string GetString(){
		ostringstream os;
		os << "RemoveBySource. uid:" << uid_ << " type:" << type_ << " source:" << source_;
		return os.str();
	}
private:
	int type_;
	long source_;
};

class RemoveBySourceSender:public RemoveBase{
public:
	RemoveBySourceSender(int uid,int type,long source,int sender):RemoveBase(uid),type_(type),source_(source),sender_(sender){}
	virtual ~RemoveBySourceSender()  {}
	virtual void GetSql(Statement &st);
	virtual bool WouldRemove(const NotifyIndex & idx);
	virtual string GetString(){
		ostringstream os;
		os << "RemoveBySourceSender. uid:" << uid_ << " type:" << type_ << " source:" << source_
				<< " sender:" << sender_;
		return os.str();
	}
private:
	int type_;
	long source_;
	int sender_;
};

class RemoveByNids:public RemoveBase{
public:
	RemoveByNids(int uid,const vector<long> & nids):RemoveBase(uid),nids_(nids.begin(),nids.end()){}
	virtual ~RemoveByNids()  {}
	virtual void GetSql(Statement &st);
	virtual bool WouldRemove(const NotifyIndex & idx);
	virtual string GetString(){
		ostringstream os;
		os << "RemoveByNids. uid:" << uid_ << " nids:" << PrintHelper::Print(nids_);
		return os.str();
	}
private:
	set<long> nids_;

};

class RemoveByNid:public RemoveBase{
public:
	RemoveByNid(int uid,long nid):RemoveBase(uid),nid_(nid){}
	virtual ~RemoveByNid()  {}
	virtual void GetSql(Statement &st);
	virtual bool WouldRemove(const NotifyIndex & idx);
	virtual bool RemoveOneAndStop();
	virtual string GetString(){
		ostringstream os;
		os << "RemoveByNid. uid:" << uid_ << " nid:" << nid_;
		return os.str();
	}
private:
	long nid_;
};

class IndexList: public Ice::Object {
public:
  void append(list<NotifyIndex>& notifies);
  void appendReply(list<NotifyIndex>& notifies);
  void addNotify(const NotifyContentPtr& notify);

  void removeInCache(RemoveBase & rq);

  MyUtil::IntSeq getFromIdSeqByType(const MyUtil::IntSeq& types,
      Ice::Int,// 该参数忽略掉
      Ice::Int limit);

  MyUtil::LongSeq getNotifyIdSeq(Ice::Long last_nid, Ice::Int limit,
      const MyUtil::IntSeq& types);
  MyUtil::LongSeq getReplyIdSeq(Ice::Long last_nid, Ice::Int limit);
  MyUtil::LongSeq getIdSeqByType(const MyUtil::IntSeq& types,
      Ice::Long last_nid, Ice::Int limit);

  MyUtil::LongSeq getNotifyIdSeq2(Ice::Int begin, Ice::Int limit,
      const MyUtil::IntSeq& types);
  MyUtil::LongSeq getReplyIdSeq2(Ice::Int begin, Ice::Int limit);
  MyUtil::LongSeq getIdSeqByType2(const MyUtil::IntSeq& types,
      Ice::Int begin, Ice::Int limit);

  MyUtil::LongSeq getIdSeqByType2Test(const MyUtil::IntSeq& types,
      Ice::Int begin, Ice::Int limit);

  ////////////////////////////////////////////
  MergeMap getNotifyIdSeqMerge(Ice::Long last_nid, Ice::Int limit,
      const MyUtil::IntSeq& types,MyUtil::LongSeq & not_merge_ids);
  MergeMap getReplyIdSeqMerge(Ice::Long last_nid, Ice::Int limit,MyUtil::LongSeq & not_merge_ids);

  MergeMap getNotifyIdSeqX(Ice::Long last_nid, Ice::Int view ,Ice::Int limit,
      const MyUtil::IntSeq& types,MyUtil::LongSeq & not_merge_ids);
  MergeMap getReplyIdSeqX(Ice::Long last_nid,Ice::Int view , Ice::Int limit,MyUtil::LongSeq & not_merge_ids);
  ////////////////////////////////////////////

  Ice::Int getCountByType(const MyUtil::IntSeq& types);

  Ice::Int getUnreadCount(const vector<int> & types,bool bytype,int view = -1);

  void set_min_unread_id(Ice::Long nid) {
    if (nid > 0)
      _min_unread_nid = nid;
  }

  Ice::Long min_unread_nid() {
    return _min_unread_nid;
  }

  void setLoadState(LOAD_TYPE type, LOAD_STATE state) {
    if (type == LTYPE_ALL) {
      _loadAllState = state;
    } else {
      _loadReplyState = state;
    }
  }

  string dump();
  Ice::Int uid() {
    return _uid;
  }

  size_t all_notify_size() const {
    size_t count = 0;
    list<NotifyIndex>::const_iterator i = _reply_list.begin();
    for(; i != _reply_list.end() ; ++i)
    {
      if(i->id < _min_notify_id)
        break;
      ++count;
    }
    return count + _other_list.size();
  }

  Ice::Long min_notify_id() {
    return _min_notify_id;
  }
  Ice::Long getMinReplyId();

  size_t loadAllThreshold() {
    return _load_all_threshold;
  }
  size_t loadReplyThreshold() {
    return _load_reply_threshold;
  }

  IceUtil::Mutex& mutex() {
    return _mutex;
  }
  void _evictNotify();
  void CheckNeedEvict();
private:
  friend class NotifyListFactoryI;
  IndexList(Ice::Int uid) //只能由Factory来创建
    : _uid(uid)
    , _min_notify_id(LLONG_MAX)
    , _load_all_threshold(30)//TODO
    , _load_reply_threshold(5)
    , _loadAllState(LSTATE_READY)
    , _loadReplyState(LSTATE_READY)
    , _add_count(0)
    , _min_unread_nid(LLONG_MAX)//初始化时，表示全部展示过
  {
  }


  void _backSplice(list<NotifyIndex>& dest, list<NotifyIndex>& src);
  void _removeListById(Ice::Long notifyId);

  bool _RemoteNotifyFromList(
  		RemoveBase & method,
  		list<NotifyIndex> & list);
  void _load(LOAD_TYPE type, size_t begin_pos);

  // TODO : 每个对象都有一个锁，是否粒度过细？
  IceUtil::Mutex _mutex;
  Ice::Int _uid;
  list<NotifyIndex> _reply_list;
  list<NotifyIndex> _other_list;

  Ice::Long _min_notify_id;

  // DB预加载的门限值
  size_t _load_all_threshold; // size() 小于该值的时候，要预加载
  size_t _load_reply_threshold; // replySize() 小于该值的时候，要预加载

  // TODO : 合并 & 使用更小的类型
  LOAD_STATE _loadAllState;
  LOAD_STATE _loadReplyState;

  //用于回收数据. add若干次之后，要删除
  int _add_count;

  // pager 上位显示过的最小的id. LLONG_MAX 表示全都显示过.
  Ice::Long _min_unread_nid;
};

typedef IceUtil::Handle<IndexList> NotifyListPtr;

class NotifyIndexManagerI: public NotifyIndexManagerN, public MyUtil::Singleton<
    NotifyIndexManagerI> {
public:
	
	NotifyIndexManagerI(){
		for(int i = 0; i < USER_POOL_SIZE; ++i){
			UserMessagePoolPtr p = new UserMessagePool;
			_user_rmessage_pool.push_back(p);
		}
	}

  virtual void addNotify(const NotifyContentPtr& r,
      const MyUtil::IntSeq& target, const Ice::Current& = Ice::Current());

  virtual void removeById(Ice::Int uid, Ice::Long notifyId,
      const Ice::Current& = Ice::Current());
  virtual void removeByIds(Ice::Int uid, const MyUtil::LongSeq & nids,
      const Ice::Current& = Ice::Current());
  virtual void removeBySource(Ice::Int uid, Ice::Int type, Ice::Long source,
      const Ice::Current& = Ice::Current());
  virtual void removeBySourceAndSender(Ice::Int uid, Ice::Int type, Ice::Long source, Ice::Int sender,
      const Ice::Current& = Ice::Current());
  void removeImpl(RemoveBase & rq);

  virtual string dumpList(Ice::Int uid, const Ice::Current& = Ice::Current());

  // 以最大 notify id  的下一个id 为起点get
  virtual NotifyContentSeq getNotifySeq(Ice::Int uid, Ice::Long nid,
      Ice::Int limit, const Ice::Current& = Ice::Current());
  virtual NotifyContentSeq getReplySeq(Ice::Int uid, Ice::Long nid,
      Ice::Int limit, const Ice::Current& = Ice::Current());
  virtual NotifyContentSeq getByType(Ice::Int uid,
      const MyUtil::IntSeq& types, Ice::Long nid, Ice::Int limit,
      const Ice::Current& = Ice::Current());

  // 以第 begin 个为起点get
  virtual NotifyContentSeq getNotifySeq2(Ice::Int uid, Ice::Int begin,
      Ice::Int limit, const Ice::Current& = Ice::Current());
  virtual NotifyContentSeq getReplySeq2(Ice::Int uid, Ice::Int begin,
      Ice::Int limit, const Ice::Current& = Ice::Current());
  virtual NotifyContentSeq getByType2(Ice::Int uid,
      const MyUtil::IntSeq& types, Ice::Int begin, Ice::Int limit,
      const Ice::Current& = Ice::Current());

  // 废弃
  virtual NotifyContentSeq getNotifySeqMerge(Ice::Int uid, Ice::Long nid,
      Ice::Int limit, const Ice::Current& = Ice::Current());
  virtual NotifyContentSeq getReplySeqMerge(Ice::Int uid, Ice::Long nid,
      Ice::Int limit, const Ice::Current& = Ice::Current());

  // 以最大 notify id  的下一个id 为起点get merge
  virtual NotifyContentSeq getNotifySeqX(Ice::Int uid, Ice::Int view, Ice::Long nid,
      Ice::Int limit, const Ice::Current& = Ice::Current());
  virtual NotifyContentSeq getReplySeqX(Ice::Int uid,Ice::Int view, Ice::Long nid,
      Ice::Int limit, const Ice::Current& = Ice::Current());


  MyUtil::IntSeq getFromIdSeq(Ice::Int uid, const MyUtil::IntSeq & types,
      Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());


  virtual Ice::Int getCount(Ice::Int uid, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
  virtual Ice::Int getUnreadCount(Ice::Int uid, const Ice::Current& = Ice::Current());
  virtual Ice::Int getUnreadCountByView(Ice::Int uid, Ice::Int view, const Ice::Current& = Ice::Current());
  virtual Ice::Int getUnreadCountByTypes(Ice::Int uid,const vector<int> & types, const Ice::Current& = Ice::Current());
  virtual Ice::Int getUnreadCountByTypesAndMaxReadId(Ice::Int uid,const vector<int> & types, Ice::Int MaxReadId,const Ice::Current& = Ice::Current());

  virtual bool reloadSchema(const Ice::Current& =  Ice::Current());
  void Evict();
  void NeedEvict(int uid, int time) {
    IceUtil::Mutex::Lock lock(evict_mutex_);
    uid_time_[uid] = time;
  }
  virtual void AddTester(int tester,const Ice::Current& =  Ice::Current()){
  	if(tester <= 0){ level_ = -tester; }
  	else {testers_.insert(tester);}
  	MCE_INFO("AddTester. tester:" << tester << " level:" << level_
  			<< " testers:" << PrintHelper::Print(testers_))
  }

	//================RMESSAGE====================
	void AppendUserRMessages(int userid, int bigtype, const list<NotifyIndex>& list, bool update_load_stat = true);
	void UpdateUserBigtypeLoadStat(int userid, int bigtype,  bool isload);
	void UserRMessageEvict(int userid);
	void UserRMessageDoEvict();
	virtual NotifyIndexSeq GetRMessageByBigtype(int uid, int begin, int limit, int bigtype, const Ice::Current& = Ice::Current());
	virtual NotifyIndexSeq GetRMessageBeforLastNid(int userid, int view, Ice::Long nid, int limit, int bigtype, const Ice::Current& = Ice::Current());
	virtual NotifyIndexSeq GetRMessageByType(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype, const Ice::Current& = Ice::Current());
	virtual NotifyIndexSeqMap GetRMessageByTypeAll(int userid, const MyUtil::IntSeq& types, int begin, int limit, const Ice::Current& = Ice::Current());
	virtual NotifyIndexSeq GetRMessageMerge(int userid, int view, Ice::Long nid, int limit, int bigtype, const Ice::Current& = Ice::Current());
	virtual int GetUnreadCountByBigtype(int userid, int bigtype, const Ice::Current& = Ice::Current());
	virtual MyUtil::Int2IntMap GetUnreadCountByBigtypes(int userid, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
	virtual void RemoveRMessageById(int uid, long notifyId, int type, int bigtype, const Ice::Current& = Ice::Current());
	virtual void RemoveRMessageByIds(int uid, const MyUtil::LongSeq& notifyIds, int type, int bigtype, const Ice::Current& = Ice::Current());
	virtual void RemoveRMessageBySource(int uid, int type, long source, int bigtype, const Ice::Current& = Ice::Current());
	virtual void RemoveRMessageBySourceSeq(int uid, int type, const MyUtil::LongSeq& source, int bigtype, const Ice::Current& = Ice::Current());
	virtual void RemoveRMessageBySourceAndSender(int uid, int type, long source, int sender, int bigtype, const Ice::Current& = Ice::Current());
	virtual int GetUndealCountByBigtype(int uid, int bigtype, const Ice::Current& = Ice::Current());
	virtual int GetUndealCountByType(int uid, int type, const Ice::Current& = Ice::Current());
	virtual MyUtil::Int2IntMap GetUndealCountByTypes(int uid, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
	//================RMESSAGE====================

private:
  NotifyListPtr locateNotifyList(Ice::Int uid) {
    return ServiceI::instance().locateObject<NotifyListPtr> ((uid / 100)
        % 10, uid);
  }
  NotifyListPtr findNotifyList(Ice::Int uid) {
    return ServiceI::instance().findObject<NotifyListPtr> (
        (uid / 100) % 10, uid);
  }
  bool _getContentsByIds(int uid,const MyUtil::LongSeq & ids,
      NotifyContentSeq & contents,set<Ice::Long> & fail_ids);

	map<int,int> uid_time_;
  IceUtil::Mutex evict_mutex_;
  set<long> testers_;
  int level_;
	
	//================RMESSAGE====================
	UserMessagePoolPtr GetRMessagePool(int userid){
		return _user_rmessage_pool[userid % USER_POOL_SIZE];
	}
	vector <UserMessagePoolPtr> _user_rmessage_pool;
};

class UpdateUnreadNidTask : public MyUtil::Task {
public:
  UpdateUnreadNidTask(const NotifyListPtr replyList)
    : _notifyList(replyList) {
  }

  virtual void handle();
private:
  NotifyListPtr _notifyList;
};

class LoadNotifyListTask: public MyUtil::Task {
public:
  LoadNotifyListTask(LOAD_TYPE type, const NotifyListPtr & replyList) :
    _loadType(type), _notifyList(replyList) {
  }

  virtual void handle();
private:
  const LOAD_TYPE _loadType;
  NotifyListPtr _notifyList;
};

class NotifyListFactoryI: public ServantFactory {
public:
  virtual Ice::ObjectPtr create(Ice::Int id) {
    NotifyListPtr reply = new IndexList(id);
    MyUtil::TaskPtr task = new LoadNotifyListTask(LTYPE_ALL, reply);
    task->handle();
    return reply;
  }
  virtual ObjectResultPtr create(const MyUtil::IntSeq& ids) {
    ObjectResultPtr res = new ObjectResult();
    for (size_t i = 0; i < ids.size(); ++i) {
      res->data[ids.at(i)] = create(ids.at(i));
    }
    return res;
  }
};

class EvictTimer:public MyUtil::Timer{
public:
	EvictTimer() :Timer(30*1000) {}
  virtual void handle() {
  	NotifyIndexManagerI::instance().Evict();
  }
private:
};

}
}

#endif // _NOTIFYI_H_
