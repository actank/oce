#ifndef __I_LIKE_IT_CACHE_I_H__
#define __I_LIKE_IT_CACHE_I_H__

#include <ILikeIt.h>
#include <Ice/Ice.h>
#include "UtilCxx/src/TaskManager.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "app/ILikeIt/share/ILikeItUtil.h"
#include "OceCxxAdapter/src/ILikeItCacheAdapter.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"

namespace xce {
namespace ilikeit {

const int ILIKEIT_CACHE = 0;
const int LOAD_BATCH_SIZE = 100;
const int SYNC_INTERVAL = 1;

typedef std::map<long, LikeDataPtr> LikeDataMap;

// ================ Thread部分 ============================
class SyncCacheThread : virtual public IceUtil::Thread {
public:
	void initialize(int mod, const ILikeItCachePrx& localPrx) {
		_mod = mod;
		_localPrx = localPrx;
	};
	void invoke(long gidLong, const LikeDataPtr& likeData);
protected:
	virtual void run();
private:
	int _mod;
	ILikeItCachePrx _localPrx;
	std::map<long, Ice::ObjectPtr> _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};
typedef IceUtil::Handle<SyncCacheThread> SyncCacheThreadPtr;

// ============== ILikeItCacheI主类 =============================
class ILikeItCacheI : virtual public ILikeItCache,
		public MyUtil::Singleton<ILikeItCacheI>  {
public:
	virtual ::Ice::Int getLikeCount(const GlobalIdPtr& gid, const Ice::Current& ctr=Ice::Current());

    	virtual ::MyUtil::IntSeq getLikeCache(const GlobalIdPtr& gid, const Ice::Current& ctr=Ice::Current());

	/* @brief 	获取按照查看的用户进行个性排序的喜欢过一个对象的用户列表
	 * @param	gid		对象的GlobalId
	 * @param	uid		查看该对象的用户id
	 * @param	limit		显示详情的用户数量
	 * @param	showStrangers	是否显示陌生人(非自己及好友)的详情
	 * @return	符合条件的用户资料信息包
	 */
    	virtual LikeInfoPtr getLikeDetail(const GlobalIdPtr& gid, ::Ice::Int uid, ::Ice::Int limit, bool showStrangers, const Ice::Current& ctr=Ice::Current());
    	virtual LikeInfo2Ptr getLikeDetail2(const GlobalIdPtr& gid, ::Ice::Int uid, ::Ice::Int limit, bool showStrangers, const Ice::Current& ctr=Ice::Current());

	/* @brief	批量获取按照查看的用户进行个性排序的顶过一组对象的用户列表
	 * @param	gids		一组对象的gids列表
	 * @param	uid		查看该对象的用户id
	 * @param	limit		显示详情的用户数量
	 * @param	detailNum	按照limit数量显示用户资料的对象(gid)的条数(其他的无论limit是多少均不显示用户详情)
	 * @return      符合条件的用户资料信息包列表,排列顺序和gids相同
	 */
    	virtual LikeInfoSeq getLikeDetailBatch(const GlobalIdSeq& gids, ::Ice::Int uid, ::Ice::Int limit, ::Ice::Int detailNum, const Ice::Current& ctr=Ice::Current());
    	virtual LikeInfo2Seq getLikeDetailBatch2(const GlobalIdSeq& gids, ::Ice::Int uid, ::Ice::Int limit, ::Ice::Int detailNum, const Ice::Current& ctr=Ice::Current());

	/* @brief 	获取喜欢一个对象的用户总数和用户id列表(有最大长度限制)
	 * @param	gid		对象的GlobalId
	 * @param	limit		列表最多显示的用户数量
	 * @return	符合条件的用户总数和用户列表	 */
	virtual LikeInfoSimplePtr getLikeInfoSimple(const GlobalIdPtr& gid, ::Ice::Int limit, const Ice::Current& ctr=Ice::Current());

	/* @brief 	批量获取喜欢一组对象的用户总数和用户id列表(有最大长度限制)
	 * @param	gids		一组对象的GlobalId
	 * @param	limit		列表最多显示的用户数量
	 * @return	相应的符合条件的用户总数和用户列表组，和gid在数组中的位置对应	 */
	virtual LikeInfoSimpleSeq getLikeInfoSimpleBatch(const GlobalIdSeq& gids, int limit, const Ice::Current& ctr=Ice::Current());

	/* @brief	一个用户是否like过一个对象
	 * @param	gid		对象的GlobalId
	 * @param	uid		用户id
	 * @return	1-该用户like过该对象; 0-没有 */
	virtual int isLiked(const GlobalIdPtr& gid, int uid, const Ice::Current& ctr=Ice::Current());

	/* @brief	一个用户是否like过一组对象
	 * @param	gids		这组对象的GlobalId
	 * @param	uid		用户id
	 * @return	该用户依次是否like过这组对象 */
	virtual MyUtil::IntSeq isLikedBatch(const GlobalIdSeq& gids, int uid, const Ice::Current& ctr=Ice::Current());

	virtual void load(const GlobalIdSeq& gids, const Ice::Current& ctr=Ice::Current());

	virtual void setData(const ::MyUtil::ObjectResultPtr& objects, const Ice::Current& ctr=Ice::Current());

	virtual void addLikeship(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());
	
	virtual void addLikeshipValid(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());
	
	virtual void addLikeshipInValid(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());
	
	virtual void removeLikeship(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());
	
	virtual void removeLikeshipValid(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());
	
	virtual void removeLikeshipInValid(const LikeshipPtr& ships, const Ice::Current& ctr=Ice::Current());

	virtual void setValid(bool newState, const Ice::Current& ctr=Ice::Current());

	virtual bool isValid(const Ice::Current& ctr=Ice::Current());

	void bootstrap(int cluster, int mod, int syncCacheNum, Ice::ObjectPrx localPrx);

	/* @breif	从DB按需加载后是否需要通知其他份的冗余cache
	 */
	bool syncCache();

	void invokeSync(long gidLong, const LikeDataPtr& likeData) {
		_syncThread.invoke(gidLong, likeData);
	}

private:
	friend class MyUtil::Singleton<ILikeItCacheI>;
	ILikeItCacheI() {
	};

	LikeDataPtr locateLikeDataWithoutDB(const GlobalIdPtr& gid); 
	//LikeDataMap locateLikeDataBatchWithoutDB(const GlobalIdSeq& gids);

	LikeDataPtr findLikeData(const GlobalIdPtr& gid); 
	LikeDataMap findLikeDataBatch(const GlobalIdSeq& gids);

	LikeDataPtr locateLikeData(const GlobalIdPtr& gid);

	bool belongsMe(const GlobalIdPtr& gid);
	bool getFriendList(vector<int>& friends, int uid);
	void getCountAndSort(vector<int>& usersSort, int& selfCount, int& friendCount, const vector<int>& users, int selfId, const vector<int>& friendList, bool withFriends, bool showStrangers, int limit);
	void getUserDetail(UserDetailSeq& details, const vector<int>& uids);
	void getUserDetail2(UserDetail2Seq& details, const vector<int>& uids);

	int _cluster;
	int _mod;
	bool _syncCache;
	ILikeItCachePrx _localPrx;	// 服务本身的proxy
	::Ice::Identity _localIdentity;	// 本地proxy的identity

	IceUtil::RWRecMutex _validMutex;	// 预加载完成与否的锁
	bool _valid;	// 预加载是否完成标志

	SyncCacheThread _syncThread;	// 同步cache的thread
};


// ============== Factory部分 ================================
class ILikeItCacheFactoryI: public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long gidLong);
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& gidLongs);
};

// ================ ResultHandle部分 ================================
// -------------- SingleLikeshipHandler ----------------------------
class SingleLikeshipHandler: public com::xiaonei::xce::ResultHandler {
public:
	SingleLikeshipHandler(LikeDataPtr& result) :
			_result(result) {
	};
	virtual bool handle(mysqlpp::Row& row) const;
private:
	LikeDataPtr& _result;
};

// -------------- BatchLikeshipHandler ----------------------------
class BatchLikeshipHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchLikeshipHandler(map<long, LikeDataPtr>& result) :
			_result(result) {
	};
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<long, LikeDataPtr>& _result;
};

};
};

#endif
