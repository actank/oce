#ifndef DISPATCHMANAGERI_H_
#define DISPATCHMANAGERI_H_

#include "Notify.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "ServiceI.h"
#include <boost/foreach.hpp>
#include <set>
using namespace std;
namespace xce {
namespace notify {

const int ID_SEQ_STEP = 100;
const string DB_INSTANCE = "notify";
const string TAB_NOTIFY_ID_SEQ = "id_seq";

const int kTaskLevelDefault = 0;
const int kTaskLevelActivity = -48;
const int kTaskLevelUgcChat = -49;
const int kTaskLevelInsertDb = -50;

class NotifyInfo: public IceUtil::Shared{
public:
	void Init(Ice::Long nid,const MyUtil::Str2StrMap& props,bool store);
	bool CanChat(){
		return feed_actor_ > 0
		&& feed_source_ > 0
		&& feed_stype_ > 0
		&& from_uid_ > 0
		&& !from_name_.empty()
		&& !from_pic_.empty()
		&& is_whisper_ != 1
		&& (!ugc_content_.empty()|| !is_at_);
	}
	void SetStore(bool store) { store_ = store;	}
	bool ShouldStore() const {	return store_;	}

	void AddUgcTargets(const set<int> & ugc_uids);
	set<Ice::Int> GetStoreUids();
	set<int> GetInstantTargets();
	set<int> GetWebpagerTargets();

	string ToString()const ;
	string ToStringSimple()const ;

	long nid_;
	int schema_id_;
	bool is_at_;
	int from_uid_;
	int feed_actor_;
	long feed_source_;
	int feed_stype_;
	string from_name_;
	string from_pic_;
	string ugc_content_;
	int is_whisper_;

	set<int> uids_;
	set<int> minigroups_;
	set<int> minigroup_members_;
	set<int> ugc_uids_;

private:
	void GetMiniGroupMembers();
	bool store_;
	string to_str_;
	string tominigroup_str_;
};
typedef IceUtil::Handle<NotifyInfo> NotifyInfoPtr;


class DispatchNManagerI: public NotifyDispatcherN, public MyUtil::Singleton<
		DispatchNManagerI> {
public:
	DispatchNManagerI() :	enable_chat_(true) {}

	virtual void dispatchN(Ice::Long notify_id, bool store,
			const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());

	virtual void EnableChat(bool enable, const Ice::Current& = Ice::Current());
	virtual bool reloadSchema(const Ice::Current& = Ice::Current());
	virtual void QuitChat(::Ice::Int, const GroupIndexSeq&,
			const ::Ice::Current& = ::Ice::Current());
	virtual void JoinChat(const vector<int> & ,const GroupIndex&,
			const ::Ice::Current& = ::Ice::Current());
private:
	void SpecialProcess(const MyUtil::Str2StrMap& props,int & task_level, bool& store);
	bool enable_chat_;
};

class DispatchTask: public MyUtil::Task {
public:
	DispatchTask(int level, Ice::Long nid, const MyUtil::Str2StrMap& props,
			NotifyInfoPtr & ninfo)
		:Task(level), notify_id_(nid), props_(props), ninfo_(ninfo)	{	}
	virtual void handle();
private:
	NotifyContentPtr InitContent();
	void ToNotifyGate(NotifyContentPtr & content);
	void ToIndexAndContent(NotifyContentPtr & content);
	MyUtil::IntSeq GetToIds();
//	MyUtil::IntSeq toIdParser(const string& to, Ice::Int uid, int type);

	Ice::Long notify_id_;
	MyUtil::Str2StrMap props_;
	NotifyInfoPtr ninfo_;
};

class DbInsertTask: public MyUtil::Task {
public:
	DbInsertTask(int level,const set<int> & targets,const NotifyContentPtr & content) :
		Task(level),targets_(targets), content_(content) {}
	virtual void handle();
private:
	const static int TABLE_COUNT = 100;
	bool InsertContent();
	bool InsertIndex(int toid);
	bool InsertIndexBatch();
	string GetContentTable(long id){
		ostringstream os;
		os << "content_" << content_->id % TABLE_COUNT;
		return os.str();
	}
	string GetIndexTable(long toid){
		ostringstream os;
		os << "index_" << toid % TABLE_COUNT;
		return os.str();
	}
	set<int> targets_;
	NotifyContentPtr content_;
};

class IdParser{
public:
	static string GetValue(const MyUtil::Str2StrMap& props, const string &key) ;
	template<class ResultType>
	static ResultType GetTypedValue(const MyUtil::Str2StrMap & props, const string & key);
	static set<int> Parse(const string & to);
};

}
}

#endif /* DISPATCHMANAGERI_H_ */
