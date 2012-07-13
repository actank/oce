/*
 * GeneralDbCache.cpp
 *
 *  Created on: 2008-8-19
 *      Author: bbc
 */

#include "GeneralDbCacheI.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace mysqlpp;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::generaldbcache;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	DbDesc desc;
	desc.columns = props->getProperty("Columns");
	desc.table_name = props->getProperty("TableName");
	desc.table_cluster = props->getPropertyAsInt("TableCluster");
	desc.id_column = props->getProperty("IdColumn");
	desc.db_name = props->getProperty("DbName");
	desc.pattern_prefix = props->getProperty("PatternPrefix");

	service.registerObjectCache(CATE_OBJECT, "O",
                        new GeneralDbCacheFactoryI(desc));


	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("GeneralDbCache.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("GeneralDbCache.Interval", 5);
	string controllerEndpoints = service.getCommunicator()->getProperties()->getProperty("GeneralDbCache.ControllerEndpoint");

	ServerStateSubscriber::instance().initialize(controllerEndpoints,
			                       &GeneralDbCacheManagerI::instance(), mod, interval);


	service.getAdapter()->add(&GeneralDbCacheManagerI::instance(),
			service.createIdentity("M", ""));
}

//***************************************************************************

DbFieldsPtr GeneralDbCacheManagerI::dbFields;

DbFieldsPtr GeneralDbCacheManagerI::getDbFields(const Ice::Current&) {
	return dbFields;
}

DbRowsPtr GeneralDbCacheManagerI::getDbRows(::Ice::Int userId,
		const Ice::Current&) {
	return ServiceI::instance().locateObject<DbRowsPtr> (CATE_OBJECT, userId);
}

void GeneralDbCacheManagerI::load(::Ice::Int id, const Ice::Current&) {
	ServiceI::instance().getObjectCache()->reloadObject(CATE_OBJECT, id);
}

//***************************************************************************

GeneralDbCacheFactoryI::GeneralDbCacheFactoryI(const DbDesc& desc) :
	_desc(desc) {
	int id = 1;

	ostringstream pattern;
	pattern << _desc.pattern_prefix;
	if (_desc.table_cluster != 0) {
		pattern << (id % _desc.table_cluster);
	}

	Statement sql;
	sql << "SELECT " << _desc.columns << " FROM " << _desc.table_name;
	if (_desc.table_cluster != 0) {
		sql << (id % _desc.table_cluster);
	}
	sql << " LIMIT 1";
	DbFieldsResultHandlerI handler;
	QueryRunner(_desc.db_name, CDbRServer, pattern.str()).query(sql,
			handler);
}

Ice::ObjectPtr GeneralDbCacheFactoryI::create(Ice::Int id) {
	MCE_DEBUG("Loading " << id);
	DbRowsPtr result = new DbRows;
	ostringstream pattern;
	pattern << _desc.pattern_prefix;
	if (_desc.table_cluster != 0) {
		pattern << (id % _desc.table_cluster);
	}

	Statement sql;
	sql << "SELECT " << _desc.columns << " FROM " << _desc.table_name;
	if (_desc.table_cluster != 0) {
		sql << (id % _desc.table_cluster);
	}
	sql << " WHERE " << _desc.id_column << "='" << id << "'";
	DbRowsResultHandlerI handler(result);
	QueryRunner(_desc.db_name, CDbRServer, pattern.str()).query(sql,
			handler);
	MCE_DEBUG("Loading " << id << " done.");
	return result;
}

//***************************************************************************

DbRowsResultHandlerI::DbRowsResultHandlerI(DbRowsPtr& rows) :
	_rows(rows) {
}

bool DbRowsResultHandlerI::handle(mysqlpp::Row& row) const {
	DbRowPtr onerow = new DbRow;
	for (MyUtil::StrSeq::iterator it =
			GeneralDbCacheManagerI::dbFields->fields.begin(); it
			!= GeneralDbCacheManagerI::dbFields->fields.end(); ++it) {
#ifndef NEWARCH
		onerow->values.push_back(row[(*it).c_str()].get_string());
#else
		onerow->values.push_back(row[(*it).c_str()].c_str());
#endif
	}
	_rows->rows.push_back(onerow);
	return true;
}
//***************************************************************************

bool DbFieldsResultHandlerI::handle(mysqlpp::Row& row) const {
	DbFieldsPtr fields = new DbFields;
	value_list_ba<FieldNames, do_nothing_type0> names = row.field_list();
	for (size_t i = 0; i < names.list->size(); ++i) {
		fields->fields.push_back((*names.list)[i].c_str());
	}
	GeneralDbCacheManagerI::dbFields = fields;
	return false;
}
//***************************************************************************
