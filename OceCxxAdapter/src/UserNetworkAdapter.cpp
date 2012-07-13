#include "UserNetworkAdapter.h"
#include <iostream>

using namespace mop::hi::svc::adapter;
using namespace std;

//以下get方法兼容NetworkManager提供的接口
//--------------------------------------------------------------
NetworkManagerPrx UserNetworkAdapter::getNetworkManager(int id) {
	return getProxy(id);
}

NetworkManagerPrx UserNetworkAdapter::getNetworkManagerOneway(int id) {
	return getProxyOneway(id);
}

NetworkBigObjectNPtr UserNetworkAdapter::getBigObjectN(int userId) {
	try {
		return getNetworkManager(userId)->getBigObjectN(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::getBigObjectN] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::getBigObjectN] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::getBigObjectN] UNKNOWN exception");
	}
	return NULL;
}

HighSchoolInfoSeq UserNetworkAdapter::getHighSchoolInfoSeq(int userId) {
	try {
		return getNetworkManager(userId)->getHighSchoolInfoSeq(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::getHighSchoolInfoSeq] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::getHighSchoolInfoSeq] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::getHighSchoolInfoSeq] UNKNOWN exception");
	}
	return HighSchoolInfoSeq();
}

//以下get方法为checker提供
//--------------------------------------------------------------
NetworkDataPtr UserNetworkAdapter::getNetworkFullData(int userId){
	NetworkManagerPrx prx = getNetworkManager(userId);
	try {
		if (prx) {
			return prx->getNetworkFullData(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::getNetworkFullData] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::getNetworkFullData] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::getNetworkFullData] UNKNOWN exception");
	}
	return NULL;
}

//以下reload/add/remove/set方法供UserNetWorkWriter(agent)调用
//--------------------------------------------------------------
void UserNetworkAdapter::reloadNetworkSeq(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadNetworkSeq(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadNetworkSeq] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadNetworkSeq] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadNetworkSeq] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addNetwork(userId, network);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addNetwork] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addNetwork] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setNetwork(userId, networkId, network);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setNetwork] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setNetwork] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeNetwork(Ice::Int userId, Ice::Int networkId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeNetwork(userId, networkId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeNetwork] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeNetwork] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeNetwork] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeNetworkByStage(Ice::Int userId, Ice::Int stage) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeNetworkByStage(userId, stage);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeNetworkByStage] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeNetworkByStage] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeNetworkByStage] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadHighSchoolInfoSeq(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadHighSchoolInfoSeq(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadHighSchoolInfoSeq] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadHighSchoolInfoSeq] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadHighSchoolInfoSeq] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& highSchoolInfo) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addHighSchoolInfo(userId, highSchoolInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addHighSchoolInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setHighSchoolInfo(userId, id, HighSchoolInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setHighSchoolInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllHighSchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllHighSchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllHighSchoolInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeHighSchoolInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeHighSchoolInfo(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeHighSchoolInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadUniversityInfoSeq(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadUniversityInfoSeq(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadUniversityInfoSeq] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadUniversityInfoSeq] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadUniversityInfoSeq] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addUniversityInfo(userId, universityInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addUniversityInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setUniversityInfo(userId, id, universityInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setUniversityInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllUniversityInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllUniversityInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllUniversityInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeUniversityInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeUniversityInfo(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeUniversityInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeUniversityInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeUniversityInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeUniversityInfoByUnivId(userId, universityId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeUniversityInfoByUnivId] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeUniversityInfoByUnivId] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeUniversityInfoByUnivId] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadWorkplaceInfoSeq(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadWorkplaceInfoSeq(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadWorkplaceInfoSeq] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadWorkplaceInfoSeq] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadWorkplaceInfoSeq] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addWorkplaceInfo(userId, workplaceInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addWorkplaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addWorkplaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addWorkplaceInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setWorkplaceInfo(userId, id, workplaceInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setWorkplaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setWorkplaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setWorkplaceInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllWorkplaceInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllWorkplaceInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllWorkplaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllWorkplaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllWorkplaceInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeWorkplaceInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeWorkplaceInfo(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeWorkplaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeWorkplaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeWorkplaceInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workplaceId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeWorkplaceInfoByWorkId(userId, workplaceId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeWorkplaceInfoByWorkId] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeWorkplaceInfoByWorkId] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeWorkplaceInfoByWorkId] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
Ice::Int UserNetworkAdapter::addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addWorkspaceInfo(userId, workspaceInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addWorkspaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addWorkspaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addWorkspaceInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setWorkspaceInfo(userId, id, workspaceInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setWorkspaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setWorkspaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setWorkspaceInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeWorkspaceInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeWorkspaceInfo(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeWorkspaceInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeWorkspaceInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeWorkspaceInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadCollegeInfoSeq(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadCollegeInfoSeq(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadCollegeInfoSeq] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadCollegeInfoSeq] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadCollegeInfoSeq] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addCollegeInfo(userId, info);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addCollegeInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setCollegeInfo(userId, id, info);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setCollegeInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllCollegeInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllCollegeInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllCollegeInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeCollegeInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeCollegeInfo(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeCollegeInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeCollegeInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeCollegeInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadElementarySchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadElementarySchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadElementarySchoolInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllElementarySchoolInfo(int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllElementarySchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllElementarySchoolInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeElementarySchoolInfoById(int userId, int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeElementarySchoolInfoById(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeElementarySchoolInfoById] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeElementarySchoolInfoById] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeElementarySchoolInfoById] UNKNOWN exception");
	}
}

void UserNetworkAdapter::setElementarySchoolInfo(int userId, int id, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setElementarySchoolInfo(userId, id, info);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setElementarySchoolInfo] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addElementarySchoolInfo(userId, info);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addElementarySchoolInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::removeElementarySchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeElementarySchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeElementarySchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeElementarySchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeElementarySchoolInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadJuniorHighSchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadJuniorHighSchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllJuniorHighSchoolInfo(int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllJuniorHighSchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeJuniorHighSchoolInfoById(int userId, int id) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeJuniorHighSchoolInfoById(userId, id);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeJuniorHighSchoolInfoById] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeJuniorHighSchoolInfoById] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeJuniorHighSchoolInfoById] UNKNOWN exception");
	}
}

void UserNetworkAdapter::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setJuniorHighSchoolInfo(userId, id, data);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addJuniorHighSchoolInfo(userId, info);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addJuniorHighSchoolInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::removeJuniorHighSchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeJuniorHighSchoolInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeJuniorHighSchoolInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeJuniorHighSchoolInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeJuniorHighSchoolInfo] UNKNOWN exception");
	}
}

//--------------------------------------------------------------
void UserNetworkAdapter::reloadRegionInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->reloadRegionInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::reloadRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::reloadRegionInfo] UNKNOWN exception");
	}
}

Ice::Int UserNetworkAdapter::addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo) {
	if (userId <= 0) {
		return 0;
	}
	try {
		return getNetworkManager(userId)->addRegionInfo(userId, regionInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::addRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::addRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::addRegionInfo] UNKNOWN exception");
	}
	return 0;
}

void UserNetworkAdapter::removeRegionInfo(Ice::Int userId, Ice::Int regionId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeRegionInfo(userId, regionId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeRegionInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::removeAllRegionInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->removeAllRegionInfo(userId);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::removeAllRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::removeAllRegionInfo] UNKNOWN exception");
	}
}

void UserNetworkAdapter::setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo) {
	if (userId <= 0) {
		return;
	}
	try {
		getNetworkManager(userId)->setRegionInfo(userId, regionId, regionInfo);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserNetworkAdapter::setRegionInfo] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserNetworkAdapter::setRegionInfo] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserNetworkAdapter::setRegionInfo] UNKNOWN exception");
	}
}

