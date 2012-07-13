#ifndef __FOOTPRINTNEW_I_H__
#define __FOOTPRINTNEW_I_H__

#include "FootprintNew.h"
#include "ServiceI.h"
#include "Date.h"
#include "QueryRunner.h"

#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>
namespace xce{
namespace fptn {

const Ice::Int FOOTPRINT_SET = 0;
const Ice::Int USER_VISIT_COUNT = 3;

//const string COLUMN_ID = "ID";
const static char* COLUMN_HOST = "HOST";
const static char* COLUMN_GUESTS = "GUESTS";
const static string TEST = "hello world!";

const static string TABLE_VIEW_COUNT = "view_count";
const string COLUMN_ID = "ID";
const string COLUMN_VIEWCOUNT = "viewcount";

const static int TASK_LEVEL_BUILD = 1;
const static int TASK_LEVEL_SAVE = 2;
const static int TASK_LEVEL_ADD = 3;
const static int TASK_LEVEL_REMOVE = 4;
//const static int TASK_LEVEL_REFRESH = 4;
const Ice::Int REACHLIMIT = 0;
const Ice::Int PASS = 1;
const Ice::Int PASS_RESET = 2;

// const static int SAVE_BATCH_SIZE = 200;
// const static time_t REFRESH_DB_TIME = 10;
const static int GUESTS_SIZE = 6;  
const static int VIEW_COUNT_LIMIT = 500;
const static int MIN_LIMIT = 200000000;		// 分钟和秒计时的一个界限

class FootprintSet;

// sort by time
class SortFootprint {
public:
	static bool OrderByTimeDesc(const FootprintTinyPtr& p1, const FootprintTinyPtr& p2) {
		return p1->timestamp > p2->timestamp;
	}
};

//add serialize
class IntSeqSerialize {
public:
   static std::string serialize(FootprintSeq seq) {
	  string result;
	  size_t guests_size = seq.size() > size_t(GUESTS_SIZE) ? size_t(GUESTS_SIZE):seq.size();
	  vector<int> byteSeq;
	  for ( size_t i = 0; i < guests_size; ++i ) {
		  // 从旧的分钟计时到新的秒计时的转换判断
		  if ( seq[i]->timestamp < MIN_LIMIT ) {
			  seq[i]->timestamp *= 60;
		  }
		  byteSeq.push_back(seq[i]->guest);
		  byteSeq.push_back(seq[i]->timestamp);
	  }

	  result.resize(guests_size * 2 * sizeof(int));
	  memcpy((void*)(result.data()), &byteSeq[0], guests_size*2*sizeof(int));
	  return result;
   }
   static FootprintSeq unserialize(const mysqlpp::Row::value_type & str) {
	FootprintSeq result;
	vector<int> tmp;	
  	tmp.resize((str.size()/sizeof(int)/2)*2);	// 之所以要先/2再*2,因为两个int成员才能组成一个footprint
  	memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));
	for ( size_t i = 0; i < tmp.size()/2; ++i ) {
		FootprintTinyPtr fptn = new FootprintTiny;
		fptn->guest = tmp[2*i];
		fptn->timestamp = tmp[2*i+1];
		if ( fptn->timestamp < MIN_LIMIT ) {
			fptn->timestamp *= 60;
		}
		if ( (fptn->guest > 0) && (fptn->timestamp >0) ) {
			result.push_back(fptn);
		}
	}
	sort(result.begin(), result.end(), SortFootprint::OrderByTimeDesc);

  	return result;
   }

   static FootprintSeq unserialize(const string & str) {

	FootprintSeq result;
	vector<int> tmp;	
  	tmp.resize((str.size()/sizeof(int)/2)*2);	// 之所以要先/2再*2,因为两个int成员才能组成一个footprint
  	memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));
	for ( size_t i = 0; i < tmp.size()/2; ++i ) {
		FootprintTinyPtr fptn = new FootprintTiny;
		fptn->guest = tmp[2*i];
		fptn->timestamp = tmp[2*i+1];
		if ( fptn->timestamp < MIN_LIMIT ) {
			fptn->timestamp *= 60;
		}
		if ( (fptn->guest > 0) && (fptn->timestamp >0) ) {
			result.push_back(fptn);
		}
	}
	sort(result.begin(), result.end(), SortFootprint::OrderByTimeDesc);

  	return result;
    }
};


class FootprintSet : virtual public Ice::Object,
		virtual public IceUtil::Mutex {
public:
	FootprintSet()
		:viewCountLimitTime_((time(NULL)/(24*60*60))*(24*60*60)),
		viewCountLimtCount_(0){}
	Ice::Int getSize();
	FootprintSeq getAll(Ice::Int begin, Ice::Int limit);
	bool add(const FootprintPtr& ftprt, bool ignorelimitstatus);
	void merge(const IceUtil::Handle<FootprintSet>& fptset);
	FootprintSeq& getFootprints() {
		return footprints_;
	}
	void clear();
	void setErrorFlag();
	bool isError();

private:
	int isReachViewCountLimit(int hostId);

	time_t viewCountLimitTime_;
	int viewCountLimtCount_;
	FootprintSeq footprints_;
};
typedef IceUtil::Handle<FootprintSet> FootprintSetPtr;

class UserVisitCount : virtual public Ice::Object, 
		virtual public IceUtil::Mutex {

public:
	void incCount(Ice::Int step);
	int getCount();
	void setCount(Ice::Int count);
private:
	int _count;
};
typedef IceUtil::Handle<UserVisitCount> UserVisitCountPtr;

class FootprintFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class FootprintBuildTask : public MyUtil::Task {
public:
	FootprintBuildTask(int id):MyUtil::Task(TASK_LEVEL_BUILD),_id(id) {};
	virtual void handle();
private:
	int _id;
};
/*
class FootprintRefreshTask : public MyUtil::Task {
public:
	FootprintRefreshTask(int id, FootprintSetPtr& obj):MyUtil::Task(TASK_LEVEL_REFRESH),_id(id),_obj(obj) {};
	virtual void handle();
private:
	int _id;
	FootprintSetPtr _obj;
};
*/
class FootprintSetHandler : public com::xiaonei::xce::ResultHandler{
public:
	FootprintSetHandler(const FootprintSetPtr& fsp):_fsp(fsp){};//这里用const ptr 初始化非const ptr可以编译成功？？  wzc 09.05.07
	virtual bool handle(mysqlpp::Row& row) const;
private:
	FootprintSetPtr _fsp;
};

//--------------------------------------------------------------------------
typedef map<int, FootprintSeq> FootprintMap;

struct FptnBucket {
	FootprintMap fptnMap;
	time_t time;
	bool update;
};
typedef map<string, FptnBucket> FptnBucketMap;

//---------------------------------------------------------------------------
class FootprintManagerI : public FootprintManager,
	public MyUtil::Singleton<FootprintManagerI> {
public:
	FootprintManagerI();
	virtual ~FootprintManagerI();

	/* @brief	访问用户主页并取得最新的脚印及人气数据
	 * @param	footprint	最新踩上去的脚印, 其中footprint->host是被踩的用户id
	 * @param	begin		获取用户脚印的开始点
	 * @param	limit		获取用户脚印的最大数量
	 * @return	用户最新的脚印及人气数据
	 */
	virtual FootprintInfoPtr visitHomepage(const FootprintPtr& footprint, Ice::Int begin, Ice::Int limit,
			const Ice::Current& = Ice::Current());

	/* @brief	访问用户主页(无返回值)
	 * @param	footprint       最新踩上去的脚印, 其中footprint->host是被踩的用户id
	 */
	virtual void visitHomepageOneway(const FootprintPtr& footprint, const Ice::Current& = Ice::Current());

	virtual Ice::Int getSize(Ice::Int uid, const Ice::Current& = Ice::Current());
	
	virtual FootprintInfoPtr getAll(Ice::Int uid, Ice::Int begin, Ice::Int limit,
			const Ice::Current& = Ice::Current());

	/* @brief 	增加一个脚印
	 * @param	footprint	新增的脚印
	 * @param	ref		脚印来源
	 * @param	inccount	(如果合法)是否增加人气
	 * @param	ignorelimitstatus	是否无视人气上限
	 * @param	isRetry		本次是否属于retry(第一次添加不成功的补救行为)	-- by zhanghan 09.12.11
	 * @return	是否添加成功
	 */
	bool add(const FootprintPtr& footprint, const string& ref, bool inccount = true, bool ignorelimitstatus = false, bool isRetry = false, bool ignoreError = false);

	void remove(const FootprintPtr& footprint);

	void visitHome(const FootprintPtr& footprint, const map<string, string>& params);

	virtual Ice::Int getUserVisitCount(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void setUserVisitCount(Ice::Int userId, Ice::Int count, const Ice::Current& = Ice::Current());
	virtual void incUserVisitCount(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void incUserVisitCountBatch(const MyUtil::IntSeq& userIds, Ice::Int count, const Ice::Current& = Ice::Current());

	FootprintSetPtr getFootprintSet(Ice::Int userId, bool ignoreError = false);
	string getTableName(int userId);
	bool validFootprint(const Ice::Int& footprint);
	void evictFootprint(FootprintSeq& footprints);

	void addToSaveBucket(const FootprintPtr& footprint);

private:
	UserVisitCountPtr locateUserVisitCount(int userId);
	UserVisitCountPtr findUserVisitCount(int userId);

	int _footprintCacheSize;
	int _footprintDataSize;

	int _validFootprintInterval;
	bool _visitUserFootprint;
	int _refreshDbInterval;
	size_t _saveBatchSize;

	bool _isSlave;		// 作为slave服务, 只提供和修改cache, 不修改db  -- by zhanghan 2010-3-24

	int _tableCount;
	string _tableName;
	string _serverName;

	IceUtil::Mutex _mutexAdd;
	FptnBucketMap _saveBucketAdd;
	IceUtil::Mutex _mutexRemove;
	FptnBucketMap _saveBucketRemove;

};
typedef IceUtil::Handle<FootprintManagerI> FootprintManagerIPtr;

//---------------------------------------------------------------------------
//class DummyFootprintSetI : virtual public FootprintSet {
//public:
//	virtual Ice::Int getSize(const Ice::Current& = Ice::Current()){return 0;};
//	virtual FootprintSeq getAll(Ice::Int begin, Ice::Int limit,
//	            const Ice::Current& = Ice::Current()){return FootprintSeq();};
//	virtual bool add(const FootprintPtr& ftprt,
//	            const Ice::Current& = Ice::Current()){return true;};
//};

class FootprintEqualComparator : public unary_function<FootprintPtr, bool> {
public:
	FootprintEqualComparator(FootprintTinyPtr fptt) :
		_fptt(fptt) {
	};

	bool operator()(FootprintTinyPtr fptt) const {
	    return ( (fptt->guest) && (fptt->guest == _fptt->guest) );
	}
private:
	FootprintTinyPtr _fptt;
};

struct FootprintGreaterComparator : public binary_function<FootprintPtr, FootprintPtr, bool> {
	bool operator()(const FootprintPtr& lhs, const FootprintPtr& rhs) const {
	  return true;//lhs->time > rhs->time;
	}
};


//--------------- Task 部分 ---------------------------------------------------
class AddTask : public MyUtil::Task {
public:
	AddTask(const FootprintPtr& footprint, FootprintManagerI& manager);
	virtual void handle();
private:
	FootprintPtr _footprint;
	FootprintManagerI& _manager;
};

class SaveTask : public MyUtil::Task {
public:
	SaveTask(std::string table, FootprintMap& queue);
	virtual void handle();
private:
	std::string _table;
	FootprintMap _fptnMap;
};

/* class AddFootprintRetryTask : public MyUtil::Task {
public:
	AddFootprintRetryTask(const FootprintPtr& footprint, const string& ref, bool incCount, bool ignorelimitstatus);
	virtual void handle();
private:
	FootprintPtr footprint_;
	string ref_;
	bool incCount_;
	bool ignorelimitstatus_;
}; */

// ------------ Factory 部分 ----------------------------------------------------
class UserVisitCountFactoryI : public MyUtil::ServantFactory {
public:
	Ice::ObjectPtr create(Ice::Int id);
};

class UserVisitCountResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	UserVisitCountResultHandlerI(UserVisitCountPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserVisitCountPtr& _result;
};


};
};
#endif

