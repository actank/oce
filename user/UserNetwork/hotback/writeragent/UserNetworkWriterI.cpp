#include "UserNetworkWriterI.h"
#include "UserNetworkAdapter.h"
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
}

NetworkWriterI::~NetworkWriterI() {
}

//--------------------------------------------------------------
void NetworkWriterI::reloadNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadNetworkSeq(userId);
}

Ice::Int NetworkWriterI::addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addNetwork(userId, network);
}

void NetworkWriterI::setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , networkId = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setNetwork(userId, networkId, network);
}

void NetworkWriterI::removeNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , networkId = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeNetwork(userId, networkId);
}

void NetworkWriterI::removeNetworkByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , stage = " << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeNetworkByStage(userId, stage);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadHighSchoolInfoSeq(userId);
}

Ice::Int NetworkWriterI::addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& highSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addHighSchoolInfo(userId, highSchoolInfo);
}

void NetworkWriterI::setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setHighSchoolInfo(userId, id, HighSchoolInfo);
}

void NetworkWriterI::removeAllHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllHighSchoolInfo(userId);
}

void NetworkWriterI::removeHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << ", id" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeHighSchoolInfo(userId, id);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadUniversityInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadUniversityInfoSeq(userId);
}

Ice::Int NetworkWriterI::addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addUniversityInfo(userId, universityInfo);
}

void NetworkWriterI::setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setUniversityInfo(userId, id, universityInfo);
}

void NetworkWriterI::removeAllUniversityInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllUniversityInfo(userId);
}

void NetworkWriterI::removeUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeUniversityInfo(userId, id);
}

void NetworkWriterI::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " universityId=" << universityId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeUniversityInfoByUnivId(userId, universityId);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadWorkplaceInfoSeq(userId);
}

Ice::Int NetworkWriterI::addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addWorkplaceInfo(userId, workplaceInfo);
}

void NetworkWriterI::setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setWorkplaceInfo(userId, id, workplaceInfo);
}

void NetworkWriterI::removeAllWorkplaceInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllWorkplaceInfo(userId);
}

void NetworkWriterI::removeWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeWorkplaceInfo(userId, id);
}

void NetworkWriterI::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workplaceId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , workplaceId=" << workplaceId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeWorkplaceInfoByWorkId(userId, workplaceId);
}

//--------------------------------------------------------------
Ice::Int NetworkWriterI::addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addWorkspaceInfo(userId, workspaceInfo);
}

void NetworkWriterI::setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setWorkspaceInfo(userId, id, workspaceInfo);
}

void NetworkWriterI::removeWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeWorkspaceInfo(userId, id);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadCollegeInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadCollegeInfoSeq(userId);
}

Ice::Int NetworkWriterI::addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addCollegeInfo(userId, info);
}

void NetworkWriterI::setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setCollegeInfo(userId, id, info);
}

void NetworkWriterI::removeAllCollegeInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllCollegeInfo(userId);
}

void NetworkWriterI::removeCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeCollegeInfo(userId, id);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadElementarySchoolInfo(userId);
}

void NetworkWriterI::removeAllElementarySchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllElementarySchoolInfo(userId);
}

void NetworkWriterI::removeElementarySchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeElementarySchoolInfoById(userId, id);
}

void NetworkWriterI::setElementarySchoolInfo(int userId, int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setElementarySchoolInfo(userId, id, info);
}

Ice::Int NetworkWriterI::addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addElementarySchoolInfo(userId, info);
}

void NetworkWriterI::removeElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeElementarySchoolInfo(userId);
}

//--------------------------------------------------------------
void NetworkWriterI::removeAllJuniorHighSchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllJuniorHighSchoolInfo(userId);
}

void NetworkWriterI::removeJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeJuniorHighSchoolInfoById(userId, id);
}

void NetworkWriterI::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setJuniorHighSchoolInfo(userId, id, data);
}

void NetworkWriterI::reloadJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadJuniorHighSchoolInfo(userId);
}

Ice::Int NetworkWriterI::addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addJuniorHighSchoolInfo(userId, info);
}

void NetworkWriterI::removeJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeJuniorHighSchoolInfo(userId);
}

//--------------------------------------------------------------
void NetworkWriterI::reloadRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().reloadRegionInfo(userId);
}

Ice::Int NetworkWriterI::addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return UserNetworkAdapter::instance().addRegionInfo(userId, regionInfo);
}

void NetworkWriterI::removeRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeRegionInfo(userId, regionId);
}

void NetworkWriterI::removeAllRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().removeAllRegionInfo(userId);
}

void NetworkWriterI::setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNetworkAdapter::instance().setRegionInfo(userId, regionId, regionInfo);
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

	//为了保持接口不变，UserNetwork的不提供reloadAll接口，
	//另一方面UserNetwork中所有reload方法的效果都一致，所以随便调用一个reload
	UserNetworkAdapter::instance().reloadRegionInfo(userId);
}

