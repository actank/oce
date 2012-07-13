#ifndef __BULLETIN_MANAGER_I_H__
#define __BULLETIN_MANAGER_I_H__

#include <string>
#include <limits>
#include <climits>
#include <bitset>
#include "Bulletin.h"
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/Singleton.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"

namespace xce {
namespace blt {


// --------------- 索引相关 -------------------------
struct BulletinList : virtual public Ice::Object {

	BulletinList():minBulletinId(0) {
	}

	map<int, BulletinPtr> listMap;
	int minBulletinId;
};

typedef IceUtil::Handle<BulletinList>  BulletinListPtr;

//added by shangmin to represent a set
class BulletinInterest : virtual public Ice::Object {
public:
	BulletinInterest() {
		table_ = "";

	}

	BulletinInterest(const std::bitset<INT_MAX>& array, const string& table) 
		: table_(table),array_(array) {
	}

	bool isSet(int pos) {
		return array_.test(pos);
	}

	void set(int pos) {
		array_.set(pos);
	}

	void clear(int pos) {
		array_.reset(pos);
	}

	string getTableName() const {
		return table_;
	}

	void setTableName(const string& table) {		
		table_ = table;
	}
private:
	string table_;
	bitset<INT_MAX> array_;
};

typedef IceUtil::Handle<BulletinInterest> BulletinInterestPtr;

/* 公告索引类 */
class BulletinIndex : virtual public Ice::Object {
public:
	/* @breif 构造索引 */
	BulletinIndex(const BulletinListPtr bulletins);

	/* @brief 按用户id进行搜索 */
	BulletinSeq search(int userid, int website);

	/* @brief 按公告id获取id */
	BulletinPtr get(int bulletinid);

	/* @brief 判断和另一索引是否相同(仅比较公告id) */
	bool compare(const IceUtil::Handle<BulletinIndex> index2);

	/* @brief 返回正排索引 */
	map<int, BulletinPtr>& getForwardIndex();
private:
	BulletinListPtr bulletins_;
	map<int, BulletinPtr> forwardIndex_;		// 正排索引
	map<int, map<int, vector<int> > > invertedIndex_;	// 倒排索引
	bool withApp_;
};
typedef IceUtil::Handle<BulletinIndex> BulletinIndexPtr;

/* 用户信息类 */
struct UserInfo : virtual public Ice::Object {
	int userid;
	int star;
	int vip;
	int gender;
	int idmod;
	int age;
	int complete;
	int stage;	// 用户stage -- by zhanghan 2010-04-20
	bool error;	// 标志是否有错
};
typedef IceUtil::Handle<UserInfo> UserInfoPtr;

struct UserApp : virtual public Ice::Object {
	std::set<int> apps;
	bool error;
};
typedef IceUtil::Handle<UserApp> UserAppPtr;

struct UserComplete : virtual public Ice::Object {
	std::set<int> completes;
	bool error;
};
typedef IceUtil::Handle<UserComplete> UserCompletePtr;

/* 用户click记录类  */
class UserAction: virtual public Ice::Object {
public:	
	UserAction(int userid):
       		userid_(userid)	{
	};
	virtual ~UserAction() {};

	int add_count(int bulletinId, int maxShow=-1);

	/* brief	记录用户点击了一个公告
	 * param	bulletinId	被点击的公告
	 */
	void click(int bulletinId);

	/* brief	查找是否在已点击过的公告中
	 * param	bulletinId	待查找的公告id
	 * return	true则为已有;false则未有
	 */
	bool denied(int bulletinId);

	void add(int bulletinid, int count);
private:
	IceUtil::Mutex mutex_;
	int userid_;
	//std::vector<int> clickedIds_;
	std::map<int, int> actions_;
};
typedef IceUtil::Handle<UserAction> UserActionPtr;

/* 重建索引线程 */
class CreateIndexThread : virtual public IceUtil::Thread {
protected:
	virtual void run();
private:
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};


/* 公告主类 */
class BulletinManagerI : virtual public BulletinManager,
	public MyUtil::Singleton<BulletinManagerI>
{
public:
    virtual ::xce::blt::BulletinPtr show(::Ice::Int userid, ::Ice::Int website, const Ice::Current&); // 旧接口

    virtual ::xce::blt::BulletinPtr get(::Ice::Int bulletinid, const Ice::Current&); // 旧接口

    virtual void renew(bool rebuildIndex, const Ice::Current&crt=Ice::Current());

    virtual void relocateUserCache(::Ice::Int userid, const Ice::Current&);

    virtual ::xce::blt::BulletinSeq list(const Ice::Current&);	// 旧接口

    virtual void click(::Ice::Int userid, ::Ice::Int bulletinId, bool beClose, const Ice::Current&);

    virtual void reloadBulletinInterest(int bltId, bool force, const Ice::Current&);

    void initialize();

    /* @brief  仅从现有公告重建索引(不读数据库, 只按上线时间更新)
     */
    void renewIndexOnly();

    int getMinBulletinId() {
	    return minBulletinId_;
    }

    void reloadBulletinInterest(BulletinListPtr bulletins, bool force);

    bool isInterested(int bltId, int userid) {
	    BulletinInterestPtr interest = getBltInterest(bltId);
	    if ( ! interest ) {
		    // MCE_INFO("[BulletinManagerI::isInterested] InterestUser of " << bltId << " is NULL");
		    return true;
	    } else {
		    bool isInterest =  interest->isSet(userid);
		    if ( isInterest ) {
			    MCE_INFO("[BulletinManagerI::isInterested] " << userid << " is InterestUser of " << bltId << "?:" << isInterest);
		    }
		    return isInterest;
	    }
    }

	
private:
    friend class MyUtil::Singleton<BulletinManagerI>;
    BulletinManagerI() {
    };
    
    /* @biref 从DB中load一组指定用户id	 
     */	
    BulletinInterestPtr loadBulletinInterest(const string& tableName);
    string getInterestTable(const BulletinPtr& blt);

    void UpdateClickInDB(int userId, int bulletinId);

    BulletinInterestPtr getBltInterest(int bltId);    

    void setBltInterest(int bltId, const BulletinInterestPtr& interest);

    void eraseBltInterest(int bltId);

    UserActionPtr locateAction(Ice::Int uid);

    /* @brief 当前公告列表 */
    BulletinListPtr bulletins_;

    /* @brief 当前索引 */
    BulletinIndexPtr index_;

    /* @brief 当前列表中所有的公告中的最小id */
    int minBulletinId_;

    /* @brief 从DB读取公告列表
     */
    BulletinListPtr loadBulletins();

    /* @brief 由公告列表创建索引
     * @return	创建好的索引
     */
    BulletinIndexPtr createIndex(const BulletinListPtr& bulletins);

    /* @brief 定时重建索引线程 */
    CreateIndexThread createThread_;

    /* @brief 用于锁替换索引 */
    IceUtil::Mutex mutexRenew_;

    //the following lines were added by shangmin
    map<int, BulletinInterestPtr> bltInterest;
};

// ------------ Factory部分 ------------------------
/* 生成UserInfo内容的Factory */
class UserInfoFactory : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int userId);
};

/* 生成UserApp内容的Factory */
class UserAppFactory : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int userId);
};

/* 生成UserComplete内容的Factory */
class UserCompleteFactory : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int userId);
};

/* 生成UserAction内容的Factory */
class UserActionFactory : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int userId);
};


// ---------- ResultHandler部分 ------------------------
/* 读取公告指定用户列表的ResultHandler */
class BulletinInterestResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	BulletinInterestResultHandler(vector<int>& userIds) : 
		_userIds(userIds) {
		}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	vector<int>& _userIds;
	string _colName;
};

/* 读取公告内容的ResultHandler */
class BulletinHandler : public com::xiaonei::xce::ResultHandler {
public:
	BulletinHandler(map<int, BulletinPtr>& result) :
		result_(result) {
	};
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, BulletinPtr>& result_;
};

/* 读取用户app信息的ResultHandler */
class UserAppResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserAppResultHandler(set<int>& apps):
		apps_(apps) {
	}
protected:
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::set<int>& apps_;

};


/* 读取用户点击信息的ResultHandler */
class UserActionResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserActionResultHandler(UserActionPtr& useraction):
		useraction_(useraction) {
	};
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	UserActionPtr useraction_;

};

}
}

#endif
