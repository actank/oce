#ifndef __BUDDY_SIXDEGREES_CACHE_I_H__
#define __BUDDY_SIXDEGREES_CACHE_I_H__

#include "BuddySixDegrees.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"
#include <QueryRunner.h>
#include "ServiceI.h"
#include "../friendindex/FriendsIndex.h"
#include "CompareFriends.h"
#include "StartStopFlag.h"
#include <tcrdb.h>
#include <iostream>
#include <set>



#if __GNUC__>2
#include <ext/hash_set>
using namespace __gnu_cxx;
#else
#include <hash_set>
using namespace stdext;
#endif

namespace xce {
namespace buddy {

const int BUDDY_CACHE_DATA=0;//好友
const int JOIN_CACHE_DATA=2;//节点用户
const int ASYN_INDEX_CACHE_DATA=3;//异步索引
const int ROUT_CACHE_DATA=4;//计算结果

const int TASK_LEVEL_CALCULATE_ROUT = 1;

/*****************************************************************************/
/**
 * 存储host到guest路径类
 */
class HostGuestRout :  virtual public ::Ice::Object , public IceUtil::Mutex
{
public:
    bool  end; 
    IntSeqSeq routList_;

    IntSeqSeq& getRoutList()
    {
        IceUtil::Mutex::Lock lock(*this);
        return routList_;
    }
    HostGuestRout()
    {
        end = false;
    }
  
    void setEnd(bool flag)
    {
        end = flag;
    }
 
    bool getEnd()
    {
        return end;
    } 
    void add(std::vector<int> rout)
    {
        IceUtil::Mutex::Lock lock(*this);
        routList_.push_back(rout);
    }

    void add(IntSeqSeq& routList)
    {
        IceUtil::Mutex::Lock lock(*this);
        routList_.insert(routList_.end(),routList.begin(),routList.end());
    }  

    int getSize()
    {
	IceUtil::Mutex::Lock lock(*this);
	return routList_.size();
    }
     
};
typedef IceUtil::Handle<HostGuestRout>  HostGuestRoutPtr;

/*****************************************************************************/
/**
 * 应用FriendsIndex类建立对节点用户的索引
 */
class FriendsIndexImpl : public xce::buddy::FriendsIndex { 
public: 
 
        FriendsIndexImpl() : xce::buddy::FriendsIndex() { 
        }; 
 
        virtual ~FriendsIndexImpl() {} 

 		/**
		 * 实现取好友接口
		 */
        virtual xce::buddy::BuddyDataPtr locateFriend(Ice::Int id) { 
                BuddyDataPtr obj = MyUtil::ServiceI::instance().findObject<BuddyDataPtr> ( BUDDY_CACHE_DATA , id); 
                return obj; 
       } 
 
 		/**
		 * 实现批量取好友接口
		 */
        virtual MyUtil::ObjectResultPtr locateFriends(const MyUtil::LongSeq& ids) { 
               return MyUtil::ServiceI::instance().getObjectCache()->findObjects(BUDDY_CACHE_DATA, ids); 
        } 
 
 		/**
		 * 实现取异步索引数据接口
		 */
       virtual xce::buddy::AsynIndexDataPtr locateAsynIndex(Ice::Int id) 
       { 
               AsynIndexDataPtr obj = MyUtil::ServiceI::instance().findObject<AsynIndexDataPtr> (  ASYN_INDEX_CACHE_DATA , id); 
 
               if (!obj) { 
                        obj = new AsynIndexData(id); 
                        MyUtil::ServiceI::instance().addObject(obj, ASYN_INDEX_CACHE_DATA , id); 
               } 
               return obj; 
       } 
 
	   /**
		* 实现取节点用户接口
		*/
       virtual xce::buddy::BuddyDataPtr locateJoinFriend(Ice::Int id ) 
       { 
               BuddyDataPtr obj = MyUtil::ServiceI::instance().findObject<BuddyDataPtr> ( JOIN_CACHE_DATA , id); 
               return obj; 
       } 
}; 


/*****************************************************************************/
/**
 * 从TokyoTyrant加载数据线程
 */
class AddFriendsThread : virtual public IceUtil::Thread {
public:
        AddFriendsThread( string host , int port , hash_set<int>&  ids, IceUtil::RWRecMutex& mutex) : host_(host), port_(port), ids_(ids), mutex_(mutex){}
   
protected:
        virtual void run();
private:
        string host_;
        int port_;
	hash_set<int>&  ids_;
	IceUtil::RWRecMutex& mutex_;
};

typedef IceUtil::Handle<AddFriendsThread>  AddFriendsThreadPtr;


/*****************************************************************************/

/**
 * 计算六度路径线程
 */
class CalculateRoutThread : virtual public IceUtil::Thread {
public:
	CalculateRoutThread(Ice::Int host, Ice::Int guest) :
 	      _host(host), _guest(guest) {}

	void pushIndexSeq(IndexInfoSeq&, IndexInfoSeq&);
	void pushIndexSeq(IndexDataPtr, IndexDataPtr);
protected:
	virtual void run();
private:
	Ice::Int _host;
	Ice::Int _guest;

    vector<IndexInfoIPtr>  _allFriendsA ;
    vector<IndexInfoIPtr>  _allFriendsB ; 

    IndexDataSeq _newFriendsA;
    IndexDataSeq _newFriendsB;

	IceUtil::Monitor<IceUtil::Mutex> _mutex;

	void calculateRout(vector<IndexInfoIPtr>, vector<IndexInfoIPtr>);
	HostGuestRoutPtr getHostGuestRout();
	bool isEnd();

	/**
	 * 负责把最新索引合并和按最优路径筛选类
	 */
	class AddNewFriendsThread : virtual public IceUtil::Thread {
	public :
		AddNewFriendsThread(Ice::Int id, vector<IndexInfoIPtr>& newFriends, vector<IndexInfoIPtr>& allFriends) : 
			_id(id), _newFriends(newFriends), _allFriends(allFriends){}
	protected:
		virtual void run();
	private:
		Ice::Int _id;
		vector<IndexInfoIPtr>& _newFriends;
		vector<IndexInfoIPtr>& _allFriends;
	};
	typedef IceUtil::Handle<AddNewFriendsThread>  AddNewFriendsThreadPtr;
};
typedef IceUtil::Handle<CalculateRoutThread>  CalculateRoutThreadPtr;


/*****************************************************************************/

/**
 *计算六度路径Task类,负责计算一度和二度好友关系，并把索引从FrinedIndex取来送给CalculateRoutThread进行大于二度的关系路径计算
 */
class CalculateRoutTask : virtual public MyUtil::Task {
public:
       CalculateRoutTask(int host, int guest,
           ::Ice::Int level=TASK_LEVEL_CALCULATE_ROUT) :MyUtil::Task(level)
       {
          host_ = host ;   
          guest_ = guest ;
       }
protected:
        virtual void handle();
private:
        int host_;
        int guest_;
		IceUtil::Monitor<IceUtil::Mutex> mutex_;

};

typedef IceUtil::Handle<CalculateRoutTask>  GetIndexThreadPtr;

/*****************************************************************************/
/**
 * 外部调用主类
 */
class BuddySixDegreesManagerI: virtual public BuddySixDegreesManager, virtual public MyUtil::Singleton<BuddySixDegreesManagerI> {

public:
	BuddySixDegreesManagerI() {
		friendsIndex_ = new FriendsIndexImpl();
	}
	virtual ~BuddySixDegreesManagerI() {}

	/**
	 *调用计算开始
	 *@param host 主人ID
	 *@param guest 要认识人ID
	 */
    bool computeTaskStart(int host, int guest , const Ice::Current& =  Ice::Current());

	/**
	 *调用计算结束
	 *@param host 主人ID
	 *@param guest 要认识人ID
	 */
    bool computeTaskStop(int host, int guest , const Ice::Current& =  Ice::Current());

	/**
	 *获取六度路径
	 *@param host 主人ID
	 *@param guest 要认识人ID
	 */
    RouteListPtr getRouteList(int host, int guest, int begin, int limit,  const Ice::Current& =  Ice::Current());

	/**
	 *旧接口
	 *@param host 主人ID
	 *@param guest 要认识人ID
	 *@return  返回好友列表
	 */
    MyUtil::IntSeq getFriendList(int hostId, int limit, const Ice::Current&);

	/**
	 *@param host 主人ID
	 *@param guest 要认识人ID
	 *@return 返回空路径
	 */
    MyUtil::IntSeqSeq checkDegreesFriends(int id1, int id2,  const Ice::Current& =  Ice::Current());

	/**
	 * 服务启动加载TT接口
	 */
    void startThread(const Ice::Current& = Ice::Current() );

	virtual bool isValid(const Ice::Current&);
	virtual void setValid(bool valid,const Ice::Current& = Ice::Current());

private:
    int count;
	bool _isValid;
	FriendsIndexPtr friendsIndex_;
    IceUtil::RWRecMutex _setMutex; 
    IceUtil::RWRecMutex _validMutex;

	hash_set<int>  mastIds_;

	/**
	 * 启动10个线程加载对应每个TT存储
	 */
	void startAddFriends() {
		vector<IceUtil::ThreadPtr> threads;
  
    	for(int i=1980; i<1980+10;i++){
	   		threads.push_back(new AddFriendsThread("127.0.0.1",i, mastIds_, _setMutex ));
       }

		for (vector<IceUtil::ThreadPtr>::iterator itp = threads.begin(); itp != threads.end(); ++itp) {
		   (*itp)->start(1024*1024).detach();
	   }
	}
};

/*****************************************************************************/

class BuddyCacheFactory : virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

class JoinCacheFactory : virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

class AsynIndexCacheFactory : virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

class RoutCacheFactory : virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

//***************************************************************************


}
}

#endif

