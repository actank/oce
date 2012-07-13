/*
 * =====================================================================================
 *
 *       Filename:  FriendsIndex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年08月18日 13时33分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FRIENDS_INDEX_H__
#define __FRIENDS_INDEX_H__

#include <set>
#include <vector>
#include <algorithm>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <IceUtil/RWRecMutex.h>
#include "BuddySixDegrees.h"
#include <LogWrapper.h>
#include <TaskManager.h>
#include "util/cpp/TimeCost.h"

namespace xce
{
namespace buddy
{

const uint MAX_INDEX = 10000;//最多索引数
const uint MAX_DATA = 10;//分成多少组发送
const uint INDEX_CACHE_SIZE = MAX_INDEX/MAX_DATA;//每组索引数

const uint WAIT_TIME = 1;

const int BUILD_SUCESS = 0;
const int BUILD_FAIL = 1;
const int BUILD_OVERFLOW = 2;

const int TASK_LEVEL_ASYNBUILDINDEX = 10;
const int TASK_LEVEL_ASYNPUSHINDEX = 11;

/**
 *	好友数据类
 */
class BuddyDataI : virtual public BuddyData ,  public IceUtil::Mutex
{
public :
      void addFriend(int id) 
      {   
          IceUtil::Mutex::Lock lock(*this);
          this->friends.push_back(id);
      }   
 
};
typedef IceUtil::Handle<BuddyDataI>  BuddyDataIPtr;

/**
 *存储索引信息类
 */
class IndexInfoI : virtual public IndexInfo
{
 public: 
    bool operator < (IndexInfoI A )
    {
           if (id < A.id) return true;
           return false;
    }
    bool operator == (IndexInfoI A )
    {
           if (id == A.id) return true;
           return false;
    }
};
typedef IceUtil::Handle<IndexInfoI>  IndexInfoIPtr;
typedef std::vector<IndexInfoIPtr>  IndexInfoSeq;

/**
 *存储索引的数据类,每个有INDEX_CACHE_SIZE个索引
 */
class IndexData : virtual public ::Ice::Object
{
public:
	IndexData() {}
	IndexData(Ice::Int id) : _id(id) {}
	IndexData(IndexInfoSeq& seq) {indexs = seq;}

	Ice::Int getId() { return _id;}

	IndexInfoSeq indexs;
private:
	Ice::Int _id;
};
typedef IceUtil::Handle<IndexData> IndexDataPtr;   
typedef std::vector<IndexDataPtr>  IndexDataSeq;

/**
 *处理索引循环和最优关系类
 */

class FriendsIndexHelper {
public:
	bool static isLoop(int first, int second, int third) {
		return first == third ? true : false;
	}

	bool static isLoop(int first, int second, int third, int fourth) {
		return first == third || second == fourth || first == fourth ? true : false;
	}

	bool static isOptimalPath(IndexInfoPtr first, IndexInfoPtr second) {
		return first->id != second->id || (first->id == second->id && first->degree < first->degree) ? true : false;
	}
};

struct less_entry : public std::binary_function<xce::buddy::IndexInfoIPtr, xce::buddy::IndexInfoIPtr, bool> {
	bool operator()(const xce::buddy::IndexInfoIPtr x, const xce::buddy::IndexInfoIPtr y) const {
		return x->id < y->id;
	}
};

struct less_equal_entry : public std::binary_function<xce::buddy::IndexInfoIPtr, xce::buddy::IndexInfoIPtr, bool> {
	bool operator()(const xce::buddy::IndexInfoIPtr x, const xce::buddy::IndexInfoIPtr y) const {
		return x->id <= y->id;
	}
};

struct equal_entry : public std::binary_function<xce::buddy::IndexInfoIPtr, xce::buddy::IndexInfoIPtr, bool> {
        bool operator()(const xce::buddy::IndexInfoIPtr x, const xce::buddy::IndexInfoIPtr y) const {
                return (x->id == y->id);					        }
};

typedef std::set<xce::buddy::IndexInfoIPtr, less_entry> IndexInfoSet;

/**
 *异步索引数据类,作为存储索引容器，负责处理新来索引
 */
class AsynIndexData : virtual public ::Ice::Object
{
public:
	AsynIndexData() {_finish = false, _dataSum = 0;}
	AsynIndexData(Ice::Int id) : _id(id) {_finish = false, _dataSum = 0;}

	int getFreshersSize() {
		IceUtil::Mutex::Lock lock(_mutex);
		return _freshers.size();
	}
	
	IndexDataPtr getFreshersData(unsigned int i) {
		ostringstream os;
		os << "AsynIndexData, getFreshersData id = " << _id;
		MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);
		IceUtil::Mutex::Lock lock(_mutex);
		ostringstream od;
		od << "get lock, _dataSum = " << _dataSum;
		tc.step(od.str());
		if(_freshersDataSeq.size()>i) {
			IndexDataPtr result = new IndexData(_freshersDataSeq[i]->indexs);
			return result;
		}else
                {
			IndexDataPtr result = new IndexData();
			return result;
                }

		return new IndexData();
	}

	void push(IndexDataPtr freshersData) {
		ostringstream os;
		os << "AsynIndexData, push id = " << _id << " size = " << freshersData->indexs.size();
		MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);

		IceUtil::Mutex::Lock lock(_mutex);

		ostringstream od;
		od << "get lock, _dataSum = " << _dataSum;
		tc.step(od.str());
		if (_dataSum > MAX_DATA) {
			MCE_INFO("AsynIndexData, push id = " << _id << " is OVERFLOW MAXDATA");
			return;
		}

		_freshersDataSeq.push_back(freshersData);
		_dataSum ++;
	}

	void push(IndexInfoSeq& freshers) {
		ostringstream os;
		os << "AsynIndexData.pushBatch, size: " << freshers.size() ;
		MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);
		IceUtil::Mutex::Lock lock(_mutex);
		tc.step("get lock");

		for(IndexInfoSeq::iterator it = freshers.begin();
				it != freshers.end(); ++it) {
			pushFreshers(*it);
		}
	}

	bool getFinish() {
		IceUtil::RWRecMutex::RLock lock(_finishMutex);
		return _finish;
	}

	void setFinish(bool finish) {
		IceUtil::RWRecMutex::WLock lock(_finishMutex);
		_finish = finish;
	}
private:
	Ice::Int _id;
	uint _dataSum;

	IndexInfoSeq _indexs;
	IndexInfoSeq _freshers;

	IndexDataSeq _freshersDataSeq;

	bool _finish;
	IceUtil::RWRecMutex _finishMutex;
	IceUtil::Mutex _mutex;

	void pushFreshers(IndexInfoIPtr indexInfo) {

		_freshers.push_back(indexInfo);

	}
};
typedef IceUtil::Handle<AsynIndexData> AsynIndexDataPtr;   


/**
 *外部应用时建立好友索引的主类
 */
class FriendsIndex : public IceUtil::Shared {
public:

	FriendsIndex(bool useOptimalPath = true) :  _useOptimalPath(useOptimalPath) {};

	virtual ~FriendsIndex() {};

	/**
	 * 使用者需要实现的三个接口，实现Cache存储。
	 */
	virtual xce::buddy::BuddyDataPtr locateFriend(Ice::Int) = 0;
	virtual MyUtil::ObjectResultPtr locateFriends(const MyUtil::LongSeq&) = 0;
	virtual xce::buddy::BuddyDataPtr locateJoinFriend(Ice::Int) = 0;

	virtual xce::buddy::AsynIndexDataPtr locateAsynIndex(Ice::Int) = 0;

	/**
	 * 提供给应用者使用的主接口，为指定用户建立索引，存入Cache.
	 * 直接把计算放进asynBuildIndexTask返回，实现异步计算
	 * ＠param int 用户ID
	 */
	void asynBuildIndex(Ice::Int);

private:
	bool _useOptimalPath;

	int buildIndexInfoByCache(int, int, IndexInfoSeq&, int = 0);
	/**
	 *异步push索引线程,用于积攒够INDEX_CACHE_SIZE条索引，封装成一个IndexData发给计算程序.
	 */
	class asynPushIndexThread : virtual public IceUtil::Thread {
	public:
		asynPushIndexThread(Ice::Int id, FriendsIndex* builder) :
		       _id(id), _builder(builder) {}

		void pushIndexInfo(IndexInfoIPtr);
	protected:
		virtual void run();
	private:
		Ice::Int _id;
		FriendsIndex* _builder;
		IndexInfoSeq _indexCache;
		IceUtil::Monitor<IceUtil::Mutex> _mutex;

		void setFinish();
		void pushIntoData(IndexInfoSeq&);
	};
	typedef IceUtil::Handle<asynPushIndexThread> asynPushIndexThreadPtr;

	/**
	 *异步建立索引线程,发送给asynPushIndexThread积攒
	 */
	class asynBuildIndexThread : virtual public IceUtil::Thread {
	public:
		asynBuildIndexThread(Ice::Int id, FriendsIndex* builder, asynPushIndexThreadPtr pushThread) :
		       _id(id), _builder(builder), _pushThread(pushThread), _indexSum(0) {}
	protected:
		virtual void run();
	private:
		Ice::Int _id;
		FriendsIndex* _builder;
		asynPushIndexThreadPtr _pushThread;
		uint _indexSum;

		int asynBuildIndexInfo(int, int, int = 0);
	};
	typedef IceUtil::Handle<asynBuildIndexThread> asynBuildIndexThreadPtr;

	/**
	 *建立索引Task,实现异步计算。
	 */
	class asynBuildIndexTask : virtual public MyUtil::Task {
	public:
		asynBuildIndexTask(Ice::Int id, FriendsIndex* builder) :
			MyUtil::Task(TASK_LEVEL_ASYNBUILDINDEX), _id(id), _builder(builder) {}
	protected:
		virtual void handle();
	private:
		Ice::Int _id;
		FriendsIndex* _builder;

		void setFinish();
	};

	/**
	 *往应用者Cache里push索引Task,把流水线上产生的新索引放进AsynIndexData，等待计算程序来取
	 */
	class asynPushIndexTask : virtual public MyUtil::Task {
	public:
		asynPushIndexTask(Ice::Int id, IndexDataPtr data, FriendsIndex* builder) :
			MyUtil::Task(TASK_LEVEL_ASYNPUSHINDEX), _id(id), _data(data), _builder(builder) {}
	protected:
		virtual void handle();
	private:
		Ice::Int _id;
		IndexDataPtr _data;
		FriendsIndex* _builder;
	};
};
typedef IceUtil::Handle<FriendsIndex> FriendsIndexPtr;

}
}
#endif
