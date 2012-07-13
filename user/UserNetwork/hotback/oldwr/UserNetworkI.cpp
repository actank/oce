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
using namespace xce::tempmutex;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(NETWORK_DATA, "N", new UserNetworkDataFactory, false);

	NetworkManagerPtr manager = new NetworkManagerI;
	service.getAdapter()->add(manager, service.createIdentity(NETWORK_MANAGER, ""));

	TaskManager::instance().config(TASK_LEVEL_SYNCCREATE, MyUtil::ThreadPoolConfig(1, 5));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkN.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkN.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerUserNetwork", manager, mod, interval);
	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkN.Status", 0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os << "MyUtil::initialize set status : " << status;
		MCE_WARN( os.str() );
	}
	ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------
NetworkManagerI::NetworkManagerI() :
	_isValid(false) {
	_tempMutexManager = new TempMutexManager();
}

NetworkManagerI::~NetworkManagerI() {
}

//--------------------------------------------------------------
NetworkDataIPtr NetworkManagerI::getNetworkData(int userId) {
	NetworkCacheDataPtr rs = getNetworkCacheData(userId);
	if (rs) {
		return rs->getData();
	}
	return NULL;
}

NetworkCacheDataPtr NetworkManagerI::getNetworkCacheData(int userId) {
	MCE_INFO("[NetworkManagerI::getNetworkCacheData] id=" << userId);
	NetworkCacheDataPtr dataI = ServiceI::instance().findObject<NetworkCacheDataPtr> (NETWORK_DATA, userId);
	if (!dataI) {
		//		//为了读性能不加锁，少数情况下会造成各个Reader的缓存不一致
		//		Lock lock(_tempMutexManager, userId);
		dataI = ServiceI::instance().locateObject<NetworkCacheDataPtr> (NETWORK_DATA, userId);
		if(dataI && isValid()){
			UserNetworkReaderAdapter::instance().setData(userId, dataI->getData());
		}else{
			MCE_WARN("[NetworkManagerI::getNetworkCacheData] No object load");
		}
	}
	return dataI;
}

//--------------------------------------------------------------
NetworkPtr NetworkManagerI::getNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << "  networkId=" << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->networkId == networkId) {
			return *it;
		}
	}
	return NULL;
}

NetworkPtr NetworkManagerI::getRegionalNetwork(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if (((*it)->networkId >= 60000000) && ((*it)->networkId <= 70000000) && (((*it)->status == 0) or ((*it)->status == -1))) {
			return *it;
		}
	}
	return NULL;
}

NetworkPtr NetworkManagerI::getNetworkByNetId(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userid=" << userId << "  networkId=" << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->networkId == networkId) {
			return *it;
		}
	}
	return NULL;
}

NetworkSeq NetworkManagerI::getNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->networks;
}

NetworkSeq NetworkManagerI::getNetworkSeqByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	NetworkSeq res;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->stage == stage || stage == -1) {
			res.push_back(*it);
		}
	}

	return res;
}

NetworkSeq NetworkManagerI::getNetworkSeqByStatus(Ice::Int userId, Ice::Int status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , status=" << status;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	NetworkSeq res;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->status == status) {
			res.push_back(*it);
		}
	}

	return res;
}

NetworkSeq NetworkManagerI::getNetworkSeqByStatuses(Ice::Int userId, const MyUtil::IntSeq& status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;
	NetworkSeq res;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if (find(status.begin(), status.end(), (*it)->status) != status.end()) {
			res.push_back(*it);
		}
	}

	return res;
}

NetworkSeq NetworkManagerI::getNetworkSeqByStageAndStatus(Ice::Int userId, Ice::Int stage, const MyUtil::IntSeq& status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , stage=" << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	NetworkSeq res;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->stage == stage) {
			if (find(status.begin(), status.end(), (*it)->status) != status.end()) {
				res.push_back(*it);
			}
		}
	}
	return res;
}

Ice::Int NetworkManagerI::getNetworkCountByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , stage=" << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkSeqByStage(userId, stage).size();
}

Ice::Int NetworkManagerI::getValidNetworkCountByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , stage=" << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	int count = 0;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->stage == stage || stage == -1) {
			if ((*it)->status == -1 || (*it)->status == 0)
				count++;
		}
	}
	return count;
}

bool NetworkManagerI::isExistInNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , networkid=" << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->networkId == networkId && (*it)->status <= 0) {
			return true;
		}
	}
	return false;
}

void NetworkManagerI::reloadNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			MCE_INFO("[NetworkManagerI::addNetwork] lock : id=" << userId
				<< ", Client=" << (current.con ? current.con->toString().substr(current.con->toString().find("remote",25)).substr(17):"localhost:-1" ));
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_NETWORK, network)).insert_id();
			MyUtil::Str2StrMap& finalNetwok = const_cast<MyUtil::Str2StrMap&> (network);
			finalNetwok.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalNetwok, NETWORKINFO);
			UserNetworkReaderAdapter::instance().addNetwork(userId, finalNetwok);
			MCE_INFO("[NetworkManagerI::addNetwork] unlock : id=" << userId
				<< ", Client=" << (current.con ? current.con->toString().substr(current.con->toString().find("remote",25)).substr(17):"localhost:-1" ));
		}
		UserLogicAdapter::instance().updateUserNetwork(userId, network);
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addNetwork] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addNetwork] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , networkId=" << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColNetworkId] = boost::lexical_cast<string>(networkId);
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_NETWORK, filter, network));
			getNetworkCacheData(userId)->setInfo(networkId, network, NETWORKINFO);
			UserNetworkReaderAdapter::instance().setNetwork(userId, networkId, network);
		}
		UserLogicAdapter::instance().updateUserNetwork(userId, network);
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setNetwork] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setNetwork] UNKNOWN exception");
	}
}

void NetworkManagerI::removeNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , networkId=" << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);
	filter[NetColNetworkId] = boost::lexical_cast<string>(networkId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			MCE_INFO("[NetworkManagerI::removeNetwork] lock : id=" << userId << ", networkId=" << networkId
				<< ", Client=" << (current.con ? current.con->toString().substr(current.con->toString().find("remote",25)).substr(17):"localhost:-1" ));
			conn.doDelete(TABLE_NETWORK, filter);
			getNetworkCacheData(userId)->removeInfo(networkId, NETWORKINFO, INFOTYPEID);
			UserNetworkReaderAdapter::instance().removeNetwork(userId, networkId);
			MCE_INFO("[NetworkManagerI::removeNetwork] unlock : id=" << userId << ", networkId=" << networkId
				<< ", Client=" << (current.con ? current.con->toString().substr(current.con->toString().find("remote",25)).substr(17):"localhost:-1" ));
		}
		UserLogicAdapter::instance().updateUserNetwork(userId, filter);
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeNetwork] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeNetwork] UNKNOWN exception");
	}
}

void NetworkManagerI::removeNetworkByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , stage=" << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);
	filter[NetColSTAGE] = boost::lexical_cast<string>(stage);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_NETWORK, filter);
			getNetworkCacheData(userId)->removeInfo(stage, NETWORKINFO, INFOSTAGE);
			UserNetworkReaderAdapter::instance().removeNetworkByStage(userId, stage);
		}
		UserLogicAdapter::instance().updateUserNetwork(userId, filter);
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeNetworkByStage] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeNetworkByStage] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
HighSchoolInfoPtr NetworkManagerI::getHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << ", schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	for (HighSchoolInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return *it;
		}
	}
	return NULL;
}

HighSchoolInfoPtr NetworkManagerI::getHighSchoolInfoBySchoolId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << ", schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	for (HighSchoolInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->HighSchoolId == id) {
			return *it;
		}
	}
	return NULL;
}

HighSchoolInfoSeq NetworkManagerI::getHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	return data;
}

Ice::Int NetworkManagerI::getHighSchoolInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->highschools.size();
}

void NetworkManagerI::reloadHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& highSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_HIGH_SCHOOL_INFO, highSchoolInfo)).insert_id();
			MyUtil::Str2StrMap& finalHighSchoolInfo = const_cast<MyUtil::Str2StrMap&> (highSchoolInfo);
			finalHighSchoolInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalHighSchoolInfo, HIGHSCHOOLINFO);
			UserNetworkReaderAdapter::instance().addHighSchoolInfo(userId, finalHighSchoolInfo);
		}
		UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addHighSchoolInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColID] = boost::lexical_cast<string>(id);
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_HIGH_SCHOOL_INFO, filter, HighSchoolInfo));
			getNetworkCacheData(userId)->setInfo(id, HighSchoolInfo, HIGHSCHOOLINFO);
			UserNetworkReaderAdapter::instance().setHighSchoolInfo(userId, id, HighSchoolInfo);
		}
		UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setHighSchoolInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeAllHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_HIGH_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, HIGHSCHOOLINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllHighSchoolInfo(userId);
		}
		UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllHighSchoolInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << ", id" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	filter[HighColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_HIGH_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, HIGHSCHOOLINFO, INFOID);
			UserNetworkReaderAdapter::instance().removeHighSchoolInfo(userId, id);
		}
		UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeHighSchoolInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
UniversityInfoPtr NetworkManagerI::getUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , universityId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;

	for (UniversityInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return *it;
		}
	}
	return NULL;
}

UniversityInfoPtr NetworkManagerI::getUniversityInfoByUnivId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , universityId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;

	for (UniversityInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->universityId == id) {
			return *it;
		}
	}
	return NULL;
}

UniversityInfoSeq NetworkManagerI::getUniversityInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;

	return getNetworkData(userId)->universities;
}

Ice::Int NetworkManagerI::getUniversityInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->universities.size();
}

void NetworkManagerI::reloadUniversityInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert("university_info", universityInfo)).insert_id();
			MyUtil::Str2StrMap& finalUniversityInfo = const_cast<MyUtil::Str2StrMap&> (universityInfo);
			finalUniversityInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalUniversityInfo, UNIVERSITYINFO);
			UserNetworkReaderAdapter::instance().addUniversityInfo(userId, finalUniversityInfo);
		}
		UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addUniversityInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , universityId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColID] = boost::lexical_cast<string>(id);
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_UNIVERSITY_INFO, filter, universityInfo));
			getNetworkCacheData(userId)->setInfo(id, universityInfo, UNIVERSITYINFO);
			UserNetworkReaderAdapter::instance().setUniversityInfo(userId, id, universityInfo);
		}
		UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setUniversityInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeAllUniversityInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_UNIVERSITY_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, UNIVERSITYINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllUniversityInfo(userId);
		}
		UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllUniversityInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , universityId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_UNIVERSITY_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, UNIVERSITYINFO, INFOID);
			UserNetworkReaderAdapter::instance().removeUniversityInfo(userId, id);
		}
		UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeUniversityInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " universityId=" << universityId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColUNIVERSITYID] = boost::lexical_cast<string>(universityId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_UNIVERSITY_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(universityId, UNIVERSITYINFO, INFOTYPEID);
			UserNetworkReaderAdapter::instance().removeUniversityInfoByUnivId(userId, universityId);
		}
		UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeUniversityInfoByUnivId] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeUniversityInfoByUnivId] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeUniversityInfoByUnivId] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
WorkplaceInfoPtr NetworkManagerI::getWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return WorkplaceInfoPtr::dynamicCast(getWorkspaceInfo(userId, id));
}

WorkplaceInfoPtr NetworkManagerI::getWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return WorkplaceInfoPtr::dynamicCast(getWorkspaceInfoByWorkId(userId, id));

}

WorkplaceInfoSeq NetworkManagerI::getWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getWorkspaceInfoSeq(userId);

	WorkplaceInfoSeq res;

	for (WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		res.push_back(WorkplaceInfoPtr::dynamicCast(*it));
	}
	return res;
}

void NetworkManagerI::reloadWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = addWorkspaceInfo(userId, workplaceInfo);

	return index;
}

void NetworkManagerI::setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	setWorkspaceInfo(userId, id, workplaceInfo);

}

void NetworkManagerI::removeAllWorkplaceInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_WORKPLACE_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, WORKSPACEINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllWorkplaceInfo(userId);
		}
		UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllWorkplaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllWorkplaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllWorkplaceInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	filter[WorkColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_WORKPLACE_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, WORKSPACEINFO, INFOID);
			UserNetworkReaderAdapter::instance().removeWorkplaceInfo(userId, id);
		}
		UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeWorkplaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeWorkplaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeWorkplaceInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workplaceId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << workplaceId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	removeWorkspaceInfo(userId, workplaceId);
}

Ice::Int NetworkManagerI::getWorkplaceInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->workspaces.size();
}

//--------------------------------------------------------------
WorkspaceInfoPtr NetworkManagerI::getWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getNetworkData(userId)->workspaces;

	for (WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return *it;
		}
	}

	return NULL;
}

WorkspaceInfoPtr NetworkManagerI::getWorkspaceInfoByWorkId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getNetworkData(userId)->workspaces;

	for (WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->workplaceId == id) {
			return *it;
		}
	}

	return NULL;
}

WorkspaceInfoSeq NetworkManagerI::getWorkspaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->workspaces;
}

Ice::Int NetworkManagerI::addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_WORKPLACE_INFO, workspaceInfo)).insert_id();
			MyUtil::Str2StrMap& finalWorkspaceInfo = const_cast<MyUtil::Str2StrMap&> (workspaceInfo);
			finalWorkspaceInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalWorkspaceInfo, WORKSPACEINFO);
			UserNetworkReaderAdapter::instance().addWorkspaceInfo(userId, finalWorkspaceInfo);
		}
		UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addWorkspaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addWorkspaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addWorkspaceInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColID] = boost::lexical_cast<string>(id);
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_WORKPLACE_INFO, filter, workspaceInfo));
			getNetworkCacheData(userId)->setInfo(id, workspaceInfo, WORKSPACEINFO);
			UserNetworkReaderAdapter::instance().setWorkspaceInfo(userId, id, workspaceInfo);
		}
		UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setWorkspaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setWorkspaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setWorkspaceInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , workplaceId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	filter[WorkColWORKPLACEID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_WORKPLACE_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, WORKSPACEINFO, INFOTYPEID);
			UserNetworkReaderAdapter::instance().removeWorkspaceInfo(userId, id);
		}
		UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeWorkspaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeWorkspaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeWorkspaceInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
CollegeInfoPtr NetworkManagerI::getCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , collegeId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CollegeInfoSeq data = getNetworkData(userId)->colleges;

	for (CollegeInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return (*it);
		}
	}
	return NULL;
}

CollegeInfoPtr NetworkManagerI::getCollegeInfoByCollegeId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , collegeId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CollegeInfoSeq data = getNetworkData(userId)->colleges;

	for (CollegeInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->collegeId == id) {
			return (*it);
		}
	}

	return 0;
}

CollegeInfoSeq NetworkManagerI::getCollegeInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->colleges;
}

Ice::Int NetworkManagerI::getCollegeInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->colleges.size();
}

void NetworkManagerI::reloadCollegeInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_COLLEGE_INFO, info)).insert_id();
			MyUtil::Str2StrMap& finalInfo = const_cast<MyUtil::Str2StrMap&> (info);
			finalInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalInfo, COLLEGEINFO);
			UserNetworkReaderAdapter::instance().addCollegeInfo(userId, finalInfo);
		}
		UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addCollegeInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , collegeId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColID] = boost::lexical_cast<string>(id);
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_COLLEGE_INFO, filter, info));
			getNetworkCacheData(userId)->setInfo(id, info, COLLEGEINFO);
			UserNetworkReaderAdapter::instance().setCollegeInfo(userId, id, info);
		}
		UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setCollegeInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeAllCollegeInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_COLLEGE_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, COLLEGEINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllCollegeInfo(userId);
		}
		UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllCollegeInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , collegeId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_COLLEGE_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, COLLEGEINFO, INFOTYPEID);
			UserNetworkReaderAdapter::instance().removeCollegeInfo(userId, id);
		}
		UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeCollegeInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
ElementarySchoolInfoPtr NetworkManagerI::getElementarySchoolInfoById(Ice::Int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", schoolId=" << id;
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
	inMsg << "userId=" << userId << " schoolId=" << id;
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
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->elementaries;
}

ElementarySchoolInfoPtr NetworkManagerI::getElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	ElementarySchoolInfoSeq seq = getNetworkData(userId)->elementaries;

	if (seq.empty()) {
		return 0;
	}
	return seq[0];
}

void NetworkManagerI::reloadElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

void NetworkManagerI::removeAllElementarySchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, ELEMENTARYSCHOOLINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllElementarySchoolInfo(userId);
		}
		UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllElementarySchoolInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeElementarySchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	filter[ElemColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, ELEMENTARYSCHOOLINFO, INFOID);
			UserNetworkReaderAdapter::instance().removeElementarySchoolInfoById(userId, id);
		}
		UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeElementarySchoolInfoById] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeElementarySchoolInfoById] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeElementarySchoolInfoById] UNKNOWN exception");
	}
}

void NetworkManagerI::setElementarySchoolInfo(int userId, int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[ElemColID] = boost::lexical_cast<string>(id);
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_ELEMENTARY_SCHOOL_INFO, filter, info));
			getNetworkCacheData(userId)->setInfo(id, info, ELEMENTARYSCHOOLINFO);
			UserNetworkReaderAdapter::instance().setElementarySchoolInfo(userId, id, info);
		}
		UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setElementarySchoolInfo] UNKNOWN exception");
	}
}

Ice::Int NetworkManagerI::addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_ELEMENTARY_SCHOOL_INFO, info)).insert_id();
			MyUtil::Str2StrMap& finalInfo = const_cast<MyUtil::Str2StrMap&> (info);
			finalInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalInfo, ELEMENTARYSCHOOLINFO);
			UserNetworkReaderAdapter::instance().addElementarySchoolInfo(userId, finalInfo);
		}
		UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addElementarySchoolInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::removeElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, ELEMENTARYSCHOOLINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeElementarySchoolInfo(userId);
		}
		UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeElementarySchoolInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
JuniorHighSchoolInfoPtr NetworkManagerI::getJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", schoolId=" << id;
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
	inMsg << "userId=" << userId << " schoolid=" << id;
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
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->juniorhighschools;
}

JuniorHighSchoolInfoPtr NetworkManagerI::getJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	JuniorHighSchoolInfoSeq data = getNetworkData(userId)->juniorhighschools;

	if (data.empty()) {
		return 0;
	}
	return data[0];
}

void NetworkManagerI::removeAllJuniorHighSchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, JUNIORHIGHSCHOOLINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllJuniorHighSchoolInfo(userId);
		}
		UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);
	filter[JunHighColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(id, JUNIORHIGHSCHOOLINFO, INFOID);
			UserNetworkReaderAdapter::instance().removeJuniorHighSchoolInfoById(userId, id);
		}
		UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeJuniorHighSchoolInfoById] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeJuniorHighSchoolInfoById] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeJuniorHighSchoolInfoById] UNKNOWN exception");
	}
}

void NetworkManagerI::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", schoolId=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[JunHighColID] = boost::lexical_cast<string>(id);
	filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter, data));
			getNetworkCacheData(userId)->setInfo(id, data, JUNIORHIGHSCHOOLINFO);
			UserNetworkReaderAdapter::instance().setJuniorHighSchoolInfo(userId, id, data);
		}
		UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::reloadJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_JUNIOR_HIGH_SCHOOL_INFO, info)).insert_id();
			MyUtil::Str2StrMap& finalJuniorHighSchool = const_cast<MyUtil::Str2StrMap&> (info);
			finalJuniorHighSchool.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalJuniorHighSchool, JUNIORHIGHSCHOOLINFO);
			UserNetworkReaderAdapter::instance().addJuniorHighSchoolInfo(userId, finalJuniorHighSchool);
		}
		UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addJuniorHighSchoolInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::removeJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, JUNIORHIGHSCHOOLINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeJuniorHighSchoolInfo(userId);
		}
		UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
RegionInfoPtr NetworkManagerI::getRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	RegionInfoSeq data = getNetworkData(userId)->regions;
	for (RegionInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->regionId == regionId) {
			return *it;
		}
	}
	return NULL;
}

RegionInfoSeq NetworkManagerI::getRegionInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->regions;
}

void NetworkManagerI::reloadRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkManagerI::addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = 0;
	try {
		{
			Lock lock(_tempMutexManager, userId);
			index = QueryRunner("common", CDbWServer).execute(Statement::buildReplace(TABLE_REGION_INFO, regionInfo)).insert_id();
			MyUtil::Str2StrMap& finalRegion = const_cast<MyUtil::Str2StrMap&> (regionInfo);
			finalRegion.insert(make_pair("id", boost::lexical_cast<string>(index)));
			getNetworkCacheData(userId)->addInfo(finalRegion, REGIONINFO);
			UserNetworkReaderAdapter::instance().addRegionInfo(userId, finalRegion);
		}
		UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::addRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::addRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::addRegionInfo] UNKNOWN exception");
	}
	return index;
}

void NetworkManagerI::removeRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	filter[RegionColREGIONID] = boost::lexical_cast<string>(regionId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_REGION_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(regionId, REGIONINFO, INFOTYPEID);
			UserNetworkReaderAdapter::instance().removeRegionInfo(userId, regionId);
		}
		UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeRegionInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::removeAllRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			conn.doDelete(TABLE_REGION_INFO, filter);
			getNetworkCacheData(userId)->removeInfo(userId, REGIONINFO, INFOALL);
			UserNetworkReaderAdapter::instance().removeAllRegionInfo(userId);
		}
		UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::removeAllRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::removeAllRegionInfo] UNKNOWN exception");
	}
}

void NetworkManagerI::setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColREGIONID] = boost::lexical_cast<string>(regionId);
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_REGION_INFO, filter, regionInfo));
			getNetworkCacheData(userId)->setInfo(regionId, regionInfo, REGIONINFO);
			UserNetworkReaderAdapter::instance().setRegionInfo(userId, regionId, regionInfo);
		}
		UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
	} catch (Ice::Exception& e) {
		MCE_WARN("[NetworkManagerI::setRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[NetworkManagerI::setRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[NetworkManagerI::setRegionInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
NetworkBigObjectPtr NetworkManagerI::getBigObject(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkDataIPtr source = getNetworkData(userId);

	NetworkBigObjectPtr res = new NetworkBigObject();
	try {
		res->networks = source->networks;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObject Exception: get network err");
	}
	try {
		res->highschools = source->highschools;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObject Exception: get highschool info err");
	}
	try {
		res->universitys = source->universities;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObject Exception: get university info err");
	}
	try {

		res->workspaces = source->workspaces;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObject Exception: get workspace info err");
	}
	return res;
}

NetworkBigObjectNPtr NetworkManagerI::getBigObjectN(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	if (userId <= 0) {
		MCE_WARN("NetworkManagerI::getBigObjectN userId=" << userId );
		return new NetworkBigObjectN;
	}

	NetworkCacheDataPtr cd = ServiceI::instance().findObject<NetworkCacheDataPtr> (NETWORK_DATA, userId);
	if (!cd) {
		IceUtil::Monitor<IceUtil::Mutex> mutex;

		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex);

		MyUtil::TaskManager::instance().execute(new CreateTask(userId, mutex, cd));

		mutex.wait();

		if (!cd) {
			return NULL;
		}

	}

	NetworkDataIPtr source = cd->getData();

	NetworkBigObjectNPtr res = new NetworkBigObjectN;
	try {
		res->networks = source->networks;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get network err");
	}
	try {
		res->highschools = source->highschools;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get highschool info err");
	}
	try {
		res->universitys = source->universities;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get university info err");
	}
	try {
		res->workspaces = source->workspaces;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get workspace info err");
	}
	try {
		res->collages = source->colleges;
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get college info err");
	}
	try {
		if (!source->elementaries.empty()) {
			res->elementary = source->elementaries.at(0);
		}
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get elementary info err");
	}
	try {
		if (!source->juniorhighschools.empty()) {
			res->juniorhighschool = source->juniorhighschools.at(0);
		}
	} catch (...) {
		MCE_WARN("NetworkManagerI::getBigObjectN Exception: get junior_high_school info err");
	}
	return res;
}

NetworkDataPtr NetworkManagerI::getNetworkFullData(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId);
}

//--------------------------------------------------------------
bool NetworkManagerI::isValid() {
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}

void NetworkManagerI::setValid(bool valid, const ::Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = valid;
	}
	ServerStateSubscriber::instance().setStatus((int) valid);
}

//仅供NetworkPreloader使用
void NetworkManagerI::setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "size=" << userData->data.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ObjectMap results;

	for (ObjectMap::const_iterator it = userData->data.begin(); it != userData->data.end(); ++it) {
		NetworkDataIPtr rs = new NetworkDataI(NetworkDataPtr::dynamicCast(it->second));

		NetworkCacheDataPtr res = new NetworkCacheData(rs->serialize());

		results.insert(make_pair(it->first, res));

	}

	MyUtil::ServiceI::instance().getObjectCache()->addObjects(NETWORK_DATA, results);
}

void NetworkManagerI::reloadAll(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		Lock lock(_tempMutexManager, userId);
		MyUtil::ServiceI::instance().removeObject(NETWORK_DATA, userId);
		NetworkCacheDataPtr data = MyUtil::ServiceI::instance().locateObject<NetworkCacheDataPtr> (NETWORK_DATA, userId);
		if (data) {
			UserNetworkReaderAdapter::instance().setData(userId, data->getData());
		}
	}
}
