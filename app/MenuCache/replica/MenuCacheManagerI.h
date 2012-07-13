#ifndef __MenuCacheManagerI_h__
#define __MenuCacheManagerI_h__

#include <Ice/Ice.h>
#include "MenuCache.h"
#include "RecentMenuLogicManagerAdapter.h"

#include "FavoriteMenuLogicManagerAdapter.h"
#include "ObjectCacheI.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {

namespace menu {

static const int RECMENU_CACHE_DATA = 0;

static const int FAVMENU_CACHE_DATA = 1;

static const string FIELD_UID = "user_id";

static const string FIELD_RECLIST = "app_ids";

static const string FIELD_FAVLIST = "app_ids";

static const size_t TABLE_NUM = 100; 

static const size_t LOAD_BATCH_SIZE = 100;

static const string REC_TABLE_NAME = "recent_menu_";
static const string FAV_TABLE_NAME = "favorite_menu_";

static const string DB_DESC_RECPATTERN = "menucache_recent_menu_";
static const string DB_DESC_FAVPATTERN = "menucache_favorite_menu_";

static const string DB_DESC_FAVMENUCACHE = "menucache_favorite_menu"; //数据源
static const string DB_DESC_RECMENUCACHE = "menucache_recent_menu"; //数据源

/*****************************************************************************/
class MenuCacheData: virtual public MenuCacheRecord {
public:
	MenuCacheData() {
	}
	MenuCacheData(const MenuCacheRecordPtr& record);
	AppIdSeq getMenucontents();
	void setMenucontents(AppIdSeq menucontents);

private:
	IceUtil::RWRecMutex _menucontentsMutex;
};

typedef IceUtil::Handle<MenuCacheData> MenuCacheDataPtr;


/*****************************************************************************/
class SyncFavCacheThread : virtual public IceUtil::Thread {
public:
	void initialize(int mod, const MenuCacheManagerPrx& locatePrx) {
		 mod_=mod;
		locatePrx_=locatePrx;
	}
	void invoke(int id, const MenuCacheDataPtr& data);
protected:
	virtual void run();
private:
	int mod_;
	MenuCacheManagerPrx locatePrx_;
	std::map<long, Ice::ObjectPtr> cache_;
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	
};
/*****************************************************************************/
class SyncRecCacheThread : virtual public IceUtil::Thread {
public:
	void initialize(int mod, const MenuCacheManagerPrx& locatePrx) {
		 mod_=mod;
		locatePrx_=locatePrx;
	}
	void invoke(int id, const MenuCacheDataPtr& data);
protected:
	virtual void run();
private:
	int mod_;
	MenuCacheManagerPrx locatePrx_;
	std::map<long, Ice::ObjectPtr> cache_;
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
};
/*****************************************************************************/
class MenuCacheManagerI: virtual public MenuCacheManager, public MyUtil::Singleton<MenuCacheManagerI> {
public:

	//类型数组typeArray，0返回ALL数组，1返回REC数组，2返回FAV数组
	virtual	CacheType2AppIdSeqMap get(::Ice::Int,const CacheTypeSeq&, const Ice::Current&);

	virtual CacheType2AppIdSeqMap getAll(::Ice::Int, const Ice::Current&);
		
	virtual	void set(::Ice::Int, CacheType, const AppIdSeq&, const Ice::Current&);		

	virtual void setBatch(const MenuCacheRecordSeq& records, const Ice::Current&);
	      
	virtual	void load(const UserIdSeq&, const Ice::Current&);
	
	virtual bool isValid(const Ice::Current& = Ice::Current());
	
	virtual void setValid(bool newState, const Ice::Current& = Ice::Current());	
	virtual void setData(const MyUtil::ObjectResultPtr& objects, CacheType type, 
			const Ice::Current& = Ice::Current());
	
	void initialize(int cluster, int mod, int syncCacheNum, Ice::ObjectPrx locatePrx);	
	
	bool syncCache();
	
	void invokeRecSync(int userId, const MenuCacheDataPtr data){
		syncRecThread_.invoke(userId,data);
	}

	void invokeFavSync(int userId, const MenuCacheDataPtr data){
		syncFavThread_.invoke(userId,data);
	}

private:
	bool _isValid;

	IceUtil::RWRecMutex _validMutex;

	int mod_;	// 自身服务所属的mod
	int cluster_;	// 服务被散的份数
	bool syncCache_;
	
	MenuCacheManagerPrx locatePrx_;
	::Ice::Identity locateIdentity_;
	
	SyncFavCacheThread syncFavThread_;

	SyncRecCacheThread syncRecThread_;

	//::com::xiaonei::service::RecentMenuLogicManagerAdapter _recentMenuAda;			/* @brief RecentMenuLogicManager的代理 */
	
	//::com::xiaonei::service::FavoriteMenuLogicManagerAdapterPtr _favoriteMenuAda;			/* @brief FavoriteMenuLogicManager的代理 */

	friend class MyUtil::Singleton<MenuCacheManagerI>;

	MenuCacheManagerI();
	MenuCacheDataPtr findRecentMenuCacheData(Ice::Int userId);
	MenuCacheDataPtr findFavoriteMenuCacheData(Ice::Int userId);
	MenuCacheDataPtr getRecentMenuCacheData(Ice::Int userId);
	MenuCacheDataPtr getFavoriteMenuCacheData(Ice::Int userId);
	
	bool belongsMe(int userid);
};

/*****************************************************************************/

class RecentMenuCacheDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual MyUtil::ObjectResultPtr create(MyUtil::IntSeq& ids);
};
/*****************************************************************************/
class FavoriteMenuCacheDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual MyUtil::ObjectResultPtr create(MyUtil::IntSeq& ids);
};
/*****************************************************************************/
class BatchRecentMenuHandler : public com::xiaonei::xce::ResultHandler {
public:	
	BatchRecentMenuHandler(map<int, vector<int> >& result):result_(result){
	};
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, vector<int> >& result_;
};

/*****************************************************************************/
class BatchFavoriteMenuHandler : public com::xiaonei::xce::ResultHandler {
public:	
	BatchFavoriteMenuHandler(map<int, vector<int> >& result):result_(result){
	};
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, vector<int> >& result_;
};

//*****************************************************************************/
class RecentMenuCacheDataCreateTask : public MyUtil::Task {
public:	
	RecentMenuCacheDataCreateTask(int userId):userId_(userId){
	};
	virtual void handle();
private:
	int userId_;
};

/*****************************************************************************/
class FavoriteMenuCacheDataCreateTask : public MyUtil::Task {
public:
	FavoriteMenuCacheDataCreateTask(int userId):userId_(userId){	
	};
	virtual void handle();
private:
	int userId_;
};

/*****************************************************************************/
class RecentMenuCacheDataFactoryCreateTask : public MyUtil::Task {
public:
	RecentMenuCacheDataFactoryCreateTask(std::vector<int>& uids):uids_(uids){
	};
	virtual void handle();
private:
	std::vector<int>& uids_;
};
/*****************************************************************************/
class FavoriteMenuCacheDataFactoryCreateTask : public MyUtil::Task {
public:
	FavoriteMenuCacheDataFactoryCreateTask(std::vector<int>& uids):uids_(uids){
	};
	virtual void handle();
private:
	std::vector<int>& uids_;
};
/*****************************************************************************/
//class IntSeqSerialize {
//public:
//        static std::string serialize(const AppIdSeq& seq) {
//                std::string result;
//		result.resize(seq.size()*sizeof(int));
//                memcpy((void*)(result.data()), &seq[0], seq.size()*sizeof(int));
//		ostringstream oss;
//		for(vector<int>::const_iterator it=seq.begin();it!=seq.end();++it)
//			oss<<*it<<" ";
//		MCE_DEBUG(oss.str()<<"serialize to string:"<<result<<"; size:"<<result.size());
//                return result;
//        }
//
//        static AppIdSeq unserialize(const std::string& str) {
//                AppIdSeq result(str.size()/sizeof(int));
//                memcpy((void*)(&result[0]), str.data(), str.size());
//		ostringstream oss;
//		for(vector<int>::const_iterator it=result.begin();it!=result.end();++it){
//			oss<<*it<<" ";
//		}
//		MCE_DEBUG(str<<" unserialize to array:"<<oss.str()<<"; size:"<<result.size());
//                return result;
//        }
//};
//
}
}

#endif
