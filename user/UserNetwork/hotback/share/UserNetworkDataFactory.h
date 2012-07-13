#ifndef __USER_NETWORKDATAFACTORY_H__
#define __USER_NETWORKDATAFACTORY_H__

//#include "DataSet.h"
//#include "ObjectCacheI.h"
//#include <IceUtil/IceUtil.h>
#include "UserNetworkDataI.h"
#include <QueryRunner.h>
#include "LogWrapper.h"
#include "Util.h"

namespace mop {
namespace hi {
namespace svc {
namespace model {
using namespace MyUtil;

const string DB_MASTER = "fb_master";
const string DB_SLAVE = "fb_slave";
//database table name
const string DB_NETWORK = "common";
//database table name
const string TABLE_NETWORK = "network_history";
const string TABLE_HIGH_SCHOOL_INFO = "high_school_info";
const string TABLE_UNIVERSITY_INFO = "university_info";
const string TABLE_WORKPLACE_INFO = "workplace_info";
const string TABLE_COLLEGE_INFO = "college_info";
const string TABLE_ELEMENTARY_SCHOOL_INFO = "elementary_school_info";
const string TABLE_JUNIOR_HIGH_SCHOOL_INFO = "junior_high_school_info";
const string TABLE_REGION_INFO = "region_info";

//--------------------------------------------------------------
class UserNetworkDataFactory: virtual public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};
typedef IceUtil::Handle<UserNetworkDataFactory> UserNetworkDataFactoryPtr;

class BatchUserNetworkResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserNetworkResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserHighSchoolResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserHighSchoolResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserUniversityResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserUniversityResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserWorkplaceResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserWorkplaceResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserCollegeResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserCollegeResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserElementarySchoolResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserElementarySchoolResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserJuniorHighSchoolResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserJuniorHighSchoolResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

class BatchUserRegionResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserRegionResultHandler(MyUtil::ObjectResultPtr& results) :
		_results(results) {
	}
	;
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _results;
};

//--------------------------------------------------------------

Ice::ObjectPtr UserNetworkDataFactory::create(Ice::Int userId) {
	MCE_INFO("UserNetworkDataFactory::create userid = "<< userId);

	ObjectResultPtr results = new ObjectResult;

	NetworkDataIPtr newData = new NetworkDataI;

	newData->userId = userId;

	results->data.insert(make_pair(userId, newData));

	{
		BatchUserNetworkResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, stage, network_id, network_name,"
			<< 	" info_id, join_time, status"
			<<  " FROM " << TABLE_NETWORK << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserHighSchoolResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, high_school_id, high_school_name,"
			<< 	" enroll_year, h_class1, h_class2, h_class3"
			<<  " FROM " << TABLE_HIGH_SCHOOL_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserUniversityResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, university_id, university_name, dorm,"
			<< 	" department, type_of_course, enroll_year"
			<<  " FROM " << TABLE_UNIVERSITY_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserWorkplaceResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, workplace_id, workplace_name, address,"
			<< 	" province, city_id, city_name, "
			<< 	" description, join_year, join_month, "
			<< 	" quit_year, quit_month, type, "
			<< 	" job_title_id, position_id"
			<<  " FROM " << TABLE_WORKPLACE_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserCollegeResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, college_id, college_name, department,"
			<< 	" enroll_year"
			<<  " FROM " << TABLE_COLLEGE_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserElementarySchoolResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, elementary_school_id, elementary_school_name, elementary_school_year"
			<<  " FROM " << TABLE_ELEMENTARY_SCHOOL_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserJuniorHighSchoolResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, junior_high_school_id, junior_high_school_name, junior_high_school_year"
			<<  " FROM " << TABLE_JUNIOR_HIGH_SCHOOL_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}
	{
		BatchUserRegionResultHandler handler(results );
		Statement sql;
		sql << "SELECT id, userid, region_id, province_name, city_name"
			<<  " FROM " << TABLE_REGION_INFO << " WHERE userid="<< userId;
		QueryRunner(DB_NETWORK , CDbRServer ).query(sql, handler);
	}

	if(results->data.size()>0)
	{
		ObjectMap::const_iterator findIt = results->data.find(userId);
		if(findIt != results->data.end())
		{
			NetworkDataIPtr ndi = NetworkDataIPtr::dynamicCast(findIt->second);
			BinStringPtr bp = ndi->serialize();
			NetworkCacheDataPtr res = new NetworkCacheData(bp);
			return res;
		}else
		{
			MCE_WARN("UserNetworkDataFactory::create can't find data for userid =" << userId);
		}
	} else
	{
		MCE_WARN("UserNetworkDataFactory::create can't find data for userid =" << userId);
	}

	return NULL;
}

bool BatchUserNetworkResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it == _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setNetworks(row);
		_results->data.insert(make_pair(id, rs));
	} else {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setNetworks(row);
	}
	return true;
}

bool BatchUserHighSchoolResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setHighSchools(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setHighSchools(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

bool BatchUserUniversityResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setUniversities(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setUniversities(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

bool BatchUserWorkplaceResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setWorkspaces(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setWorkspaces(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

bool BatchUserCollegeResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setColleges(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setColleges(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

bool BatchUserElementarySchoolResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setElementaries(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setElementaries(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

bool BatchUserJuniorHighSchoolResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setJuniorHighSchools(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setJuniorHighSchools(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

bool BatchUserRegionResultHandler::handle(mysqlpp::Row& row) const {

	int id = (int) row["userid"];
	MyUtil::ObjectMap::iterator it = _results->data.find(id);
	if (it != _results->data.end()) {
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setRegions(row);
	} else {
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;

		rs->userId = id;
		rs->setRegions(row);
		_results->data.insert(make_pair(id, rs));
	}
	return true;
}

}
}
}
}
#endif

