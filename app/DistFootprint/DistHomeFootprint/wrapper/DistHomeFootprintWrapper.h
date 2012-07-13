/*
 * =====================================================================================
 *
 *       Filename:  DistHomeFootprintWrapper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月25日 18时50分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DIST_HOME_FOOTPRINT_WRAPPER_H__
#define __DIST_HOME_FOOTPRINT_WRAPPER_H__

#include "FootprintUnite.h"
#include "Footprint.pb.h"
#include "QueryRunner.h"
#include "./DistCacheWrapper/src/DistCacheWrapper.h"

namespace xce {
namespace footprint {
using namespace ::MyUtil;
using namespace xce::distcache;
using namespace xce::distcache::footprint;
using namespace com::xiaonei::xce;

/**
 * 人气控制参数
 */

const Ice::Int VISIT_INTERVAL = 10;
const Ice::Int VISIT_LIMIT = 500;

const std::string COLUMN_VIEWCOUNT = "viewcount";
const std::string TABLE_VIEW_COUNT = "view_count";
const std::string DESC_VIEW_COUNT = "user_view_count";

const std::string TABLE_FOOTPRINT_HEAD = "home_footprint_head";
const std::string TABLE_FOOTPRINT_BODY = "home_footprint_body";
const std::string COLUMN_OWNER = "owner";
const std::string COLUMN_VISITOR = "visitors";
const std::string DESC_FOOTPRINT = "footprint";

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
            return (visitor_ == p->visitor());
        }
private:
        long visitor_;
};//end FootprintEqualComparator

template<class T>
class FootprintLessComparator : public std::unary_function<T, bool> {
public:
        FootprintLessComparator(int time) :
                time_(time) {
        };

        bool operator()(const T& p) const {
            return (time_ < p->time());
        }
private:
        int time_;
};//end FootprintLessComparator

/**
 * 对应proto的HomeFootprint
 */
class HomeFootprintI : virtual public xce::distcache::footprint::HomeFootprint, virtual public Ice::Object {
};//end HomeFootprint
typedef IceUtil::Handle<HomeFootprintI> HomeFootprintIPtr;

class HomeFootprintDataI : virtual public xce::distcache::footprint::HomeFootprintData, virtual public Ice::Object {
public:
	void addRow(const mysqlpp::Row::value_type& str) {
		vector<int> tmp;
		tmp.resize(str.size()/sizeof(int)); 
		memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));

		ostringstream oss;
		for ( size_t i = 0; i < tmp.size()/4; ++i ) {
			int visitor = tmp[4*i];
			int gid = ((long)tmp[4*i+1]<<32L) + tmp[4*i+2];
			int time = tmp[4*i+3];

			HomeFootprint* fpt;
			fpt = add_footprints();
			fpt->set_visitor(visitor);
			fpt->set_gid(gid);
			fpt->set_time(time);
		}
	}

	bool addFootprint(const FootprintInfoPtr& fpt) {
	}

	bool removeFootprint(const FootprintInfoPtr& fpt) {
	}

	int getSize() {
	}

	string getHeadSerialize() {
	}

	string getBodySerialize() {
	}
private:
	vector<HomeFootprintIPtr> mem_footprints_;

	void toMemFootprints() {
		for (int i = 0; i < footprints_size(); i++) {
			vector<HomeFootprintIPtr>::iterator it = find_if(mem_footprints_.begin(), mem_footprints_.end(), FootprintLessComparator<HomeFootprintIPtr>(footprints(i).time()));
			HomeFootprintIPtr mem_fpt = new HomeFootprintI();
			mem_fpt->set_visitor(footprints(i).visitor());
			mem_fpt->set_gid(footprints(i).gid());
			mem_fpt->set_time(footprints(i).time());
			mem_footprints_.insert(it, mem_fpt);
		}
	}

	void toProFootprints() {
		clear_footprints();
		for (int i = 0; i < mem_footprints_.size(); i++) {
			HomeFootprint* fpt;
			fpt = add_footprints();
			fpt->set_visitor(mem_footprints_.at(i)->visitor());
			fpt->set_gid(mem_footprints_.at(i)->gid());
			fpt->set_time(mem_footprints_.at(i)->time());
		}
	}

	bool visitCountFilter(const FootprintInfoPtr& fpt) {
		vector<HomeFootprintIPtr>::iterator it = find_if(mem_footprints_.begin(), mem_footprints_.end(), 
				FootprintEqualComparator<HomeFootprintIPtr>(fpt->visitor));

		int now = time(NULL);
		if (it != mem_footprints_.end()) {
			if (now - (*it)->time() < VISIT_INTERVAL) {
				mem_footprints_.erase(it);
				return false;
			}
		}

		if (grow_count() + 1 < VISIT_LIMIT) {
			set_grow_count(grow_count() + 1);
			return true;
		}

		if (now - timestamp() > 24*60*60) {
			set_timestamp(now);
			set_grow_count(0);
			return true;
		}
		return false;
	}
};
typedef IceUtil::Handle<HomeFootprintDataI> HomeFootprintDataIPtr;

/**
 * 读数据库类
 */
class VisitCountResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        VisitCountResultHandlerI(HomeFootprintDataIPtr& result) : result_(result) {}
        virtual bool handle(mysqlpp::Row& row) const {
			int visit_count = (int) row[COLUMN_VIEWCOUNT.c_str()];
			result_ ->set_visit_count(visit_count);
			return true;
		}
private:
        HomeFootprintDataIPtr& result_;
};

class HomeFootprintResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        HomeFootprintResultHandlerI(HomeFootprintDataIPtr& result) : result_(result) {};
        virtual bool handle(mysqlpp::Row& row) const {
			result_ ->addRow(row[COLUMN_VISITOR.c_str()]);
			return true;
		}
private:
        HomeFootprintDataIPtr& result_;
};

//end 各种数据库加载类

class HomeFootprintWrapper : virtual public DistCacheWrapper<HomeFootprintDataI, HomeFootprintDataIPtr>, virtual public Ice::Object {
public:
	HomeFootprintWrapper() : DistCacheWrapper<HomeFootprintDataI, HomeFootprintDataIPtr>("HomeFootprint") {} 
	virtual HomeFootprintDataIPtr create(int64_t id) {

		HomeFootprintDataIPtr result = new HomeFootprintDataI();
		
		try {
			Statement sql;
			sql << "SELECT " << COLUMN_VIEWCOUNT << " FROM " << TABLE_VIEW_COUNT << " WHERE id = '" << id << "'";
			VisitCountResultHandlerI handler(result);
			QueryRunner(DESC_VIEW_COUNT, CDbRServer).query(sql, handler);
		} catch (mysqlpp::Exception& e) {
		} catch (...) {
		}

		try {
			std::string table = getTableName(TABLE_FOOTPRINT_HEAD, id);
			Statement sql;
			sql << "SELECT * FROM " << table << " WHERE " << COLUMN_OWNER << "=" << id;
			HomeFootprintResultHandlerI handler(result);
			QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, handler);
		} catch (mysqlpp::Exception& e) {
		} catch (...) {
		}

		try {
			std::string table = getTableName(TABLE_FOOTPRINT_BODY, id);
			Statement sql;
			sql << "SELECT * FROM " << table << " WHERE " << COLUMN_OWNER << "=" << id;
			HomeFootprintResultHandlerI handler(result);
			QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, handler);
		} catch (mysqlpp::Exception& e) {
		} catch (...) {
		}
		
		return result;
	}
private:
	std::string getTableName(std::string prefix, int64_t id) {
		ostringstream oss;
		oss << prefix << "_" << id;
		return oss.str();
	}
	
};
typedef IceUtil::Handle<HomeFootprintWrapper> HomeFootprintWrapperPtr;

}
}
#endif
