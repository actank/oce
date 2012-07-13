#include "UserAdsCacheLoaderI.h"
#include "UserAdsCacheAdapter.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"

using namespace std;
using namespace IceUtil;
using namespace xce::useradscache;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserAdsCacheLoaderI::instance(), service.createIdentity("M", ""));

	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserAdsCache.Controller", "ControllerUserAdsCache");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserAdsCache.Proxy.Timeout", 300);
        int produceBatchSize = 3000;
	int consumeBatchSize = 3000;
        int writeBatchSize = 3000;
        int threadSize = 1;
	CacheLoader<UserAdsCacheManagerPrx>::instance().initialize(
		new BatchUsersIdProducer,
                new UserAdsCacheFactory,
		ctlEndpoint,
                subInterval,
                timeout,
                produceBatchSize,
		consumeBatchSize,
                writeBatchSize,
                threadSize );	
}


void UserAdsCacheLoaderI::load(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	MCE_DEBUG("UserAdsCacheLoaderI::load -> size: "<<ids.size()<<" " << current.con->toString().substr(current.con->toString().find("remote",25)));
	MyUtil::IntSeq is;
  for (MyUtil::IntSeq::const_iterator i = ids.begin(); i != ids.end(); ++i) {
    if (*i > 0) {
      is.push_back(*i);
    }
  }
  if (is.size() <= 0) {
    return ;
  }
  CacheLoader<UserAdsCacheManagerPrx>::instance().reload(is);
}

//--------------------------UserAdsCacheFactory::create-----------------------------------------------------
MyUtil::ObjectResultPtr UserAdsCacheFactory::create(Ice::Int minId,Ice::Int maxId) {
	ObjectResultPtr results = new ObjectResult;
	
	Statement sql;
	sql << "SELECT id, status FROM user_passport WHERE id >= "<<minId<<" AND id <= "<<maxId;
	BatchUserPassportResultHandlerI handler(results);
        QueryRunner(DB_USER_PASSPORT, CDbRServer).query(sql,handler);

	vector<ThreadPtr> threads;
	threads.push_back(new BatchUserBornThread(minId,maxId,results));
	threads.push_back(new BatchUserStageThread(minId,maxId,results));
	threads.push_back(new BatchUserUrlThread(minId,maxId, results));
	threads.push_back(new BatchRegionInfoThread(minId,maxId, results));
	threads.push_back(new BatchNetworkHistoryThread(minId,maxId, results));
	threads.push_back(new BatchWorkplaceInfoThread(minId,maxId, results));
	threads.push_back(new BatchElementarySchoolInfoThread(minId,maxId, results));
	threads.push_back(new BatchJuniorHighSchoolInfoThread(minId,maxId, results));
	threads.push_back(new BatchHighSchoolInfoThread(minId,maxId, results));
	threads.push_back(new BatchCollegeInfoThread(minId,maxId, results));
	threads.push_back(new BatchUniversityInfoThread(minId,maxId, results));
	for (vector<ThreadPtr>::iterator itp = threads.begin(); itp != threads.end(); ++itp) {
		(*itp)->start();
	}
	for (vector<ThreadPtr>::iterator itc = threads.begin(); itc != threads.end(); ++itc) {
		(*itc)->getThreadControl().join();
	}
	return results;
}

MyUtil::ObjectResultPtr UserAdsCacheFactory::create(const MyUtil::IntSeq& ids) {
	ObjectResultPtr results = new ObjectResult;
	MyUtil::IntSeq::const_iterator it = ids.begin();
	ostringstream oss;
	
        oss << "(" << *it;
        ++it;
	for (; it != ids.end(); ++it)
	       oss << "," << *it;
        oss << ")";
	
	Statement sql;
	sql << "SELECT id, status FROM user_passport WHERE id IN " << oss.str();
	BatchUserPassportResultHandlerI handler(results);
	QueryRunner(DB_USER_PASSPORT, CDbRServer).query(sql,handler);

	vector<ThreadPtr> threads;
	threads.push_back(new BatchUserBornThread(oss.str(), results));
	threads.push_back(new BatchUserStageThread(oss.str(), results));
	threads.push_back(new BatchUserUrlThread(oss.str(), results));
	threads.push_back(new BatchRegionInfoThread(oss.str(), results));
	threads.push_back(new BatchNetworkHistoryThread(oss.str(), results));
	threads.push_back(new BatchWorkplaceInfoThread(oss.str(), results));
	threads.push_back(new BatchElementarySchoolInfoThread(oss.str(), results));
	threads.push_back(new BatchJuniorHighSchoolInfoThread(oss.str(), results));
	threads.push_back(new BatchHighSchoolInfoThread(oss.str(), results));
	threads.push_back(new BatchCollegeInfoThread(oss.str(), results));
	threads.push_back(new BatchUniversityInfoThread(oss.str(), results));
	for (vector<ThreadPtr>::iterator itp = threads.begin(); itp != threads.end(); ++itp) {
		(*itp)->start();
	}
	for (vector<ThreadPtr>::iterator itc = threads.begin(); itc != threads.end(); ++itc) {
		(*itc)->getThreadControl().join();
	}
	return results;
}

//---------------------------------Thread-----------------------------

BatchUserBornThread::BatchUserBornThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, gender, birth_year, birth_month, birth_day, home_province, home_city FROM user_born WHERE id IN " << ids;
}

BatchUserBornThread::BatchUserBornThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, gender, birth_year, birth_month, birth_day, home_province, home_city FROM user_born WHERE id >= "<<minId<<" AND id <= "<<maxId;
}

void BatchUserBornThread::run() {
	BatchUserBornResultHandlerI handler(_result);
	QueryRunner(DB_USER_BORN, CDbRServer).query(_sql,handler);
}

BatchUserStageThread::BatchUserStageThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, stage FROM user_stage WHERE id IN " << ids;
}

BatchUserStageThread::BatchUserStageThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, stage FROM user_stage WHERE id >= "<<minId<<" AND id <= "<<maxId;
}

void BatchUserStageThread::run() {
	BatchUserStageResultHandlerI handler(_result);
	QueryRunner(DB_USER_STAGE, CDbRServer).query(_sql,handler);
}

BatchUserUrlThread::BatchUserUrlThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, tinyurl FROM user_url WHERE id IN " << ids;
}

BatchUserUrlThread::BatchUserUrlThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, tinyurl FROM user_url WHERE id >= "<<minId<<" AND id <= "<<maxId;
}

void BatchUserUrlThread::run() {
	BatchUserUrlResultHandlerI handler(_result);
	QueryRunner(DB_USER_URL, CDbRServer).query(_sql,handler);
}

BatchRegionInfoThread::BatchRegionInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,province_name, city_name FROM region_info WHERE userid IN " << ids;
}

BatchRegionInfoThread::BatchRegionInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,province_name, city_name FROM region_info WHERE userid >= "<<minId<<" AND userid <= "<<maxId;
}

void BatchRegionInfoThread::run() {
	BatchRegionResultHandlerI handler(_result);
	QueryRunner(DB_REGION_INFO, CDbRServer).query(_sql,handler);
}

BatchNetworkHistoryThread::BatchNetworkHistoryThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,network_id, stage, status FROM network_history WHERE userid IN " << ids;
}

BatchNetworkHistoryThread::BatchNetworkHistoryThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,network_id, stage, status FROM network_history WHERE userid >= "<<minId<<" AND userid <= "<<maxId;
}


void BatchNetworkHistoryThread::run() {
	BatchNetworkResultHandlerI handler(_result);
	QueryRunner(DB_REGION_INFO, CDbRServer).query(_sql,handler);
}

BatchWorkplaceInfoThread::BatchWorkplaceInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,workplace_name, job_title_id, position_id FROM workplace_info WHERE userid IN "<<ids;
}

BatchWorkplaceInfoThread::BatchWorkplaceInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,workplace_name, job_title_id, position_id FROM workplace_info WHERE userid >= "<<minId << " AND userid <= "<<maxId;
}

void BatchWorkplaceInfoThread::run() {
	BatchWorkplaceResultHandlerI handler(_result);
	QueryRunner(DB_WORKPLACE_INFO, CDbRServer).query(_sql,handler);
}

BatchElementarySchoolInfoThread::BatchElementarySchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,elementary_school_id, elementary_school_name, elementary_school_year FROM elementary_school_info WHERE userid IN "<<ids;
}

BatchElementarySchoolInfoThread::BatchElementarySchoolInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,elementary_school_id, elementary_school_name, elementary_school_year FROM elementary_school_info WHERE userid >= "<<minId<<" AND userid <= "<<maxId;
}

void BatchElementarySchoolInfoThread::run() {
	BatchElementaryResultHandlerI handler(_result);
	QueryRunner(DB_ELEMENTARY_SCHOOL_INFO, CDbRServer).query(_sql,handler);
}

BatchJuniorHighSchoolInfoThread::BatchJuniorHighSchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,junior_high_school_id,junior_high_school_name, junior_high_school_year FROM junior_high_school_info WHERE userid IN "<<ids;
}

BatchJuniorHighSchoolInfoThread::BatchJuniorHighSchoolInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,junior_high_school_id,junior_high_school_name, junior_high_school_year FROM junior_high_school_info WHERE userid >= "<<minId<< " AND userid <= "<<maxId;
}

void BatchJuniorHighSchoolInfoThread::run() {
	BatchJuniorResultHandlerI handler(_result);
	QueryRunner(DB_JUNIOR_HIGH_SCHOOL_INFO, CDbRServer).query(_sql,handler);
}

BatchHighSchoolInfoThread::BatchHighSchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,high_school_id, high_school_name,enroll_year FROM high_school_info WHERE userid IN "<<ids;
}

BatchHighSchoolInfoThread::BatchHighSchoolInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,high_school_id, high_school_name,enroll_year FROM high_school_info WHERE userid >= "<<minId<<" AND userid <= "<<maxId;
}

void BatchHighSchoolInfoThread::run() {
	BatchSeniorResultHandlerI handler(_result);
	QueryRunner(DB_HIGH_SCHOOL_INFO, CDbRServer).query(_sql,handler);
}

BatchCollegeInfoThread::BatchCollegeInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,college_id, college_name,enroll_year, department FROM college_info WHERE userid IN "<<ids;
}

BatchCollegeInfoThread::BatchCollegeInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,college_id, college_name,enroll_year, department FROM college_info WHERE userid >= "<<minId<<" AND userid <= "<<maxId;
}

void BatchCollegeInfoThread::run() {
	BatchCollegeResultHandlerI handler(_result);
	QueryRunner(DB_COLLEGE_INFO, CDbRServer).query(_sql,handler);
}

BatchUniversityInfoThread::BatchUniversityInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,university_id, university_name, enroll_year, department, dorm FROM university_info WHERE userid IN " << ids;
}

BatchUniversityInfoThread::BatchUniversityInfoThread(int minId, int maxId, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,university_id, university_name, enroll_year, department, dorm FROM university_info WHERE userid >= "<<minId<<" AND userid <= "<<maxId;
}

void BatchUniversityInfoThread::run() {
	BatchUniversityResultHandlerI handler(_result);
	QueryRunner(DB_UNIVERSITY_INFO, CDbRServer).query(_sql,handler);
}

//-------------------------BatchResultHandlers-----------------------------
BatchUserPassportResultHandlerI::BatchUserPassportResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
		_result(result){
}

bool BatchUserPassportResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                UserAdsCacheGeneralDataIPtr obj = new UserAdsCacheGeneralDataI;
                obj->id = id;
                _result->data.insert(make_pair(id, obj));
        } else {
                MCE_WARN("BatchUserPassportResultHandlerI::handle -> id = " << id << "conflict");
        }
        return true;
}

BatchUserBornResultHandlerI::BatchUserBornResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserBornResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchUserBornResultHandlerI::handle -> user_born has userid=" << id << " but user_passport not. ");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUserBornProperties(row, "gender", "birth_year","birth_month","birth_day", "home_province", "home_city");
        }
        return true;
}

BatchUserStageResultHandlerI::BatchUserStageResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserStageResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchUserStageResultHandlerI::handle -> user_stage has userid=" << id << " but user_passport not. ");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUserStageProperties(row, "stage");
        }
        return true;
}

BatchUserUrlResultHandlerI::BatchUserUrlResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserUrlResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchUserUrlResultHandlerI::handle -> user_url has userid=" << id << " but user_passport not. ");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUserUrlProperties(row, "tinyurl");
        }
        return true;
}

BatchRegionResultHandlerI::BatchRegionResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchRegionResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchRegionResultHandlerI::handle ->  region_info has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setRegionProperties(row, "province_name", "city_name");
        }
        return true;
}

BatchNetworkResultHandlerI::BatchNetworkResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchNetworkResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchNetworkResultHandlerI::handle -> network_history has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setNetworkProperties(row, "network_id", "stage", "status");
        }
        return true;
}

BatchWorkplaceResultHandlerI::BatchWorkplaceResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchWorkplaceResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchWorkplaceResultHandlerI::handle -> workplace_info has userid=" << id << " but user_passport not. ");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setWorkplaceProperties(row, "workplace_name", "job_title_id", "position_id");
        }
        return true;
}

BatchElementaryResultHandlerI::BatchElementaryResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchElementaryResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchElementaryResultHandlerI::handle -> elementary_school_info has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setElementaryProperties(row, "elementary_school_id","elementary_school_name", "elementary_school_year");
        }
        return true;
}

BatchJuniorResultHandlerI::BatchJuniorResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchJuniorResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchJuniorResultHandlerI::handle -> junior_high_school_info has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setJuniorProperties(row, "junior_high_school_id", "junior_high_school_name", "junior_high_school_year");
        }
        return true;
}

BatchSeniorResultHandlerI::BatchSeniorResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchSeniorResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchSeniorResultHandlerI::handle -> high_school_info has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setSeniorProperties(row, "high_school_id", "high_school_name", "enroll_year");
        }
        return true;
}

BatchCollegeResultHandlerI::BatchCollegeResultHandlerI(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchCollegeResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchCollegeResultHandlerI::handle -> college_info has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setCollegeProperties(row, "college_id", "college_name", "enroll_year", "department");
        }
        return true;
}

BatchUniversityResultHandlerI::BatchUniversityResultHandlerI(MyUtil::ObjectResultPtr& result) : _result(result) {
}

bool BatchUniversityResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchUniversityResultHandlerI::handle -> university_info has userid=" << id << " but user_passport not.");
        } else {
                UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUniversityProperties(row, "university_id", "university_name", "enroll_year", "department", "dorm");
        }
        return true;
}
//-------------------------BatchResultHandlers end-----------------------------

