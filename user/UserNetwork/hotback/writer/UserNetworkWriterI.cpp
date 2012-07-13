#include "UserNetworkWriterI.h"
#include "UserNetworkReaderAdapter.h"
#include "LogWrapper.h"
#include "Util.h"
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

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	NetworkWriterPtr manager = new NetworkWriterI;
	service.getAdapter()->add(manager, service.createIdentity(NETWORK_WRITER, ""));

	TaskManager::instance().config(TASK_LEVEL_SYNCCREATE, MyUtil::ThreadPoolConfig(1, 5));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkWriter.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkWriter.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerUserNetworkWriter", manager, mod, interval);
	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkWriter.Status", 0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os << "MyUtil::initialize set status : " << status;
		MCE_WARN( os.str() );
	}
	ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------
NetworkWriterI::NetworkWriterI() {
	_tempMutexManager = new TempMutexManager();
	_factory = new UserNetworkDataFactory();
}

NetworkWriterI::~NetworkWriterI() {
}

//--------------------------------------------------------------
void NetworkWriterI::reloadNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_NETWORK, network)).insert_id();
		MyUtil::Str2StrMap& finalNetwok = const_cast<MyUtil::Str2StrMap&> (network);
		finalNetwok.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addNetwork(userId, finalNetwok);
	}

	UserLogicAdapter::instance().updateUserNetwork(userId, network);
	return index;
}

void NetworkWriterI::setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , networkId = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColNetworkId] = boost::lexical_cast<string>(networkId);
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_NETWORK, filter, network));
		UserNetworkReaderAdapter::instance().setNetwork(userId, networkId, network);
	}

	UserLogicAdapter::instance().updateUserNetwork(userId, network);
}

void NetworkWriterI::removeNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , networkId = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);
	filter[NetColNetworkId] = boost::lexical_cast<string>(networkId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_NETWORK, filter);
		UserNetworkReaderAdapter::instance().removeNetwork(userId, networkId);
	}

	UserLogicAdapter::instance().updateUserNetwork(userId, filter);
}

void NetworkWriterI::removeNetworkByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , stage = " << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[NetColUSERID] = boost::lexical_cast<string>(userId);
	filter[NetColSTAGE] = boost::lexical_cast<string>(stage);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_NETWORK, filter);
		UserNetworkReaderAdapter::instance().removeNetworkByStage(userId, stage);
	}

	UserLogicAdapter::instance().updateUserNetwork(userId, filter);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& highSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_HIGH_SCHOOL_INFO, highSchoolInfo)).insert_id();
		MyUtil::Str2StrMap& finalHighSchoolInfo = const_cast<MyUtil::Str2StrMap&> (highSchoolInfo);
		finalHighSchoolInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addHighSchoolInfo(userId, finalHighSchoolInfo);
	}

	UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColID] = boost::lexical_cast<string>(id);
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_HIGH_SCHOOL_INFO, filter, HighSchoolInfo));
		UserNetworkReaderAdapter::instance().setHighSchoolInfo(userId, id, HighSchoolInfo);
	}

	UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
}

void NetworkWriterI::removeAllHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_HIGH_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllHighSchoolInfo(userId);
	}

	UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
}

void NetworkWriterI::removeHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << ", id" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[HighColUSERID] = boost::lexical_cast<string>(userId);
	filter[HighColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_HIGH_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeHighSchoolInfo(userId, id);
	}

	UserLogicAdapter::instance().updateUserHighSchool(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::reloadUniversityInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert("university_info", universityInfo)).insert_id();
		MyUtil::Str2StrMap& finalUniversityInfo = const_cast<MyUtil::Str2StrMap&> (universityInfo);
		finalUniversityInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addUniversityInfo(userId, finalUniversityInfo);
	}

	UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColID] = boost::lexical_cast<string>(id);
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_UNIVERSITY_INFO, filter, universityInfo));
		UserNetworkReaderAdapter::instance().setUniversityInfo(userId, id, universityInfo);
	}

	UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
}

void NetworkWriterI::removeAllUniversityInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_UNIVERSITY_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllUniversityInfo(userId);
	}

	UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
}

void NetworkWriterI::removeUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_UNIVERSITY_INFO, filter);
		UserNetworkReaderAdapter::instance().removeUniversityInfo(userId, id);
	}

	UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
}

void NetworkWriterI::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " universityId=" << universityId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColUNIVERSITYID] = boost::lexical_cast<string>(universityId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_UNIVERSITY_INFO, filter);
		UserNetworkReaderAdapter::instance().removeUniversityInfoByUnivId(userId, universityId);
	}

	UserLogicAdapter::instance().updateUserUniversity(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::reloadWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index = addWorkspaceInfo(userId, workplaceInfo);

	return index;
}

void NetworkWriterI::setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	setWorkspaceInfo(userId, id, workplaceInfo);

}

void NetworkWriterI::removeAllWorkplaceInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_WORKPLACE_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllWorkplaceInfo(userId);
	}

	UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());

}

void NetworkWriterI::removeWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	filter[WorkColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_WORKPLACE_INFO, filter);
		UserNetworkReaderAdapter::instance().removeWorkplaceInfo(userId, id);
	}

	UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
}

void NetworkWriterI::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workplaceId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , workplaceId=" << workplaceId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	removeWorkspaceInfo(userId, workplaceId);
}

//--------------------------------------------------------------
Ice::Int NetworkWriterI::addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_WORKPLACE_INFO, workspaceInfo)).insert_id();
		MyUtil::Str2StrMap& finalWorkspaceInfo = const_cast<MyUtil::Str2StrMap&> (workspaceInfo);
		finalWorkspaceInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addWorkspaceInfo(userId, finalWorkspaceInfo);
	}

	UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColID] = boost::lexical_cast<string>(id);
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_WORKPLACE_INFO, filter, workspaceInfo));
		UserNetworkReaderAdapter::instance().setWorkspaceInfo(userId, id, workspaceInfo);
	}

	UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
}

void NetworkWriterI::removeWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[WorkColUSERID] = boost::lexical_cast<string>(userId);
	filter[WorkColWORKPLACEID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_WORKPLACE_INFO, filter);
		UserNetworkReaderAdapter::instance().removeWorkspaceInfo(userId, id);
	}

	UserLogicAdapter::instance().updateUserWorkplace(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::reloadCollegeInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_COLLEGE_INFO, info)).insert_id();
		MyUtil::Str2StrMap& finalInfo = const_cast<MyUtil::Str2StrMap&> (info);
		finalInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addCollegeInfo(userId, finalInfo);
	}

	UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColID] = boost::lexical_cast<string>(id);
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_COLLEGE_INFO, filter, info));
		UserNetworkReaderAdapter::instance().setCollegeInfo(userId, id, info);
	}

	UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
}

void NetworkWriterI::removeAllCollegeInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_COLLEGE_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllCollegeInfo(userId);
	}

	UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
}

void NetworkWriterI::removeCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[UniColUSERID] = boost::lexical_cast<string>(userId);
	filter[UniColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_COLLEGE_INFO, filter);
		UserNetworkReaderAdapter::instance().removeCollegeInfo(userId, id);
	}

	UserLogicAdapter::instance().updateUserCollege(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::reloadElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

void NetworkWriterI::removeAllElementarySchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllElementarySchoolInfo(userId);
	}

	UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
}

void NetworkWriterI::removeElementarySchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	filter[ElemColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeElementarySchoolInfoById(userId, id);
	}

	UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
}

void NetworkWriterI::setElementarySchoolInfo(int userId, int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[ElemColID] = boost::lexical_cast<string>(id);
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_ELEMENTARY_SCHOOL_INFO, filter, info));
		UserNetworkReaderAdapter::instance().setElementarySchoolInfo(userId, id, info);
	}

	UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
}

Ice::Int NetworkWriterI::addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_ELEMENTARY_SCHOOL_INFO, info)).insert_id();
		MyUtil::Str2StrMap& finalInfo = const_cast<MyUtil::Str2StrMap&> (info);
		finalInfo.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addElementarySchoolInfo(userId, finalInfo);
	}

	UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::removeElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_ELEMENTARY_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeElementarySchoolInfo(userId);
	}

	UserLogicAdapter::instance().updateUserElementarySchool(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::removeAllJuniorHighSchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllJuniorHighSchoolInfo(userId);
	}

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
}

void NetworkWriterI::removeJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);
	filter[JunHighColID] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeJuniorHighSchoolInfoById(userId, id);
	}

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
}

void NetworkWriterI::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	Str2StrMap filter;
	filter[JunHighColID] = boost::lexical_cast<string>(id);
	filter[JunHighColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter, data));
		UserNetworkReaderAdapter::instance().setJuniorHighSchoolInfo(userId, id, data);
	}

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
}

void NetworkWriterI::reloadJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildInsert(TABLE_JUNIOR_HIGH_SCHOOL_INFO, info)).insert_id();
		MyUtil::Str2StrMap& finalJuniorHighSchool = const_cast<MyUtil::Str2StrMap&> (info);
		finalJuniorHighSchool.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addJuniorHighSchoolInfo(userId, finalJuniorHighSchool);
	}

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::removeJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[ElemColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_JUNIOR_HIGH_SCHOOL_INFO, filter);
		UserNetworkReaderAdapter::instance().removeJuniorHighSchoolInfo(userId);
	}

	UserLogicAdapter::instance().updateUserJuniorHighSchool(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::reloadRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	reloadAll(userId);
}

Ice::Int NetworkWriterI::addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	int index;
	{
		Lock lock(_tempMutexManager, userId);
		index = QueryRunner("common", CDbWServer).execute(Statement::buildReplace(TABLE_REGION_INFO, regionInfo)).insert_id();
		MyUtil::Str2StrMap& finalRegion = const_cast<MyUtil::Str2StrMap&> (regionInfo);
		finalRegion.insert(make_pair("id", boost::lexical_cast<string>(index)));
		UserNetworkReaderAdapter::instance().addRegionInfo(userId, finalRegion);
	}

	UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
	return index;
}

void NetworkWriterI::removeRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	filter[RegionColREGIONID] = boost::lexical_cast<string>(regionId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_REGION_INFO, filter);
		UserNetworkReaderAdapter::instance().removeRegionInfo(userId, regionId);
	}

	UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
}

void NetworkWriterI::removeAllRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);
	ConnectionQuery conn("common", CDbWServer);

	{
		Lock lock(_tempMutexManager, userId);
		conn.doDelete(TABLE_REGION_INFO, filter);
		UserNetworkReaderAdapter::instance().removeAllRegionInfo(userId);
	}

	UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
}

void NetworkWriterI::setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	Str2StrMap filter;
	filter[RegionColREGIONID] = boost::lexical_cast<string>(regionId);
	filter[RegionColUSERID] = boost::lexical_cast<string>(userId);

	{
		Lock lock(_tempMutexManager, userId);
		QueryRunner("common", CDbWServer).execute(Statement::buildUpdate(TABLE_REGION_INFO, filter, regionInfo));
		UserNetworkReaderAdapter::instance().setRegionInfo(userId, regionId, regionInfo);
	}

	UserLogicAdapter::instance().updateUserRegion(userId, Str2StrMap());
}

//--------------------------------------------------------------
void NetworkWriterI::setValid(bool valid, const ::Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	ServerStateSubscriber::instance().setStatus((int) valid);
}

void NetworkWriterI::reloadAll(Ice::Int userId, const Ice::Current& current){
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	NetworkCacheDataPtr data = NetworkCacheDataPtr::dynamicCast(_factory->create(userId));

	if(data){
		UserNetworkReaderAdapter::instance().setData(userId, data->getData());
	}
}
