/*
 * =====================================================================================
 *
 *       Filename:  DistCacheIdProducer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月25日 16时03分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __DIST_CACHE_ID_PRODUCER_H__
#define __DIST_CACHE_ID_PRODUCER_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace distcache {

const std::string DB_FULL_ID = "user_passport";
const std::string DB_ACTIVE_ID = "friendfinder_userlogin";

const int BATCH_SIZE = 3000;


class IDHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	IDHandlerI( std::vector<int>& result, Ice::Int& maxId, int cluster, int mod ) :
		result_(result), maxId_(maxId), cluster_(cluster), mod_(mod) {}
	~IDHandlerI() {}

	virtual bool handle(mysqlpp::Row& row) const {
		int id = (int) row["id"];
		maxId_ = id > maxId_ ? id : maxId_;
		if (id%cluster_ == mod_) {
			result_.push_back( id );
		}
		return true;
	}
private:
	std::vector<int>& result_;
	Ice::Int& maxId_;

	int cluster_;
	int mod_;
};

class IDProducer : virtual public Ice::Object {
public:
	IDProducer(int cluster, int mod) : cluster_(cluster), mod_(mod) {}
	~IDProducer() {}
	virtual std::vector<int> produce(const int beginId, int& maxId) = 0;

	int get_cluster() {return cluster_;}
	int get_mod() {return mod_;}
private:
	int cluster_;
	int mod_;
};
typedef IceUtil::Handle<IDProducer> IDProducerPtr;

class FullIDProducer : virtual public IDProducer {
public:
	FullIDProducer (int cluster, int mod) : IDProducer(cluster, mod) {}
	//~FullIDProduer() {}

	virtual std::vector<int> produce(const int beginId, int& maxId) {
		std::vector<int> results;
		com::xiaonei::xce::Statement sql;
		sql << "SELECT id FROM user_passport WHERE id > " << beginId
			<< " ORDER BY id LIMIT " << BATCH_SIZE;
		IDHandlerI handler( results, maxId , get_cluster(), get_mod());
		com::xiaonei::xce::QueryRunner( DB_FULL_ID, com::xiaonei::xce::CDbRServer ).query( sql, handler );
		return results;
	}
};

class ActiveIDProducer : virtual public IDProducer {
public:
	ActiveIDProducer (int cluster, int mod) : IDProducer(cluster, mod) {
		timestamp_ = time(0);
	}
	//~ActiveIDProduer() {}

	virtual std::vector<int> produce(const int beginId, int& maxId) {
		std::vector<int> results;
		com::xiaonei::xce::Statement sql;
		sql << "SELECT id FROM " << getTableName(timestamp_) << " WHERE id > " << beginId
			<< " ORDER BY id LIMIT " << BATCH_SIZE;
		IDHandlerI handler( results, maxId , get_cluster(), get_mod());
		com::xiaonei::xce::QueryRunner( DB_ACTIVE_ID, com::xiaonei::xce::CDbRServer ).query( sql, handler );
		return results;
	}
private:
	time_t timestamp_;
	std::string getTableName(time_t now) {
		std::string tableName = "user_login_";
		char year[10];
		char month[10];
		char date[10];
		
		time_t activeTime = now - 3*24*3600;//按三天前的日期取表

		strftime(year, sizeof(year), "%Y", localtime(&activeTime));
		strftime(month, sizeof(month), "%m", localtime(&activeTime));
		strftime(date, sizeof(date), "%d", localtime(&activeTime));

		std::string result = tableName + std::string(year) + std::string(month);
		return result;
	}
};


}
}
#endif
