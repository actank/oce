#include "UserNetworkReaderI.h"
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

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(NETWORK_DATA, "N", new UserNetworkDataFactory, false);

	NetworkReaderPtr reader = new NetworkReaderI;
	service.getAdapter()->add(reader, service.createIdentity(NETWORK_READER, ""));

	TaskManager::instance().config(TASK_LEVEL_SYNCCREATE, MyUtil::ThreadPoolConfig(1, 5));
	TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 5));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkReader.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkReader.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerUserNetworkReader", reader, mod, interval);
	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserNetworkReader.Status", 0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os << "MyUtil::initialize set status : " << status;
		MCE_WARN( os.str() );
	}
	ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------
NetworkReaderI::NetworkReaderI() : _isValid(false) {
}

NetworkReaderI::~NetworkReaderI() {
}

//--------------------------------------------------------------
NetworkDataIPtr NetworkReaderI::getNetworkData(int userId) {
	NetworkCacheDataPtr rs = getNetworkCacheData(userId);
	if (rs) {
		return rs->getData();
	}
	return NULL;
}

NetworkCacheDataPtr NetworkReaderI::getNetworkCacheData(int userId) {
	NetworkCacheDataPtr dataI = ServiceI::instance().findObject<NetworkCacheDataPtr> (NETWORK_DATA, userId);
	if (!dataI) {
		dataI = ServiceI::instance().locateObject<NetworkCacheDataPtr> (NETWORK_DATA, userId);
		if(isValid())
		{
			UserNetworkWriterAdapter::instance().reloadAll(userId);
		}
	}
	return dataI;
}

//--------------------------------------------------------------
NetworkPtr NetworkReaderI::getNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << "  networkId=" << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->networkId == networkId) {
			return *it;
		}
	}
	return NULL;
}

NetworkPtr NetworkReaderI::getRegionalNetwork(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if (((*it)->networkId >= 60000000) && ((*it)->networkId <= 70000000) && (((*it)->status == 0) or ((*it)->status == -1))) {
			return *it;
		}
	}
	return NULL;
}

NetworkPtr NetworkReaderI::getNetworkByNetId(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
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

NetworkSeq NetworkReaderI::getNetworkSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->networks;
}

NetworkSeq NetworkReaderI::getNetworkSeqByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
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

NetworkSeq NetworkReaderI::getNetworkSeqByStatus(Ice::Int userId, Ice::Int status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , status = " << status;
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

NetworkSeq NetworkReaderI::getNetworkSeqByStatuses(Ice::Int userId, const MyUtil::IntSeq& status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
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

NetworkSeq NetworkReaderI::getNetworkSeqByStageAndStatus(Ice::Int userId, Ice::Int stage, const MyUtil::IntSeq& status, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , stage=" << stage;
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

void NetworkReaderI::addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(network, NETWORKINFO);
}

void NetworkReaderI::setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , networkId = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(networkId, network, NETWORKINFO);
}

void NetworkReaderI::removeNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , networkId = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(networkId, NETWORKINFO, INFOTYPEID);
}

void NetworkReaderI::removeNetworkByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , stage = " << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(stage, NETWORKINFO, INFOSTAGE);
}

Ice::Int NetworkReaderI::getNetworkCountByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , stage = " << stage;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkSeqByStage(userId, stage).size();
}

Ice::Int NetworkReaderI::getValidNetworkCountByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , stage = " << stage;
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

bool NetworkReaderI::isExistInNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , networkid = " << networkId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkSeq data = getNetworkData(userId)->networks;

	for (NetworkSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->networkId == networkId && (*it)->status <= 0) {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
HighSchoolInfoPtr NetworkReaderI::getHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	for (HighSchoolInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return *it;
		}
	}
	return NULL;
}

HighSchoolInfoPtr NetworkReaderI::getHighSchoolInfoBySchoolId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	for (HighSchoolInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->HighSchoolId == id) {
			return *it;
		}
	}
	return NULL;
}

HighSchoolInfoSeq NetworkReaderI::getHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	HighSchoolInfoSeq data = getNetworkData(userId)->highschools;

	return data;
}

void NetworkReaderI::addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& highSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(highSchoolInfo, HIGHSCHOOLINFO);
}

void NetworkReaderI::setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(id, HighSchoolInfo, HIGHSCHOOLINFO);
}

void NetworkReaderI::removeAllHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, HIGHSCHOOLINFO, INFOALL);
}

void NetworkReaderI::removeHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << ", id" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, HIGHSCHOOLINFO, INFOID);
}

Ice::Int NetworkReaderI::getHighSchoolInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->highschools.size();
}

//--------------------------------------------------------------
UniversityInfoPtr NetworkReaderI::getUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;

	for (UniversityInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return *it;
		}
	}
	return NULL;
}

UniversityInfoPtr NetworkReaderI::getUniversityInfoByUnivId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;

	for (UniversityInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->universityId == id) {
			return *it;
		}
	}
	return NULL;
}

UniversityInfoSeq NetworkReaderI::getUniversityInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	UniversityInfoSeq data = getNetworkData(userId)->universities;

	return getNetworkData(userId)->universities;
}

void NetworkReaderI::addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(universityInfo, UNIVERSITYINFO);
}

void NetworkReaderI::setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(id, universityInfo, UNIVERSITYINFO);
}

void NetworkReaderI::removeAllUniversityInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, UNIVERSITYINFO, INFOALL);
}

void NetworkReaderI::removeUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, UNIVERSITYINFO, INFOID);
}

void NetworkReaderI::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " universityId=" << universityId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(universityId, UNIVERSITYINFO, INFOTYPEID);
}

Ice::Int NetworkReaderI::getUniversityInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->universities.size();
}

//--------------------------------------------------------------
WorkplaceInfoPtr NetworkReaderI::getWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return WorkplaceInfoPtr::dynamicCast(getWorkspaceInfo(userId, id));
}

WorkplaceInfoPtr NetworkReaderI::getWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return WorkplaceInfoPtr::dynamicCast(getWorkspaceInfoByWorkId(userId, id, current));
}

WorkplaceInfoSeq NetworkReaderI::getWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getWorkspaceInfoSeq(userId);

	WorkplaceInfoSeq res;

	for (WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		res.push_back(WorkplaceInfoPtr::dynamicCast(*it));
	}
	return res;
}

void NetworkReaderI::addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	addWorkspaceInfo(userId, workplaceInfo);
}

void NetworkReaderI::setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	setWorkspaceInfo(userId, id, workplaceInfo);
}

//--------------------------------------------------------------
void NetworkReaderI::removeAllWorkplaceInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, WORKSPACEINFO, INFOALL);
}

void NetworkReaderI::removeWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, WORKSPACEINFO, INFOID);
}

void NetworkReaderI::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workplaceId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , workplaceId=" << workplaceId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	removeWorkspaceInfo(userId, workplaceId);
}

Ice::Int NetworkReaderI::getWorkplaceInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->workspaces.size();
}

WorkspaceInfoPtr NetworkReaderI::getWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getNetworkData(userId)->workspaces;

	for (WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return *it;
		}
	}

	return NULL;
}

WorkspaceInfoPtr NetworkReaderI::getWorkspaceInfoByWorkId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	WorkspaceInfoSeq data = getNetworkData(userId)->workspaces;

	for (WorkspaceInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->workplaceId == id) {
			return *it;
		}
	}

	return NULL;
}

WorkspaceInfoSeq NetworkReaderI::getWorkspaceInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->workspaces;
}

void NetworkReaderI::addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(workspaceInfo, WORKSPACEINFO);
}

void NetworkReaderI::setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(id, workspaceInfo, WORKSPACEINFO);
}

void NetworkReaderI::removeWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, WORKSPACEINFO, INFOTYPEID);
}

//--------------------------------------------------------------
CollegeInfoPtr NetworkReaderI::getCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CollegeInfoSeq data = getNetworkData(userId)->colleges;

	for (CollegeInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->id == id) {
			return (*it);
		}
	}
	return NULL;
}

CollegeInfoPtr NetworkReaderI::getCollegeInfoByCollegeId(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	CollegeInfoSeq data = getNetworkData(userId)->colleges;

	for (CollegeInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->collegeId == id) {
			return (*it);
		}
	}

	return 0;
}

CollegeInfoSeq NetworkReaderI::getCollegeInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->colleges;
}

void NetworkReaderI::addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(info, COLLEGEINFO);
}

void NetworkReaderI::setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(id, info, COLLEGEINFO);
}

void NetworkReaderI::removeAllCollegeInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, COLLEGEINFO, INFOALL);
}

void NetworkReaderI::removeCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, COLLEGEINFO, INFOTYPEID);
}

Ice::Int NetworkReaderI::getCollegeInfoCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId)->colleges.size();
}

//--------------------------------------------------------------
ElementarySchoolInfoPtr NetworkReaderI::getElementarySchoolInfoById(Ice::Int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	ElementarySchoolInfoSeq seq = getNetworkData(userId)->elementaries;
	for (ElementarySchoolInfoSeq::iterator i = seq.begin(); i != seq.end(); ++i) {
		if (ElementarySchoolInfoNPtr::dynamicCast(*i)->id == id) {
			return *i;
		}
	}
	return NULL;
}

ElementarySchoolInfoPtr NetworkReaderI::getElementarySchoolInfoBySchoolId(Ice::Int userId, int id, const Ice::Current& current) {
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

ElementarySchoolInfoSeq NetworkReaderI::getElementarySchoolInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->elementaries;
}

void NetworkReaderI::removeAllElementarySchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, ELEMENTARYSCHOOLINFO, INFOALL);
}

void NetworkReaderI::removeElementarySchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, ELEMENTARYSCHOOLINFO, INFOID);
}

void NetworkReaderI::setElementarySchoolInfo(int userId, int id, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(id, info, ELEMENTARYSCHOOLINFO);
}

ElementarySchoolInfoPtr NetworkReaderI::getElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	ElementarySchoolInfoSeq seq = getNetworkData(userId)->elementaries;

	if (seq.empty()) {
		return 0;
	}
	return seq[0];
}

void NetworkReaderI::addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(info, ELEMENTARYSCHOOLINFO);
}

void NetworkReaderI::removeElementarySchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, ELEMENTARYSCHOOLINFO, INFOALL);
}

//--------------------------------------------------------------
void NetworkReaderI::removeAllJuniorHighSchoolInfo(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, JUNIORHIGHSCHOOLINFO, INFOALL);
}

void NetworkReaderI::removeJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(id, JUNIORHIGHSCHOOLINFO, INFOID);
}

JuniorHighSchoolInfoPtr NetworkReaderI::getJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	JuniorHighSchoolInfoSeq seq = getNetworkData(userId)->juniorhighschools;
	for (JuniorHighSchoolInfoSeq::iterator i = seq.begin(); i != seq.end(); ++i) {
		if (JuniorHighSchoolInfoNPtr::dynamicCast(*i)->id == id) {
			return *i;
		}
	}
	return NULL;
}

JuniorHighSchoolInfoPtr NetworkReaderI::getJuniorHighSchoolInfoBySchoolId(int userId, int id, const Ice::Current& current) {
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

JuniorHighSchoolInfoSeq NetworkReaderI::getJuniorHighSchoolInfoSeq(int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->juniorhighschools;
}

void NetworkReaderI::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " id=" << id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(id, data, JUNIORHIGHSCHOOLINFO);
}

JuniorHighSchoolInfoPtr NetworkReaderI::getJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	JuniorHighSchoolInfoSeq data = getNetworkData(userId)->juniorhighschools;

	if (data.empty()) {
		return 0;
	}
	return data[0];
}

void NetworkReaderI::addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(info, JUNIORHIGHSCHOOLINFO);
}

void NetworkReaderI::removeJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, JUNIORHIGHSCHOOLINFO, INFOALL);
}

//--------------------------------------------------------------
RegionInfoPtr NetworkReaderI::getRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	RegionInfoSeq data = getNetworkData(userId)->regions;
	for (RegionInfoSeq::const_iterator it = data.begin(); it != data.end(); ++it) {
		if ((*it)->regionId == regionId) {
			return *it;
		}
	}
	return NULL;
}

RegionInfoSeq NetworkReaderI::getRegionInfoSeq(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	return getNetworkData(userId)->regions;
}

void NetworkReaderI::addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->addInfo(regionInfo, REGIONINFO);
}

void NetworkReaderI::removeRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(regionId, REGIONINFO, INFOTYPEID);
}

void NetworkReaderI::removeAllRegionInfo(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->removeInfo(userId, REGIONINFO, INFOALL);
}

void NetworkReaderI::setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "userId=" << userId << " , regionId=" << regionId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	getNetworkCacheData(userId)->setInfo(regionId, regionInfo, REGIONINFO);
}

//--------------------------------------------------------------
NetworkBigObjectPtr NetworkReaderI::getBigObject(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	NetworkDataIPtr source = getNetworkData(userId);

	NetworkBigObjectPtr res = new NetworkBigObject();
	try {
		res->networks = source->networks;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObject Exception: get network err");
	}
	try {
		res->highschools = source->highschools;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObject Exception: get highschool info err");
	}

	try {
		res->universitys = source->universities;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObject Exception: get university info err");
	}
	try {

		res->workspaces = source->workspaces;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObject Exception: get workspace info err");
	}
	return res;
}

NetworkBigObjectNPtr NetworkReaderI::getBigObjectN(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	if (userId <= 0) {
		MCE_WARN("NetworkReaderI::getBigObjectN userId = " << userId );
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
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get network err");
	}
	try {
		res->highschools = source->highschools;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get highschool info err");
	}

	try {
		res->universitys = source->universities;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get university info err");
	}
	try {
		res->workspaces = source->workspaces;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get workspace info err");
	}
	try {
		res->collages = source->colleges;
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get college info err");
	}
	try {
		if (!source->elementaries.empty()) {
			res->elementary = source->elementaries.at(0);
		}
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get elementary info err");
	}
	try {
		if (!source->juniorhighschools.empty()) {
			res->juniorhighschool = source->juniorhighschools.at(0);
		}
	} catch (...) {
		MCE_WARN("NetworkReaderI::getBigObjectN Exception: get junior_high_school info err");
	}
	return res;
}

NetworkDataPtr NetworkReaderI::getNetworkFullData(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id = " << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return getNetworkData(userId);
}

//--------------------------------------------------------------
bool NetworkReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}

void NetworkReaderI::setValid(bool valid, const ::Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = valid;
	}
	ServerStateSubscriber::instance().setStatus((int) valid);
}

void NetworkReaderI::setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "size = " << userData->data.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  MyUtil::TaskManager::instance().execute(new FillTask(userData));
}
