#include "UserNetworkI.h"
#include "LogWrapper.h"
#include "Util.h"
#include <boost/lexical_cast.hpp>
#include "BatchUpdaterAdapter.h"
#include "UserLogicAdapter.h"
#include "Date.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"
using namespace mop::hi::svc::model;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace ::xce::serverstate;

void
MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(NETWORK_DATA , "N", new UserNetworkDataFactory, false);

	NetworkManagerPtr manager = new NetworkManagerI;
	service.getAdapter()->add(manager, service.createIdentity(NETWORK_MANAGER,""));    
	
	TaskManager::instance().config(TASK_LEVEL_SYNCCREATE, MyUtil::ThreadPoolConfig(1, 5));	

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkN.Mod",0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkN.Interval",5);
	ServerStateSubscriber::instance().initialize("ControllerUserNetwork",manager,mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkN.Status",0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;
		MCE_WARN( os.str() );
	}
  ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------------------------

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
	
	int id= (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setNetworks(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setNetworks(row); 
    }
    return true;
}

bool BatchUserHighSchoolResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["userid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setHighSchools(row); 
    } else 
	{
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
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setUniversities(row); 
    } else 
	{
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
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setWorkspaces(row); 
    } else 
	{
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
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setColleges(row); 
    } else 
	{
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
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setElementaries(row); 
    } else 
	{
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
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setJuniorHighSchools(row); 
    } else 
	{
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
    if (it != _results->data.end())
	{
		mop::hi::svc::model::NetworkDataIPtr::dynamicCast(it->second)->setRegions(row); 
    } else 
	{
		mop::hi::svc::model::NetworkDataIPtr rs = new mop::hi::svc::model::NetworkDataI;
	
		rs->userId = id;
		rs->setRegions(row);
		_results->data.insert(make_pair(id, rs));
    }
    return true;
}


//------------------------------------------------

void NetworkManagerI::reloadNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

NetworkPtr NetworkManagerI::getNetwork(Ice::Int userId, Ice::Int networkId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<"  networkId="<< networkId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->networkId == networkId)
		{
			return *it;
		}
	}
	return NULL;
}

NetworkPtr NetworkManagerI::getRegionalNetwork(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	NetworkSeq data = getNetworkData(userId)->networks;
	
	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if(((*it)->networkId >= 60000000)&&((*it)->networkId<=70000000)&&(((*it)->status==0)or((*it)->status==-1)))
		{
			return *it;
		}
	}
	return NULL;
}

NetworkPtr NetworkManagerI::getNetworkByNetId(Ice::Int userId,
		Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"userid="<<userId <<"  networkId="<< networkId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	NetworkSeq data = getNetworkData(userId)->networks;

	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->networkId == networkId)
		{
			return *it;
		}
	}
	return NULL;
}

NetworkSeq NetworkManagerI::getNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->networks;
}

NetworkSeq NetworkManagerI::getNetworkSeqByStage(Ice::Int userId,
		Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	NetworkSeq res;
	
	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->stage == stage || stage == -1)
		{
			res.push_back(*it);
		}
	}
	
	return res;
}

NetworkSeq NetworkManagerI::getNetworkSeqByStatus(Ice::Int userId,
		Ice::Int status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , status = "<< status;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	NetworkSeq res;
	
	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->status == status)
		{
			res.push_back(*it);
		}
	}
	
	return res;
}

Ice::Int NetworkManagerI::addNetwork(Ice::Int userId,
		const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert(TABLE_NETWORK, network)).insert_id();

	MyUtil::Str2StrMap& finalNetwok = const_cast<MyUtil::Str2StrMap&> (network);

	finalNetwok.insert(make_pair("id",boost::lexical_cast<string>(index)));

	getNetworkCacheData(userId)->addInfo(finalNetwok, NETWORKINFO);

	UserLogicAdapter::instance().updateUserNetwork(userId, network);
	
	return index;
}

void NetworkManagerI::setNetwork(Ice::Int userId, Ice::Int networkId,
		const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , networkId = "<<networkId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColNetworkId] = boost::lexical_cast<string>(networkId);
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);

	QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_NETWORK , filter ,  network ) );

	getNetworkCacheData(userId)->setInfo(networkId, network, NETWORKINFO );
	
	UserLogicAdapter::instance().updateUserNetwork(userId, network);
}
void NetworkManagerI::removeNetwork(Ice::Int userId, Ice::Int networkId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , networkId = "<<networkId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	
	Str2StrMap filter;
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);
	filter[NetColNetworkId] = boost::lexical_cast<string>(networkId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_NETWORK,filter);

	getNetworkCacheData(userId)->removeInfo(networkId, NETWORKINFO, INFOTYPEID);

	UserLogicAdapter::instance().updateUserNetwork(userId, filter);
}

void NetworkManagerI::removeNetworkByStage(Ice::Int userId, Ice::Int stage,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , stage = "<< stage;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	Str2StrMap filter;
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);
	filter[NetColSTAGE] = boost::lexical_cast<string>(stage);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_NETWORK,filter);

	getNetworkCacheData(userId)->removeInfo(stage, NETWORKINFO, INFOSTAGE);
	
	UserLogicAdapter::instance().updateUserNetwork(userId, filter);
}

Ice::Int NetworkManagerI::getNetworkCountByStage(Ice::Int userId,
		Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , stage = "<< stage;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkSeqByStage(userId, stage).size();
}

Ice::Int NetworkManagerI::getValidNetworkCountByStage(Ice::Int userId,
		Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , stage = "<< stage;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	NetworkSeq data = getNetworkData(userId)->networks;

	int count = 0;
	
	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->stage == stage || stage == -1)
		{
			if((*it)->status == -1 || (*it)->status == 0)
				count ++;
		}
	}
	return count;
}

bool NetworkManagerI::isExistInNetwork(Ice::Int userId, Ice::Int networkId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , networkid = "<<networkId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	NetworkSeq data = getNetworkData(userId)->networks;
	
	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->networkId == networkId && (*it)->status <=0)
		{
			return true;
		}
	}
	return false;
}

//----------------------------

void NetworkManagerI::reloadHighSchoolInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

HighSchoolInfoPtr NetworkManagerI::getHighSchoolInfo(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId << ", id=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;
	
	for(HighSchoolInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->id == id)
		{
			return *it;
		}
	}
	return NULL;
}

HighSchoolInfoPtr NetworkManagerI::getHighSchoolInfoBySchoolId(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId << ", id=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;
	
	for(HighSchoolInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->HighSchoolId == id)
		{
			return *it;
		}
	}
	return NULL;
}

HighSchoolInfoSeq NetworkManagerI::getHighSchoolInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	return data;
}

Ice::Int NetworkManagerI::addHighSchoolInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& highSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert(TABLE_HIGH_SCHOOL_INFO, highSchoolInfo)).insert_id();

	MyUtil::Str2StrMap& finalHighSchoolInfo = const_cast<MyUtil::Str2StrMap&> (highSchoolInfo);

	finalHighSchoolInfo.insert(make_pair("id",boost::lexical_cast<string>(index)));

	getNetworkCacheData(userId)->addInfo(finalHighSchoolInfo,HIGHSCHOOLINFO);
	
	UserLogicAdapter::instance().updateUserHighSchool(userId,Str2StrMap());
	
	return index;
}

void NetworkManagerI::setHighSchoolInfo(Ice::Int userId, Ice::Int id,
		const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColID] = boost::lexical_cast<string>(id);
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_HIGH_SCHOOL_INFO , filter ,  HighSchoolInfo ) );
	
	getNetworkCacheData(userId)->setInfo(id, HighSchoolInfo, HIGHSCHOOLINFO);
	
	UserLogicAdapter::instance().updateUserHighSchool(userId,Str2StrMap());
}

void NetworkManagerI::removeAllHighSchoolInfo(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_HIGH_SCHOOL_INFO,filter);
	
	getNetworkCacheData(userId)->removeInfo(userId, HIGHSCHOOLINFO, INFOALL);

	UserLogicAdapter::instance().updateUserHighSchool(userId,Str2StrMap());
}


void NetworkManagerI::removeHighSchoolInfo(Ice::Int userId, Ice::Int id,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<", id"<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	filter[HighColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_HIGH_SCHOOL_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(id, HIGHSCHOOLINFO, INFOID);

	UserLogicAdapter::instance().updateUserHighSchool(userId,Str2StrMap());
}

Ice::Int NetworkManagerI::getHighSchoolInfoCount(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->highschools.size();
}
//-------------------------------------
void NetworkManagerI::reloadUniversityInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

UniversityInfoPtr NetworkManagerI::getUniversityInfo(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;
	
	for(UniversityInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->id == id)
		{
			return *it;
		}
	}
	return NULL;
}

UniversityInfoPtr NetworkManagerI::getUniversityInfoByUnivId(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;
	
	for(UniversityInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->universityId == id)
		{
			return *it;
		}
	}
	return NULL;
}

UniversityInfoSeq NetworkManagerI::getUniversityInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;
	
	return getNetworkData(userId)->universities;
}

Ice::Int NetworkManagerI::addUniversityInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert("university_info", universityInfo)).insert_id();
	
	MyUtil::Str2StrMap& finalUniversityInfo= const_cast<MyUtil::Str2StrMap&> (universityInfo);

	finalUniversityInfo.insert(make_pair("id",boost::lexical_cast<string>(index)));

	getNetworkCacheData(userId)->addInfo(finalUniversityInfo,UNIVERSITYINFO);
	
	UserLogicAdapter::instance().updateUserUniversity(userId,Str2StrMap());
	return index;
}

void NetworkManagerI::setUniversityInfo(Ice::Int userId, Ice::Int id,
		const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColID] = boost::lexical_cast<string>(id);
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_UNIVERSITY_INFO , filter ,  universityInfo) );

	getNetworkCacheData(userId)->setInfo(id, universityInfo, UNIVERSITYINFO);

	UserLogicAdapter::instance().updateUserUniversity(userId,Str2StrMap());

}

void NetworkManagerI::removeAllUniversityInfo(Ice::Int userId, 
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_UNIVERSITY_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(userId, UNIVERSITYINFO, INFOALL);
	
	UserLogicAdapter::instance().updateUserUniversity(userId,Str2StrMap());
}
void NetworkManagerI::removeUniversityInfo(Ice::Int userId, Ice::Int id,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_UNIVERSITY_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(id, UNIVERSITYINFO, INFOID);
	
	UserLogicAdapter::instance().updateUserUniversity(userId,Str2StrMap());
}

void NetworkManagerI::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId << " universityId=" << universityId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);  
	
	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColUNIVERSITYID] = boost::lexical_cast<string>(universityId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_UNIVERSITY_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(universityId, UNIVERSITYINFO, INFOTYPEID);
	
	UserLogicAdapter::instance().updateUserUniversity(userId,Str2StrMap());
}

Ice::Int NetworkManagerI::getUniversityInfoCount(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->universities.size();
}

//-------------------------------------------

void NetworkManagerI::reloadWorkplaceInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

WorkplaceInfoPtr NetworkManagerI::getWorkplaceInfo(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return WorkplaceInfoPtr::dynamicCast(getWorkspaceInfo(userId, id));
}

WorkplaceInfoPtr NetworkManagerI::getWorkplaceInfoByWorkId(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return WorkplaceInfoPtr::dynamicCast(getWorkspaceInfoByWorkId(userId, id));

}

WorkplaceInfoSeq NetworkManagerI::getWorkplaceInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getWorkspaceInfoSeq(userId); 

	WorkplaceInfoSeq res;
	
	for(WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		res.push_back(WorkplaceInfoPtr::dynamicCast(*it));
	}
	return res;
}

Ice::Int NetworkManagerI::addWorkplaceInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	int index = addWorkspaceInfo(userId, workplaceInfo);

	return index;
}

void NetworkManagerI::setWorkplaceInfo(Ice::Int userId, Ice::Int id,
		const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	setWorkspaceInfo(userId, id, workplaceInfo);

}
//---------------------------------------------------------------------------
void NetworkManagerI::removeAllWorkplaceInfo(Ice::Int userId, 
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_WORKPLACE_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(userId, WORKSPACEINFO, INFOALL);
	
	UserLogicAdapter::instance().updateUserWorkplace(userId,Str2StrMap());

}
void NetworkManagerI::removeWorkplaceInfo(Ice::Int userId, Ice::Int id,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	filter[WorkColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_WORKPLACE_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(id, WORKSPACEINFO, INFOID);

	UserLogicAdapter::instance().updateUserWorkplace(userId,Str2StrMap());
}

void NetworkManagerI::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workplaceId,
		const Ice::Current& current){
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , workplaceId="<< workplaceId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	removeWorkspaceInfo(userId,workplaceId);
}

Ice::Int NetworkManagerI::getWorkplaceInfoCount(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->workspaces.size();
}

WorkspaceInfoPtr NetworkManagerI::getWorkspaceInfo(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getNetworkData(userId)->workspaces;
	
	for(WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->id == id)
		{
			return *it;
		}
	}
	
	return NULL;
}

WorkspaceInfoPtr NetworkManagerI::getWorkspaceInfoByWorkId(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getNetworkData(userId)->workspaces;
	
	for(WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->workplaceId == id)
		{
			return *it;
		}
	}
	
	return NULL;
}

WorkspaceInfoSeq NetworkManagerI::getWorkspaceInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return getNetworkData(userId)->workspaces;
}

Ice::Int NetworkManagerI::addWorkspaceInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert(TABLE_WORKPLACE_INFO, workspaceInfo)).insert_id();
	
	MyUtil::Str2StrMap& finalWorkspaceInfo= const_cast<MyUtil::Str2StrMap&> (workspaceInfo);

	finalWorkspaceInfo.insert(make_pair("id",boost::lexical_cast<string>(index)));

	getNetworkCacheData(userId)->addInfo(finalWorkspaceInfo,WORKSPACEINFO);
	
	UserLogicAdapter::instance().updateUserWorkplace(userId,Str2StrMap());
	
	return index;

}

void NetworkManagerI::setWorkspaceInfo(Ice::Int userId, Ice::Int id,
		const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColID] = boost::lexical_cast<string>(id);
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_WORKPLACE_INFO , filter ,  workspaceInfo) );

	getNetworkCacheData(userId)->setInfo(id, workspaceInfo, WORKSPACEINFO);
	
	UserLogicAdapter::instance().updateUserWorkplace(userId,Str2StrMap());

}

void NetworkManagerI::removeWorkspaceInfo(Ice::Int userId, Ice::Int id,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	filter[WorkColWORKPLACEID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_WORKPLACE_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(id, WORKSPACEINFO, INFOID);

	UserLogicAdapter::instance().updateUserWorkplace(userId,Str2StrMap());
}

NetworkSeq NetworkManagerI::getNetworkSeqByStatuses(Ice::Int userId,
		 const MyUtil::IntSeq& status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

        NetworkSeq data = getNetworkData(userId)->networks;
	NetworkSeq res;

        for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
        {
		if (find(status.begin(), status.end(), (*it)->status) != status.end())
                {
                        res.push_back(*it);
                }
        }


	return res;
}



NetworkSeq NetworkManagerI::getNetworkSeqByStageAndStatus(Ice::Int userId,
		Ice::Int stage, const MyUtil::IntSeq& status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , stage="<< stage;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	NetworkSeq res;
	
	for(NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		if((*it)->stage == stage)
		{
			if (find(status.begin(), status.end(), (*it)->status) != status.end()) {
				res.push_back(*it);
			}
		}
	}
	return res;
}

// **************************************************************************

void NetworkManagerI::reloadCollegeInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

CollegeInfoPtr NetworkManagerI::getCollegeInfo(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CollegeInfoSeq data = getNetworkData(userId)->colleges;

	for(CollegeInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
        {
                if((*it)->id == id)
                {
                        return (*it);
                }
        }
	return NULL;
}

CollegeInfoPtr NetworkManagerI::getCollegeInfoByCollegeId(Ice::Int userId,
		Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CollegeInfoSeq data = getNetworkData(userId)->colleges;

	for(CollegeInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
        {
                if((*it)->collegeId == id)
                {
                        return (*it);
                }
        }

	return 0;
}

CollegeInfoSeq NetworkManagerI::getCollegeInfoSeq(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->colleges;
}

Ice::Int NetworkManagerI::addCollegeInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert(TABLE_COLLEGE_INFO, info)).insert_id();

	MyUtil::Str2StrMap& finalInfo = const_cast<MyUtil::Str2StrMap&> (info);

    finalInfo.insert(make_pair("id",boost::lexical_cast<string>(index)));

    getNetworkCacheData(userId)->addInfo(finalInfo, COLLEGEINFO);

	UserLogicAdapter::instance().updateUserCollege(userId,Str2StrMap());
	
	return index;
}

void NetworkManagerI::setCollegeInfo(Ice::Int userId, Ice::Int id,
		const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColID] = boost::lexical_cast<string>(id);
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_COLLEGE_INFO, filter ,  info) );

	getNetworkCacheData(userId)->setInfo(id, info, COLLEGEINFO);

	UserLogicAdapter::instance().updateUserCollege(userId,Str2StrMap());
}

void NetworkManagerI::removeAllCollegeInfo(Ice::Int userId, 
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_COLLEGE_INFO,filter);
	
	getNetworkCacheData(userId)->removeInfo(userId, COLLEGEINFO, INFOALL);

	UserLogicAdapter::instance().updateUserCollege(userId,Str2StrMap());
}
void NetworkManagerI::removeCollegeInfo(Ice::Int userId, Ice::Int id,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId <<" , id="<< id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_COLLEGE_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(id, COLLEGEINFO, INFOTYPEID);

	UserLogicAdapter::instance().updateUserCollege(userId,Str2StrMap());
}

Ice::Int NetworkManagerI::getCollegeInfoCount(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->colleges.size();
}

// **************************************************************************

void NetworkManagerI::reloadElementarySchoolInfo(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

ElementarySchoolInfoPtr NetworkManagerI::getElementarySchoolInfoById(Ice::Int userId, int id, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId << " id=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  ElementarySchoolInfoSeq seq = getNetworkData(userId)->elementaries;
  for (ElementarySchoolInfoSeq::iterator i = seq.begin(); i != seq.end(); ++i) {
    if (ElementarySchoolInfoNPtr::dynamicCast(*i)->id == id) {
      return *i;
    }
  }
  return NULL;
}

ElementarySchoolInfoPtr NetworkManagerI::getElementarySchoolInfoBySchoolId(Ice::Int userId, int id, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId << " schoolId=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  ElementarySchoolInfoSeq seq = getNetworkData(userId)->elementaries;
  for (ElementarySchoolInfoSeq::iterator i = seq.begin(); i != seq.end(); ++i) {
    if (ElementarySchoolInfoNPtr::dynamicCast(*i)->elementarySchoolId == id) {
      return *i;
    }
  }
  return NULL;
}

ElementarySchoolInfoSeq NetworkManagerI::getElementarySchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  return getNetworkData(userId)->elementaries;
}

void NetworkManagerI::removeAllElementarySchoolInfo(int userId, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Str2StrMap filter;
  filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
  ConnectionQuery conn("common", CDbWServer);
  conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO,filter);
  
  getNetworkCacheData(userId)->removeInfo(userId, ELEMENTARYSCHOOLINFO, INFOALL);
  UserLogicAdapter::instance().updateUserElementarySchool(userId,Str2StrMap());
}

void NetworkManagerI::removeElementarySchoolInfoById(int userId,int id, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Str2StrMap filter;
  filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
  filter[ElemColID] = boost::lexical_cast<string>(id);
  ConnectionQuery conn("common", CDbWServer);
  conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO,filter);
  getNetworkCacheData(userId)->removeInfo(id, ELEMENTARYSCHOOLINFO, INFOID);
  UserLogicAdapter::instance().updateUserElementarySchool(userId,Str2StrMap());
}

void NetworkManagerI::setElementarySchoolInfo(int userId, int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg << "userId=" << userId << " id=" << id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Str2StrMap filter;
  filter[ElemColID] = boost::lexical_cast<string>(id);
  filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
  QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_ELEMENTARY_SCHOOL_INFO , filter ,  info) );
  getNetworkCacheData(userId)->setInfo(id, info, ELEMENTARYSCHOOLINFO);
  UserLogicAdapter::instance().updateUserElementarySchool(userId,Str2StrMap());
}

ElementarySchoolInfoPtr NetworkManagerI::getElementarySchoolInfo(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	ElementarySchoolInfoSeq seq = getNetworkData(userId)->elementaries;

	if(seq.empty()){
		return 0;
	}
	return seq[0];
}

Ice::Int NetworkManagerI::addElementarySchoolInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert(TABLE_ELEMENTARY_SCHOOL_INFO, info)).insert_id();

	MyUtil::Str2StrMap& finalInfo = const_cast<MyUtil::Str2StrMap&> (info);

        
	finalInfo.insert(make_pair("id",boost::lexical_cast<string>(index)));

	getNetworkCacheData(userId)->addInfo(finalInfo, ELEMENTARYSCHOOLINFO);

	UserLogicAdapter::instance().updateUserElementarySchool(userId,Str2StrMap());
	return index;
}


void NetworkManagerI::removeElementarySchoolInfo(Ice::Int userId, 
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO,filter);
	getNetworkCacheData(userId)->removeInfo(userId, ELEMENTARYSCHOOLINFO, INFOALL);

	UserLogicAdapter::instance().updateUserElementarySchool(userId,Str2StrMap());
}
// **************************************************************************

void NetworkManagerI::removeAllJuniorHighSchoolInfo(int userId, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Str2StrMap filter;
  filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);
  ConnectionQuery conn("common", CDbWServer);
  conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO,filter);
  getNetworkCacheData(userId)->removeInfo(userId, JUNIORHIGHSCHOOLINFO, INFOALL);
  UserLogicAdapter::instance().updateUserJuniorHighSchool(userId,Str2StrMap());
}

void NetworkManagerI::removeJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId << " id="<<id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Str2StrMap filter;
  filter[JunHighColUSERID]=boost::lexical_cast<string>(userId);
  filter[JunHighColID]=boost::lexical_cast<string>(id);
  ConnectionQuery conn("common", CDbWServer);
  conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO,filter);
  getNetworkCacheData(userId)->removeInfo(id, JUNIORHIGHSCHOOLINFO, INFOID);
  UserLogicAdapter::instance().updateUserJuniorHighSchool(userId,Str2StrMap());
}

JuniorHighSchoolInfoPtr NetworkManagerI::getJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId << " id="<<id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  JuniorHighSchoolInfoSeq seq = getNetworkData(userId)->juniorhighschools;
  for (JuniorHighSchoolInfoSeq::iterator i = seq.begin(); i != seq.end(); ++i) {
    if (JuniorHighSchoolInfoNPtr::dynamicCast(*i)->id == id) {
      return *i;
    }
  }
  return NULL;
}

JuniorHighSchoolInfoPtr NetworkManagerI::getJuniorHighSchoolInfoBySchoolId(int userId, int id, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId << " schoolid="<<id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  JuniorHighSchoolInfoSeq seq = getNetworkData(userId)->juniorhighschools;
  for (JuniorHighSchoolInfoSeq::iterator i = seq.begin(); i != seq.end(); ++i) {
    if (JuniorHighSchoolInfoNPtr::dynamicCast(*i)->juniorHighSchoolId == id) {
      return *i;
    }
  }
  return NULL;
}

JuniorHighSchoolInfoSeq NetworkManagerI::getJuniorHighSchoolInfoSeq(int userId, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  return getNetworkData(userId)->juniorhighschools;
}

void NetworkManagerI::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data, const Ice::Current& current) {
  ostringstream inMsg;
  inMsg<<"userId=" << userId << " id=" <<id;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Str2StrMap filter;
  filter[JunHighColID] = boost::lexical_cast<string>(id);
  filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);
  QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_JUNIOR_HIGH_SCHOOL_INFO , filter ,  data) );
  getNetworkCacheData(userId)->setInfo(id, data, JUNIORHIGHSCHOOLINFO);
  UserLogicAdapter::instance().updateUserJuniorHighSchool(userId,Str2StrMap());
}

void NetworkManagerI::reloadJuniorHighSchoolInfo(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

JuniorHighSchoolInfoPtr NetworkManagerI::getJuniorHighSchoolInfo(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	JuniorHighSchoolInfoSeq data = getNetworkData(userId)->juniorhighschools;

	if(data.empty()){
		return 0;
	}
	return data[0];
}

Ice::Int NetworkManagerI::addJuniorHighSchoolInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildInsert(TABLE_JUNIOR_HIGH_SCHOOL_INFO, info)).insert_id();

	MyUtil::Str2StrMap& finalJuniorHighSchool = const_cast<MyUtil::Str2StrMap&> (info);

	finalJuniorHighSchool.insert(make_pair("id",boost::lexical_cast<string>(index)));
	
	
	getNetworkCacheData(userId)->addInfo(finalJuniorHighSchool, JUNIORHIGHSCHOOLINFO);

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId,Str2StrMap());
	return index;
}

void NetworkManagerI::removeJuniorHighSchoolInfo(Ice::Int userId, 
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(userId, JUNIORHIGHSCHOOLINFO, INFOALL);

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId,Str2StrMap());
}

// **************************************************************************
void NetworkManagerI::reloadRegionInfo(Ice::Int userId,
	       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI::instance().reloadObject(NETWORK_DATA, userId);
}

RegionInfoPtr NetworkManagerI::getRegionInfo(Ice::Int userId, 
		Ice::Int regionId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId << " , regionId=" << regionId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	RegionInfoSeq data = getNetworkData(userId)->regions;
        for(RegionInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it)
        {
                if((*it)->regionId == regionId)
                {
                        return *it;
                }
        }
        return NULL;
}

RegionInfoSeq NetworkManagerI::getRegionInfoSeq(Ice::Int userId,
		const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->regions;
}

Ice::Int NetworkManagerI::addRegionInfo(Ice::Int userId,
		const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	int index = QueryRunner("common" , CDbWServer ).execute(Statement::buildReplace(TABLE_REGION_INFO, regionInfo)).insert_id();

        MyUtil::Str2StrMap& finalRegion = const_cast<MyUtil::Str2StrMap&> (regionInfo);

        finalRegion.insert(make_pair("id",boost::lexical_cast<string>(index)));

        getNetworkCacheData(userId)->addInfo(finalRegion, REGIONINFO);


	UserLogicAdapter::instance().updateUserRegion(userId,Str2StrMap());
	return index;
}

void NetworkManagerI::removeRegionInfo(Ice::Int userId,
		Ice::Int regionId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId << " , regionId=" << regionId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	filter[RegionColREGIONID] = boost::lexical_cast<string>(regionId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_REGION_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(regionId,REGIONINFO , INFOTYPEID);

	UserLogicAdapter::instance().updateUserRegion(userId,Str2StrMap());
}

void NetworkManagerI::removeAllRegionInfo(Ice::Int userId,
		const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);
	conn.doDelete(TABLE_REGION_INFO,filter);

	getNetworkCacheData(userId)->removeInfo(userId, REGIONINFO, INFOALL);
	UserLogicAdapter::instance().updateUserRegion(userId,Str2StrMap());
}

void NetworkManagerI::setRegionInfo(Ice::Int userId,
		Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id = " << userId << " , regionId=" << regionId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColREGIONID] = boost::lexical_cast<string>(regionId);
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	QueryRunner( "common", CDbWServer ).execute( Statement::buildUpdate( TABLE_REGION_INFO , filter , regionInfo) );

	getNetworkCacheData(userId)->setInfo(regionId, regionInfo, REGIONINFO);

	UserLogicAdapter::instance().updateUserRegion(userId,Str2StrMap());
}
// **************************************************************************


NetworkBigObjectPtr NetworkManagerI::getBigObject(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkDataIPtr source = getNetworkData(userId);

	NetworkBigObjectPtr res = new NetworkBigObject();
	try
	{
		res->networks = source->networks;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObject Exception: get network err");
	}
	try {
		res->highschools = source->highschools;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObject Exception: get highschool info err");
	}

	try {
		res->universitys = source->universities;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObject Exception: get university info err");
	}
	try
	{

		res->workspaces = source->workspaces;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObject Exception: get workspace info err");
	}
	return res;
}


NetworkBigObjectNPtr NetworkManagerI::getBigObjectN(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	if( userId <= 0)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN userId = " << userId );
		return new NetworkBigObjectN;
	}

	NetworkCacheDataPtr cd = ServiceI::instance().findObject<NetworkCacheDataPtr>(NETWORK_DATA, userId);
	if( !cd )
	{
		IceUtil::Monitor<IceUtil::Mutex> mutex;
		
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex);

		MyUtil::TaskManager::instance().execute(new CreateTask(userId, mutex, cd));

		mutex.wait();

		if( !cd )
		{
			return NULL;
		}

	}

	NetworkDataIPtr source = cd->getData();
	
	NetworkBigObjectNPtr res = new NetworkBigObjectN;
	try
	{
		res->networks = source->networks;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get network err");
	}
	try {
		res->highschools = source->highschools;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get highschool info err");
	}

	try {
		res->universitys = source->universities;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get university info err");
	}
	try
	{
		res->workspaces = source->workspaces;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get workspace info err");
	}
	try
	{
		res->collages = source->colleges;
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get college info err");
	}
	try
	{
		if( !source->elementaries.empty() )
		{
			res->elementary = source->elementaries.at(0);
		}
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get elementary info err");
	}
	try
	{
		if( !source->juniorhighschools.empty() )
		{
			res->juniorhighschool = source->juniorhighschools.at(0);
		}
	} catch(...)
	{
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get junior_high_school info err");
	}
	return res;
}


NetworkDataPtr NetworkManagerI::getNetworkFullData(Ice::Int userId,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id = " << userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId);
}


// **************************************************************************


NetworkDataIPtr NetworkManagerI::getNetworkData(int userId)
{
	NetworkCacheDataPtr rs = getNetworkCacheData(userId);
	
	if(rs)
	{
		return rs->getData();
	}

	return NULL;
}

NetworkCacheDataPtr NetworkManagerI::getNetworkCacheData(int userId)
{
	return ServiceI::instance().locateObject<NetworkCacheDataPtr>(NETWORK_DATA, userId);
}

void NetworkManagerI::setValid(bool valid, const ::Ice::Current& current)
{
  InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	ServerStateSubscriber::instance().setStatus((int)valid);
}

void NetworkManagerI::setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& current)
{
  ostringstream oss;
  oss << "size = " << userData->data.size();
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ObjectMap results;

	for(ObjectMap::const_iterator it = userData->data.begin(); it != userData->data.end(); ++it)
	{
		NetworkDataIPtr rs = new NetworkDataI(NetworkDataPtr::dynamicCast(it->second));

		NetworkCacheDataPtr res = new NetworkCacheData(rs->serialize());
		
		results.insert(make_pair(it->first, res));

	}
	
	MyUtil::ServiceI::instance().getObjectCache()->addObjects(NETWORK_DATA, results);
}


