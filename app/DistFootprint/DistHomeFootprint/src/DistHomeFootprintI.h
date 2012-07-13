/*
 * =====================================================================================
 *
 *       Filename:  DistHomeFootprintI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月17日 14时17分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DIST_HOME_FOOTPRINT_I_H__
#define __DIST_HOME_FOOTPRINT_I_H__

#include <FootprintUnite.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "./distcache/DistCacheAdapter.h"
#include "QueryRunner.h"
#include "ServiceI.h"

namespace xce {
namespace footprint {

using namespace xce::distcache;
/**
 * 脚印两级cache大小
 */
const Ice::Int FOOTPRINT_HEAD = 9;
const Ice::Int FOOTPRINT_BODY = 39;

/**
 * 人气控制参数
 */

const Ice::Int VISIT_INTERVAL = 10;
const Ice::Int VISIT_LIMIT = 500;

/**
 * 数据库操作参数
 */
const Ice::Int WRITE_BATCH_SIZE = 100;
const Ice::Int WRITE_WAIT_TIME = 10;

/**
 * 数据表名
 */
const std::string TABLE_HEAD_FOOTPRINT = "";
const std::string TABLE_BODY_FOOTPRINT = "";

/**
 * 数据库字段
 */
const std::string COLUMN_OWNER = "owner";
const static std::string DESC_FOOTPRINT = "footprint";
const static std::string COLUMN_ID = "ID";
const static std::string COLUMN_VIEWCOUNT = "viewcount";
const static std::string COLUMN_VISITOR = "visitors";
const static std::string DESC_VIEW_COUNT = "user_view_count";
const static std::string TABLE_VIEW_COUNT = "view_count";

/**
 * 对应proto的HomeFootprint
 */
class HomeFootprintI : virtual public Ice::Object {
public:
	HomeFootprintI(int visitor, int time, long gid) : 
		visitor_(visitor), time_(time), gid_(gid) {}
	~HomeFootprintI() {}

	int get_visitor() { return visitor_; }
	int get_time() { return time_; }
	long get_gid() { return gid_; }
private:
	int visitor_;//来访者
	int time_;//来访时间
	long gid_;//global id
};//end HomeFootprint
typedef IceUtil::Handle<HomeFootprintI> HomeFootprintIPtr;
/**
 * 对应proto的HomeFootprintData
 */
class HomeFootprintDataI : virtual public xce::distcache::footprint::HomeFootprintData, virtual public Ice::Object {
public:
	HomeFootprintDataI () {}
	HomeFootprintDataI (xce::distcache::DataPtr data);
	~HomeFootprintDataI();

	/**
	 * 踩脚印
	 * return 是否成功
	 */
	bool addFootprint(const FootprintInfoPtr&);

	/**
	 * 返回写入DistCache对象
	 */
	xce::distcache::DataPtr getData();

	xce::distcache::DataPtr removeFootprint(const FootprintInfoPtr&);

	int getSize();

	//void setVisitCount(size_t visit_count) {visit_count_ = visit_count;}

	void addRow(const mysqlpp::Row::value_type& str);

	string getHeadSerialize();
	string getBodySerialize();
private:
	std::vector<HomeFootprintIPtr> footprints_;
	/*
	size_t visit_count_;//人气值

	//下面两个变量控制单位时间访问上限
	int grow_timestamp_;//访问时间戳，控制时间段人气上限
	int grow_count_;//控制时间内人气增长值
	*/

	bool visitCountFilter(const FootprintInfoPtr& fpt);
};//end HomeFootprintDataI
typedef IceUtil::Handle<HomeFootprintDataI> HomeFootprintDataIPtr;

/**
 * 从数据库创建对象类，包括一二级脚印和人气
 */
class HomeFootprintFactoryI : public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(long userId);
};

/**
 * 数据库写类
 */
class WriteThread : public IceUtil::Thread {
public:
	WriteThread(int index, xce::distcache::DistCacheAdapterPtr cache) : index_(index), cache_(cache)
	{}

	void push(long owner);
	virtual void run();
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	int index_;
	xce::distcache::DistCacheAdapterPtr cache_;

	std::set<long> owners_;

	std::string getTableName(std::string table) {
		ostringstream oss;
		oss << table << "_" << index_;
		return oss.str();
	}
};//end WreteThread
typedef IceUtil::Handle<WriteThread> WriteThreadPtr;

class WriteDBHelper : virtual public Ice::Object {
public:
	WriteDBHelper (int table_size, xce::distcache::DistCacheAdapterPtr cache) : table_size_(table_size), cache_(cache) {
		for (int i = 0; i < table_size_; ++i) {
			WriteThreadPtr wt = new WriteThread(i,cache_);
			threads_.push_back(wt);
			threads_[i]->start();
		}
	}

	void write(long owner) {
		threads_[owner%table_size_]->push(owner);
	}
private:
	int table_size_;
	xce::distcache::DistCacheAdapterPtr cache_;
	std::vector<WriteThreadPtr> threads_;

};//end WriteDBHelper
typedef IceUtil::Handle<WriteDBHelper> WriteDBHelperPtr;


/**
 * HomeFootprint接口类
 */
class HomeFootprintCacheI : virtual public HomeFootprintCache,
	public MyUtil::Singleton<HomeFootprintCacheI> {
public:
    HomeFootprintCacheI();
    virtual ~HomeFootprintCacheI();

	void initialize();

    virtual void visit(const FootprintInfoPtr&, const Ice::Current& crt = Ice::Current());
    virtual void visitBatch(const xce::footprint::FootprintInfoSeq&, const Ice::Current&);
    virtual void remove(const xce::footprint::FootprintInfoPtr&, const Ice::Current&);
    virtual ::Ice::Int getSize(Ice::Long, const Ice::Current&);
    virtual HomeInfoPtr getAll(Ice::Long, Ice::Int, Ice::Int, Ice::Int, const Ice::Current& crt = Ice::Current());

    virtual HomeInfoPtr visitAndGet(const FootprintInfoPtr&, Ice::Int, Ice::Int, Ice::Int, const Ice::Current& crt = Ice::Current());
    virtual ::Ice::Int getUserVisitCount(Ice::Long, const Ice::Current& crt=Ice::Current());
    virtual void setUserVisitCount(Ice::Long, Ice::Int, const Ice::Current&);
    virtual void incUserVisitCount(Ice::Long, const Ice::Current& crt=Ice::Current());
    virtual void incUserVisitCountBatch(const ::MyUtil::LongSeq&, Ice::Int,	const Ice::Current&);


    virtual bool isValid(const Ice::Current&);
    virtual void setValid(bool, const Ice::Current&);
    virtual void contral(const ::MyUtil::Str2StrMap&, const Ice::Current&);
private:
	DistCacheAdapterPtr dist_cache_;
	HomeFootprintFactoryI * factory_;
	WriteDBHelperPtr db_helper_;
	HomeFootprintDataIPtr getHomeFootprintDataI(long owner, xce::distcache::DataPtr);
	
};//end HomeFootprintCache

/**
 * 读数据库类
 */
class VisitCountResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        VisitCountResultHandlerI(HomeFootprintDataIPtr& result) : result_(result) {}
        virtual bool handle(mysqlpp::Row&) const;
private:
        HomeFootprintDataIPtr& result_;
};

class HomeFootprintResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        HomeFootprintResultHandlerI(HomeFootprintDataIPtr& result) : result_(result) {};
        virtual bool handle(mysqlpp::Row&) const;
private:
        HomeFootprintDataIPtr& result_;
};

//end 各种数据库加载类



/**
 * 各种控制逻辑，人气一天500上线，要重新实现，逻辑服务要详细记录log
 */

/**
 * 比较器
 */
template<class T>
class FootprintEqualComparator : public std::unary_function<T, bool> {
public:
        FootprintEqualComparator(long visitor) :
                visitor_(visitor) {
        };

        bool operator()(const T& p) const {
            //return (visitor_ == p->visitor);
        }
private:
        long visitor_;
};//end FootprintEqualComparator

}
}
#endif
