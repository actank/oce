#include "UserAdsCacheI.h"
#include "user/UserAdsCache/loader/UserAdsCacheDataI.cpp"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using namespace IceUtil;
using namespace ::MyUtil;
using namespace ::xce::useradscache;
using namespace ::xce::serverstate;
using namespace ::com::xiaonei::platform::storage;
using namespace mysqlpp;
using namespace ::com::xiaonei::xce;
void MyUtil::initialize() {
        ServiceI& service = ServiceI::instance();
        service.registerObjectCache(USER_ADS_CACHE_URL, "UAC", new UserAdsCacheFactoryI, false);
        service.getAdapter()->add(&UserAdsCacheManagerI::instance(),service.createIdentity(USER_ADS_CACHE_URL_MANAGER, ""));

        TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserAdsCache.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserAdsCache.Interval", 5);

	UserAdsCacheManagerI::instance().set_mod(mod);

        ServerStateSubscriber::instance().initialize("ControllerUserAdsCache", &UserAdsCacheManagerI::instance(), mod, interval);
		ServerStateSubscriber::instance().setStatus(1);
}

UserAdsCacheManagerI::UserAdsCacheManagerI() :_valid(false) {
}

UserAdsCacheDataPtr UserAdsCacheManagerI::getUserAdsCache(::Ice::Int userId, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCache id=" << userId << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        return getUserAdsCacheData(userId);
}

Int2UserAdsCacheDataMap UserAdsCacheManagerI::getUserAdsCacheList(const IntSeq& userIds, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCacheList -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        ObjectResultPtr objres = getUserAdsCacheData(userIds);
        Int2UserAdsCacheDataMap result;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
                ObjectMap::iterator hasRes = objres->data.find(*it);
                if (hasRes != objres->data.end()) {
                        result[*it]=toOld(UserAdsCacheUrlDataPtr::dynamicCast(hasRes->second));
                } else {
                        MCE_WARN("Not Exists: " << *it);
                }
        }
        return result;
}

UserAdsCacheUrlDataPtr UserAdsCacheManagerI::getUserAdsCacheUrl(::Ice::Int userId, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCacheUrl id=" << userId << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        return getUserAdsCacheUrlData(userId);
}

Int2UserAdsCacheUrlDataMap UserAdsCacheManagerI::getUserAdsCacheUrlList(const IntSeq& userIds, const Ice::Current& current) {
	ostringstream msg;
	msg << "UserAdsCacheManagerI::getUserAdsCacheUrlList -> size: " << userIds.size() << " "
	    << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(msg.str());

        ObjectResultPtr objres = getUserAdsCacheUrlData(userIds);
        Int2UserAdsCacheUrlDataMap result;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
                ObjectMap::iterator hasRes = objres->data.find(*it);
                if (hasRes != objres->data.end()) {
                        result[*it]=UserAdsCacheUrlDataPtr::dynamicCast(hasRes->second);
			result[*it]->tinyurl = HeadUrlUtil::instance().getHeadFullUrlFromOuterSite(result[*it]->tinyurl);
                } else {
                        MCE_WARN("Not Exists: " << *it);
                }
        }
        return result;
}

bool UserAdsCacheManagerI::isValid(const Ice::Current& current) {
        IceUtil::RWRecMutex::RLock lock(_validMutex);
	map<string, string>::const_iterator isNew = current.ctx.find("isNewService");
	if (isNew != current.ctx.end()){
		return !_valid;
	}
        return _valid;
}

void UserAdsCacheManagerI::setValid(bool valid, const Ice::Current&) {
        IceUtil::RWRecMutex::WLock lock(_validMutex);
	MCE_INFO("UserAdsCacheManagerI::setValid -> "<<valid);
        _valid = valid;
	if (_valid){
		ServerStateSubscriber::instance().setStatus(1);
	}else{
		ServerStateSubscriber::instance().setStatus(0);
	}
}

void UserAdsCacheManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
        //TaskManager::instance().execute(new FillTask(userData));
}

UserAdsCacheDataPtr UserAdsCacheManagerI::getUserAdsCacheData(int userId) {
	
	//MCE_DEBUG("[UserAdsCacheManagerI::getUserAdsCacheData : " << userId);
        UserAdsCacheUrlDataPtr obj = ServiceI::instance().locateObject<UserAdsCacheUrlDataPtr> (USER_ADS_CACHE_URL,userId);

        if ( obj == 0 ) {
                MCE_WARN("[UserAdsCacheUrlDataPtr::ptr] null " << "[" << userId << "]");
        }
	UserAdsCacheDataPtr objOld = toOld(obj);
        return objOld;
}

ObjectResultPtr UserAdsCacheManagerI::getUserAdsCacheData(
                const IntSeq& userIds) {

        MyUtil::LongSeq uids;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
                        != userIds.end(); ++it) {
                uids.push_back(*it);
        }
	ObjectResultPtr result = new ObjectResult;
	ObjectResultPtr obj = ServiceI::instance().getObjectCache()->locateObjects(USER_ADS_CACHE_URL, uids);
	for(MyUtil::ObjectMap::const_iterator it = obj->data.begin(); it
			!= obj->data.end(); ++it){
		result->data.insert(make_pair((*it).first, toOld(UserAdsCacheUrlDataPtr::dynamicCast((*it).second))));
	}
 //       return ServiceI::instance().getObjectCache()->locateObjects(USER_ADS_CACHE_URL, uids);
          return result;
}

UserAdsCacheUrlDataPtr UserAdsCacheManagerI::getUserAdsCacheUrlData(int userId) {
	//MCE_DEBUG("[UserAdsCacheManagerI::getUserAdsCacheUrlData : " << userId);
        UserAdsCacheUrlDataPtr obj = ServiceI::instance().locateObject<UserAdsCacheUrlDataPtr> (USER_ADS_CACHE_URL,userId);
	if (obj->tinyurl.find("http://") == string::npos){
		obj->tinyurl = HeadUrlUtil::instance().getHeadFullUrlFromOuterSite(obj->tinyurl);
	}
        if ( obj == 0 ) {
                MCE_WARN("[UserAdsCacheUrlDataPtr::ptr] null " << "[" << userId << "]");
        }
        return obj;
}

ObjectResultPtr UserAdsCacheManagerI::getUserAdsCacheUrlData(
                const IntSeq& userIds) {
        MyUtil::LongSeq uids;
        for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it
                        != userIds.end(); ++it) {
                uids.push_back(*it);
        }
        return ServiceI::instance().getObjectCache()->locateObjects(USER_ADS_CACHE_URL, uids);
}

UserAdsCacheDataPtr UserAdsCacheManagerI::toOld(const UserAdsCacheUrlDataPtr& obj){

	UserAdsCacheDataPtr objOld = new UserAdsCacheData;
	objOld->id = obj->id;
	objOld->stage = obj->stage;
	objOld->sex = obj->sex;
	objOld->birthyear = obj->birthyear;
	objOld->birthmonth = obj->birthmonth;
	objOld->birthday = obj->birthday;
	objOld->homeprovince = obj->homeprovince;
	objOld->homecity = obj->homecity;
	objOld->seatprovince = obj->seatprovince;
	objOld->seatcity = obj->seatcity;
	
	objOld->schools = obj->schools;
	objOld->workplaces = obj->workplaces;
	objOld->networks = obj->networks;
	return objOld;
}

FillTask::FillTask(const ObjectResultPtr& userData):_userData(userData) {
}

void FillTask::handle() {
        MCE_DEBUG("[FillTask::handle] set Data size : " << _userData->data.size());
	int cluster = 4;
	int mod = UserAdsCacheManagerI::instance().mod();
	MyUtil::ObjectResultPtr result=new MyUtil::ObjectResult();
	for (map<long, Ice::ObjectPtr>::const_iterator i = _userData->data.begin(); i != _userData->data.end(); ++i) {
		if (i->first % cluster == mod) {
			result->data.insert(*i);
		}
	}
	if (result->data.size() > 0) {
	        ServiceI::instance().getObjectCache()->addObjects(USER_ADS_CACHE_URL, result->data);
	}
}

Ice::ObjectPtr UserAdsCacheFactoryI::create(Ice::Long id) {
        UserAdsCacheUrlDataPtr result = new UserAdsCacheUrlData;
	MCE_INFO("UserAdsCacheLoaderAdapter::instance().load(id): " << id);
	MyUtil::LongSeq ids;
	ids.push_back(id);
	MyUtil::ObjectResultPtr res = create(ids);
	if(!res->data.empty())
	{
		return res->data.at(0);
	}
        return NULL;
}

MyUtil::ObjectResultPtr UserAdsCacheFactoryI::create(const LongSeq& ids) {
        MCE_DEBUG("Loading " << ids.size() << " objects");
	ObjectResultPtr results = new ObjectResult;
	MyUtil::LongSeq::const_iterator it = ids.begin();
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

	if(results->data.empty())
	{
		return results;
	}
	vector<ThreadPtr> threads;
	threads.push_back(new BatchUserUrlThread(oss.str(), results));
	threads.push_back(new BatchUserBornThread(oss.str(), results));
	threads.push_back(new BatchUserStageThread(oss.str(), results));
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

void BatchUserBornThread::run() {
	BatchUserBornResultHandlerI handler(_result);
	QueryRunner(DB_USER_BORN, CDbRServer).query(_sql,handler);
}

BatchUserStageThread::BatchUserStageThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, stage FROM user_stage WHERE id IN " << ids;
}

void BatchUserStageThread::run() {
	BatchUserStageResultHandlerI handler(_result);
	QueryRunner(DB_USER_STAGE, CDbRServer).query(_sql,handler);
}


BatchUserUrlThread::BatchUserUrlThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT id, tinyurl FROM user_url WHERE id IN " << ids;
}

void BatchUserUrlThread::run() {
	BatchUserUrlResultHandlerI handler(_result);
	QueryRunner(DB_USER_URL, CDbRServer).query(_sql,handler);
}

BatchRegionInfoThread::BatchRegionInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,province_name, city_name FROM region_info WHERE userid IN " << ids;
}

void BatchRegionInfoThread::run() {
	BatchRegionResultHandlerI handler(_result);
	QueryRunner(DB_REGION_INFO, CDbRServer).query(_sql,handler);
}

BatchNetworkHistoryThread::BatchNetworkHistoryThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,network_id, stage, status FROM network_history WHERE userid IN " << ids;
}

void BatchNetworkHistoryThread::run() {
	BatchNetworkResultHandlerI handler(_result);
	QueryRunner(DB_REGION_INFO, CDbRServer).query(_sql,handler);
}

BatchWorkplaceInfoThread::BatchWorkplaceInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result) : _result(result) {
	_sql << "SELECT userid,workplace_name, job_title_id, position_id FROM workplace_info WHERE userid IN "<<ids;
}

void BatchWorkplaceInfoThread::run() {
	BatchWorkplaceResultHandlerI handler(_result);
	QueryRunner(DB_WORKPLACE_INFO, CDbRServer).query(_sql,handler);
}

BatchElementarySchoolInfoThread::BatchElementarySchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,elementary_school_id, elementary_school_name, elementary_school_year FROM elementary_school_info WHERE userid IN "<<ids;
}

void BatchElementarySchoolInfoThread::run() {
	BatchElementaryResultHandlerI handler(_result);
	QueryRunner(DB_ELEMENTARY_SCHOOL_INFO, CDbRServer).query(_sql,handler);
}

BatchJuniorHighSchoolInfoThread::BatchJuniorHighSchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,junior_high_school_id,junior_high_school_name, junior_high_school_year FROM junior_high_school_info WHERE userid IN "<<ids;
}

void BatchJuniorHighSchoolInfoThread::run() {
	BatchJuniorResultHandlerI handler(_result);
	QueryRunner(DB_JUNIOR_HIGH_SCHOOL_INFO, CDbRServer).query(_sql,handler);
}

BatchHighSchoolInfoThread::BatchHighSchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,high_school_id, high_school_name,enroll_year FROM high_school_info WHERE userid IN "<<ids;
}

void BatchHighSchoolInfoThread::run() {
	BatchSeniorResultHandlerI handler(_result);
	QueryRunner(DB_HIGH_SCHOOL_INFO, CDbRServer).query(_sql,handler);
}

BatchCollegeInfoThread::BatchCollegeInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,college_id, college_name,enroll_year, department FROM college_info WHERE userid IN "<<ids;
}

void BatchCollegeInfoThread::run() {
	BatchCollegeResultHandlerI handler(_result);
	QueryRunner(DB_COLLEGE_INFO, CDbRServer).query(_sql,handler);
}

BatchUniversityInfoThread::BatchUniversityInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result):_result(result) {
	_sql << "SELECT userid,university_id, university_name, enroll_year, department, dorm FROM university_info WHERE userid IN " << ids;
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
	int status = (int)row["status"];
	if (status >= 3){
		return true;
	}
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
			xce::useradscache::UserAdsCacheGeneralDataIPtr obj = new xce::useradscache::UserAdsCacheGeneralDataI;
                obj->id = id;
                _result->data.insert(make_pair(id, obj));
        } else {
                MCE_WARN("BatchUserPassportResultHandlerI::handle -> unreachable code segment.");
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
                MCE_WARN("BatchUserBornResultHandlerI::handle -> user_url has userid=" << id << " but user_basic not. ");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUserBornProperties(row, "gender", "birth_year","birth_month","birth_day", "home_province", "home_city");
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
                MCE_WARN("BatchUserStageResultHandlerI::handle -> user_url has userid=" << id << " but user_basic not. ");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUserStageProperties(row, "stage");
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
                MCE_WARN("BatchUserUrlResultHandlerI::handle -> user_url has userid=" << id << " but user_basic not. ");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUserUrlProperties(row, "tinyurl");
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
                MCE_WARN("BatchRegionResultHandlerI::handle ->  region_info has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setRegionProperties(row, "province_name", "city_name");
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
                MCE_WARN("BatchNetworkResultHandlerI::handle -> network_history has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setNetworkProperties(row, "network_id", "stage", "status");
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
                MCE_WARN("BatchWorkplaceResultHandlerI::handle -> workplace_info has userid=" << id << " but user_basic not. ");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setWorkplaceProperties(row, "workplace_name", "job_title_id", "position_id");
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
                MCE_WARN("BatchElementaryResultHandlerI::handle -> elementary_school_info has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setElementaryProperties(row, "elementary_school_id","elementary_school_name", "elementary_school_year");
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
                MCE_WARN("BatchJuniorResultHandlerI::handle -> junior_high_school_info has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setJuniorProperties(row, "junior_high_school_id", "junior_high_school_name", "junior_high_school_year");
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
                MCE_WARN("BatchSeniorResultHandlerI::handle -> high_school_info has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setSeniorProperties(row, "high_school_id", "high_school_name", "enroll_year");
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
                MCE_WARN("BatchCollegeResultHandlerI::handle -> college_info has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setCollegeProperties(row, "college_id", "college_name", "enroll_year", "department");
        }
        return true;
}

BatchUniversityResultHandlerI::BatchUniversityResultHandlerI(MyUtil::ObjectResultPtr& result) : _result(result) {
}

bool BatchUniversityResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["userid"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
                MCE_WARN("BatchUniversityResultHandlerI::handle -> university_info has userid=" << id << " but user_basic not.");
        } else {
                xce::useradscache::UserAdsCacheGeneralDataIPtr::dynamicCast(it->second)->setUniversityProperties(row, "university_id", "university_name", "enroll_year", "department", "dorm");
        }
        return true;
}
//-------------------------BatchResultHandlers end-----------------------------

