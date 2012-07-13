/*
 * GeneralDbCache.h
 *
 *  Created on: 2008-8-19
 *      Author: bbc
 */

#ifndef GENERALDBCACHE_H_
#define GENERALDBCACHE_H_

#include "GeneralDbCache.h"
#include "ObjectCacheI.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include <Singleton.h>
#include <QueryRunner.h>
#include "TopicI.h"
// #include "ServiceAdapter.h"
#include "ServiceI.h"

namespace xce {
namespace generaldbcache {

const int CATE_OBJECT = 0;

//***************************************************************************

class GeneralDbCacheManagerI: virtual public GeneralDbCacheManager,
		public MyUtil::Singleton<GeneralDbCacheManagerI> {
public:
	static DbFieldsPtr dbFields;

	virtual DbFieldsPtr getDbFields(const Ice::Current&);
	virtual DbRowsPtr getDbRows(::Ice::Int, const Ice::Current&);
	virtual void load(::Ice::Int, const Ice::Current&);
};

//***************************************************************************

struct DbDesc {
	std::string columns;
	std::string table_name;
	int table_cluster;
	std::string id_column;

	std::string db_name;
	std::string pattern_prefix;
};

class GeneralDbCacheFactoryI: public MyUtil::ServantFactory {
public:
	GeneralDbCacheFactoryI(const DbDesc& desc);
	virtual Ice::ObjectPtr create(Ice::Int id);
private:
	DbDesc _desc;
};

//***************************************************************************

class DbRowsResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	DbRowsResultHandlerI(DbRowsPtr& rows);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	DbRowsPtr _rows;
};

class DbFieldsResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	virtual bool handle(mysqlpp::Row& row) const;
};

//***************************************************************************

}
}
#endif /* GENERALDBCACHE_H_ */
