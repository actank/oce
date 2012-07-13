#ifndef __TIPS_MANAGER_I_H__
#define __TIPS_MANAGER_I_H__

#include <string>
#include <limits>
#include <climits>
#include <bitset>
#include "Tips.h"
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/Singleton.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"

namespace xce {
namespace tps {


// --------------- 索引相关 -------------------------
struct TipsList : virtual public Ice::Object {

	TipsList():minTipsId(0) {
	}

	map<int, TipsPtr> listMap;
	int minTipsId;
};

typedef IceUtil::Handle<TipsList>  TipsListPtr;

class TipsInterest : virtual public Ice::Object {
public:
	TipsInterest() {
		table_ = "";

	}

	TipsInterest(const std::bitset<INT_MAX>& array, const string& table) 
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

typedef IceUtil::Handle<TipsInterest> TipsInterestPtr;

/* 提示索引类 */
class TipsIndex : virtual public Ice::Object {
public:
	/* @breif 构造索引 */
	TipsIndex(const TipsListPtr tipss);

	/* @brief 按用户id进行搜索 */
	TipsSeq search(int userid, int website);

	/* @brief 按提示id获取id */
	TipsPtr get(int tipsid);

	/* @brief 判断和另一索引是否相同(仅比较提示id) */
	bool compare(const IceUtil::Handle<TipsIndex> index2);

	/* @brief 返回正排索引 */
	map<int, TipsPtr>& getForwardIndex();
private:
	TipsListPtr tipss_;
	map<int, TipsPtr> forwardIndex_;		// 正排索引
	map<int, map<int, vector<int> > > invertedIndex_;	// 倒排索引
	bool withApp_;
};
typedef IceUtil::Handle<TipsIndex> TipsIndexPtr;

/* 用户信息类 */
struct UserInfo : virtual public Ice::Object {
	int userid;
	int star;
	int vip;
	int gender;
	int idmod;
	int age;
	int complete;
	int stage;	// 用户stage
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

	int add_count(int tipsId, int maxShow=-1);

	/* brief	记录用户点击了一个提示
	 * param	tipsId	被点击的提示
	 */
	void click(int tipsId);

	/* brief	查找是否在已点击过的提示中
	 * param	tipsId	待查找的提示id
	 * return	true则为已有;false则未有
	 */
	bool denied(int tipsId);

	void add(int tipsid, int count);
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


/* 提示主类 */
class TipsManagerI : virtual public TipsManager,
	public MyUtil::Singleton<TipsManagerI>
{
public:
    virtual ::xce::tps::TipsPtr show(::Ice::Int userid, ::Ice::Int website, const Ice::Current&); // 旧接口

    virtual ::xce::tps::TipsPtr get(::Ice::Int tipsid, const Ice::Current&); // 旧接口

    virtual void renew(bool rebuildIndex, const Ice::Current&crt=Ice::Current());

    virtual void relocateUserCache(::Ice::Int userid, const Ice::Current&);

    virtual ::xce::tps::TipsSeq list(const Ice::Current&);	// 旧接口

    virtual void click(::Ice::Int userid, ::Ice::Int tipsId, bool beClose, const Ice::Current&);

    virtual void reloadTipsInterest(int tpsId, bool force, const Ice::Current&);

    void initialize();

    /* @brief  仅从现有提示重建索引(不读数据库, 只按上线时间更新)
     */
    void renewIndexOnly();

    int getMinTipsId() {
	    return minTipsId_;
    }

    void reloadTipsInterest(TipsListPtr tipss, bool force);

    bool isInterested(int tpsId, int userid) {
	    TipsInterestPtr interest = getTpsInterest(tpsId);
	    if ( ! interest ) {
		    // MCE_INFO("[TipsManagerI::isInterested] InterestUser of " << tpsId << " is NULL");
		    return true;
	    } else {
		    bool isInterest =  interest->isSet(userid);
		    if ( isInterest ) {
			    MCE_INFO("[TipsManagerI::isInterested] " << userid << " is InterestUser of " << tpsId << "?:" << isInterest);
		    }
		    return isInterest;
	    }
    }

	
private:
    friend class MyUtil::Singleton<TipsManagerI>;
    TipsManagerI() {
    };
    
    /* @biref 从DB中load一组指定用户id	 
     */	
    TipsInterestPtr loadTipsInterest(const string& tableName);
    string getInterestTable(const TipsPtr& tps);

    void UpdateClickInDB(int userId, int tipsId);

    TipsInterestPtr getTpsInterest(int tpsId);    

    void setTpsInterest(int tpsId, const TipsInterestPtr& interest);

    void eraseTpsInterest(int tpsId);

    UserActionPtr locateAction(Ice::Int uid);

    /* @brief 当前提示列表 */
    TipsListPtr tipss_;

    /* @brief 当前索引 */
    TipsIndexPtr index_;

    /* @brief 当前列表中所有的提示中的最小id */
    int minTipsId_;

    /* @brief 从DB读取提示列表
     */
    TipsListPtr loadTipss();

    /* @brief 由提示列表创建索引
     * @return	创建好的索引
     */
    TipsIndexPtr createIndex(const TipsListPtr& tipss);

    /* @brief 定时重建索引线程 */
    CreateIndexThread createThread_;

    /* @brief 用于锁替换索引 */
    IceUtil::Mutex mutexRenew_;

    map<int, TipsInterestPtr> tpsInterest;
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
/* 读取提示指定用户列表的ResultHandler */
class TipsInterestResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	TipsInterestResultHandler(vector<int>& userIds) : 
		_userIds(userIds) {
		}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	vector<int>& _userIds;
	string _colName;
};

/* 读取提示内容的ResultHandler */
class TipsHandler : public com::xiaonei::xce::ResultHandler {
public:
	TipsHandler(map<int, TipsPtr>& result) :
		result_(result) {
	};
	virtual bool handle(mysqlpp::Row& row) const;
private:
	map<int, TipsPtr>& result_;
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
