#include "GeneralDbServiceI.h"
#include "ServiceI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::generaldbservice;
using namespace std;
using namespace MyUtil;
using namespace Ice;
using namespace IceUtil;
using namespace mysqlpp;
using namespace boost;
using namespace com::xiaonei::xce;
using namespace ::xce::serverstate;

void MyUtil::initialize(){
	MCE_DEBUG("initialize begin");
	ServiceI& service = ServiceI::instance();
	PropertiesPtr props = service.getCommunicator()->getProperties();
        DBDESC.db_name  = props->getPropertyWithDefault("DbName","");
        DBDESC.table_name = props->getPropertyWithDefault("TableName","");
        DBDESC.table_cluster = props->getPropertyAsIntWithDefault("TableCluster",0);
	DBDESC.pattern_prefix = props->getPropertyWithDefault("PatternPrefix","");
	DBDESC.id_column = props->getPropertyWithDefault("IdColumn", "");

//	MCE_DEBUG("DbName: "<<DBDESC.db_name<<" TableName: "<<DBDESC.table_name<<" TableCluster: "<< DBDESC.table_cluster<< " "<< DBDESC.pattern_prefix<<" IdColumn: "<<DBDESC.id_column);

        service.registerObjectCache(CATE_OBJECT, "O", new GeneralDbServiceFactoryI);
        service.getAdapter()->add(&GeneralDbServiceManagerI::instance(), service.createIdentity("M", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("GeneralDbService.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("GeneralDbService.Interval",5);
	string controller = props->getPropertyWithDefault("ControllerName", "");
	ServerStateSubscriber::instance().initialize(controller,&GeneralDbServiceManagerI::instance(),mod,interval);
	MCE_DEBUG("initialize end");
}

// GeneralDbServcieManagerI

GeneralDbServiceManagerI::GeneralDbServiceManagerI(){
}

GeneralDbServiceManagerI::~GeneralDbServiceManagerI(){
}

void GeneralDbServiceManagerI::set(int id, const Str2StrMap& content, const Ice::Current& current) {
	MCE_INFO("GeneralDbServiceManagerI::set -> "<<id);
	Str2StrMap filter;
	filter[DBDESC.id_column]=lexical_cast<string>(id);
	QueryRunner(DBDESC.db_name,CDbWServer).schedule(Statement::buildUpdate(DBDESC.table_name,filter,content));
	vector<string> result=ServiceI::instance().locateObject<GeneralDbServiceDataPtr> (CATE_OBJECT, id)->getProperties();
	for (unsigned int i = 0; i < DBDESC.columns.size(); ++i) {
		Str2StrMap::const_iterator r = content.find(DBDESC.columns[i]);
		if (r != content.end()) {
			result[i] = r->second;
		}
	}
	ServiceI::instance().locateObject<GeneralDbServiceDataPtr> (CATE_OBJECT, id)->setProperties(result);
}

Str2StrMap GeneralDbServiceManagerI::get(int id, const Ice::Current& current) {
	MCE_INFO("GeneralDbServiceManagerI::get -> "<<id);
	Str2StrMap result;
	vector<string> data = ServiceI::instance().locateObject<GeneralDbServiceDataPtr> (CATE_OBJECT, id)->getProperties();
	for(unsigned int i = 0; i < DBDESC.columns.size(); ++i) {
		result[DBDESC.columns.at(i)] = data.at(i);
	}
	return result;
}

void GeneralDbServiceManagerI::reload(int id,const Ice::Current& current){
	MCE_INFO("GeneralDbServiceManagerI::reload -> "<<id);
	ServiceI::instance().reloadObject(CATE_OBJECT,id);
}

void GeneralDbServiceManagerI::create(const Str2StrMap& content,const Ice::Current& current) {
	MCE_INFO("GeneralDbServiceManagerI::create");
	stringstream pattern;
	pattern << DBDESC.pattern_prefix;
	if (DBDESC.table_cluster != 0) {
		pattern << 0;
	}
	QueryRunner(DBDESC.db_name,CDbWServer, pattern.str()).execute(Statement::buildInsert(DBDESC.table_name, content));
}

// GeneralDbServiceFactoryI

GeneralDbServiceFactoryI::GeneralDbServiceFactoryI(){
	stringstream pattern;
	pattern << DBDESC.pattern_prefix;
	if (DBDESC.table_cluster != 0) {
		pattern << 0;
	}

	Statement sql;
	sql << "SELECT * FROM " << DBDESC.table_name;

	if (DBDESC.table_cluster != 0) {
		sql << 0;
	}
	sql << " LIMIT 1";
	GeneralDbServiceFieldsResultHandlerI handler;
	QueryRunner(DBDESC.db_name, CDbRServer, pattern.str()).query(sql, handler);
}

GeneralDbServiceFactoryI::~GeneralDbServiceFactoryI(){
}

ObjectPtr GeneralDbServiceFactoryI::create(Int id){
	MCE_DEBUG("GeneralDbServiceFactoryI::create -> "<<id);
	GeneralDbServiceDataPtr data = new GeneralDbServiceData;
	ostringstream pattern;
	pattern << DBDESC.pattern_prefix;
	if (DBDESC.table_cluster != 0) {
		pattern << (id % DBDESC.table_cluster);
	}

	Statement sql;
	sql << "SELECT * FROM " << DBDESC.table_name;
	if (DBDESC.table_cluster != 0) {
		sql << (id % DBDESC.table_cluster);
	}
	sql << " WHERE " << DBDESC.id_column << "='" << id << "'";
	GeneralDbServiceDataResultHandlerI handler(data);
	QueryRunner(DBDESC.db_name, CDbRServer, pattern.str()).query(sql, handler);

	if (data->getProperties().size() == 0) {
		return 0;
	}

	return data;
}

// GeneralDbServerData

void GeneralDbServiceData::setProperties(const vector<string>& data) {
	RWRecMutex::WLock lock(_dataMutex);
	_data = data;
}

vector<string> GeneralDbServiceData::getProperties() {
	RWRecMutex::WLock lock(_dataMutex);
	return _data;
}

// GeneralDbServiceDataResultHandlerI

GeneralDbServiceDataResultHandlerI::GeneralDbServiceDataResultHandlerI(const GeneralDbServiceDataPtr& data):_data(data){
}

GeneralDbServiceDataResultHandlerI::~GeneralDbServiceDataResultHandlerI(){
}

bool GeneralDbServiceDataResultHandlerI::handle(Row& row) const {
	vector<string> result;
	for (vector<string>::iterator i = DBDESC.columns.begin(); i != DBDESC.columns.end(); ++i) {
#ifndef NEWARCH
		result.push_back(row[(*i).c_str()].get_string());
#else
		result.push_back(row[(*i).c_str()].c_str());
#endif
	}
	_data->setProperties(result);
	return false;
}

// GeneralDbServiceFieldsResultHandlerI

GeneralDbServiceFieldsResultHandlerI::GeneralDbServiceFieldsResultHandlerI(){
}

GeneralDbServiceFieldsResultHandlerI::~GeneralDbServiceFieldsResultHandlerI(){
}

bool GeneralDbServiceFieldsResultHandlerI::handle(Row& row) const {
	value_list_ba<FieldNames, do_nothing_type0> names = row.field_list();
	for(size_t i = 0; i < names.list->size(); ++i) {
		DBDESC.columns.push_back((*names.list)[i].c_str());
	}
	return false;
}
