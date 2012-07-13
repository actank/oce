#include "UserNetworkReaderAdapter.h"

using namespace mop::hi::svc::adapter;
using namespace std;

// 以下get方法兼容NetworkManager提供的接口
//--------------------------------------------------------------
NetworkReaderPrx UserNetworkReaderAdapter::getNetworkReader(int id) {
	return getProxy(id);
}

NetworkReaderPrx UserNetworkReaderAdapter::getNetworkReaderOneway(int id) {
	return getProxyOneway(id);
}

//oldwr中不再保留缓存，oldwr的读全由reader提供，为oldwr增设所有的get接口
//--------------------------------------------------------------
NetworkPtr UserNetworkReaderAdapter::getNetwork(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetwork(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetwork Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetwork std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetwork UNKNOWN exception id = " << userId);
	}
	return NULL;
}

NetworkPtr UserNetworkReaderAdapter::getRegionalNetwork(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getRegionalNetwork(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionalNetwork Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionalNetwork std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionalNetwork UNKNOWN exception id = " << userId);
	}
	return NULL;
}

NetworkPtr UserNetworkReaderAdapter::getNetworkByNetId(Ice::Int userId, Ice::Int networkId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkByNetId(userId, networkId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkByNetId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkByNetId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkByNetId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

NetworkSeq UserNetworkReaderAdapter::getNetworkSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeq UNKNOWN exception id = " << userId);
	}
	return NetworkSeq();
}

NetworkSeq UserNetworkReaderAdapter::getNetworkSeqByStage(Ice::Int userId, Ice::Int stage) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkSeqByStage(userId, stage);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStage Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStage std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStage UNKNOWN exception id = " << userId);
	}
	return NetworkSeq();
}

NetworkSeq UserNetworkReaderAdapter::getNetworkSeqByStatus(Ice::Int userId, Ice::Int status) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkSeqByStatus(userId, status);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStatus Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStatus std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStatus UNKNOWN exception id = " << userId);
	}
	return NetworkSeq();
}

NetworkSeq UserNetworkReaderAdapter::getNetworkSeqByStageAndStatus(Ice::Int userId, Ice::Int stage, const MyUtil::IntSeq& status) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkSeqByStageAndStatus(userId, stage, status);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStageAndStatus Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStageAndStatus std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStageAndStatus UNKNOWN exception id = " << userId);
	}
	return NetworkSeq();
}

NetworkSeq UserNetworkReaderAdapter::getNetworkSeqByStatuses(Ice::Int userId, const MyUtil::IntSeq& status) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkSeqByStatuses(userId, status);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStatuses Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStatuses std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkSeqByStatuses UNKNOWN exception id = " << userId);
	}
	return NetworkSeq();
}

Ice::Int UserNetworkReaderAdapter::getNetworkCountByStage(Ice::Int userId, Ice::Int stage) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkCountByStage(userId, stage);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkCountByStage Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkCountByStage std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkCountByStage UNKNOWN exception id = " << userId);
	}
	return 0;
}

int UserNetworkReaderAdapter::getValidNetworkCountByStage(Ice::Int userId, Ice::Int stage) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getValidNetworkCountByStage(userId, stage);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getValidNetworkCountByStage Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getValidNetworkCountByStage std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getValidNetworkCountByStage UNKNOWN exception id = " << userId);
	}
	return 0;
}

bool UserNetworkReaderAdapter::isExistInNetwork(Ice::Int userId, Ice::Int networkId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->isExistInNetwork(userId, networkId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::isExistInNetwork Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::isExistInNetwork std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::isExistInNetwork UNKNOWN exception id = " << userId);
	}
	return false;
}

//--------------------------------------------------------------
HighSchoolInfoPtr UserNetworkReaderAdapter::getHighSchoolInfo(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getHighSchoolInfo(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

HighSchoolInfoPtr UserNetworkReaderAdapter::getHighSchoolInfoBySchoolId(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getHighSchoolInfoBySchoolId(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoBySchoolId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoBySchoolId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoBySchoolId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

HighSchoolInfoSeq UserNetworkReaderAdapter::getHighSchoolInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getHighSchoolInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoSeq UNKNOWN exception id = " << userId);
	}
	return HighSchoolInfoSeq();
}

Ice::Int UserNetworkReaderAdapter::getHighSchoolInfoCount(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getHighSchoolInfoCount(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoCount Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoCount std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getHighSchoolInfoCount UNKNOWN exception id = " << userId);
	}
	return 0;
}

//--------------------------------------------------------------
UniversityInfoPtr UserNetworkReaderAdapter::getUniversityInfo(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getUniversityInfo(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

UniversityInfoPtr UserNetworkReaderAdapter::getUniversityInfoByUnivId(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getUniversityInfoByUnivId(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoByUnivId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoByUnivId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoByUnivId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

UniversityInfoSeq UserNetworkReaderAdapter::getUniversityInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getUniversityInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoSeq UNKNOWN exception id = " << userId);
	}
	return UniversityInfoSeq();
}

Ice::Int UserNetworkReaderAdapter::getUniversityInfoCount(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getUniversityInfoCount(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoCount Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoCount std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getUniversityInfoCount UNKNOWN exception id = " << userId);
	}
	return 0;
}

//--------------------------------------------------------------
WorkplaceInfoPtr UserNetworkReaderAdapter::getWorkplaceInfo(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkplaceInfo(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

WorkplaceInfoPtr UserNetworkReaderAdapter::getWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkplaceInfoByWorkId(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoByWorkId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoByWorkId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoByWorkId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

WorkplaceInfoSeq UserNetworkReaderAdapter::getWorkplaceInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkplaceInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoSeq UNKNOWN exception id = " << userId);
	}
	return WorkplaceInfoSeq();
}

Ice::Int UserNetworkReaderAdapter::getWorkplaceInfoCount(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkplaceInfoCount(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoCount Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoCount std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkplaceInfoCount UNKNOWN exception id = " << userId);
	}
	return 0;
}

//--------------------------------------------------------------
WorkspaceInfoPtr UserNetworkReaderAdapter::getWorkspaceInfo(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkspaceInfo(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

WorkspaceInfoPtr UserNetworkReaderAdapter::getWorkspaceInfoByWorkId(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkspaceInfoByWorkId(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfoByWorkId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfoByWorkId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfoByWorkId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

WorkspaceInfoSeq UserNetworkReaderAdapter::getWorkspaceInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getWorkspaceInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getWorkspaceInfoSeq UNKNOWN exception id = " << userId);
	}
	return WorkspaceInfoSeq();
}

//--------------------------------------------------------------
CollegeInfoPtr UserNetworkReaderAdapter::getCollegeInfo(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getCollegeInfo(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

CollegeInfoPtr UserNetworkReaderAdapter::getCollegeInfoByCollegeId(Ice::Int userId, Ice::Int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getCollegeInfoByCollegeId(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoByCollegeId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoByCollegeId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoByCollegeId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

CollegeInfoSeq UserNetworkReaderAdapter::getCollegeInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getCollegeInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoSeq UNKNOWN exception id = " << userId);
	}
	return CollegeInfoSeq();
}

Ice::Int UserNetworkReaderAdapter::getCollegeInfoCount(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getCollegeInfoCount(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoCount Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoCount std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getCollegeInfoCount UNKNOWN exception id = " << userId);
	}
	return 0;
}

//--------------------------------------------------------------
ElementarySchoolInfoPtr UserNetworkReaderAdapter::getElementarySchoolInfo(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getElementarySchoolInfo(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

ElementarySchoolInfoSeq UserNetworkReaderAdapter::getElementarySchoolInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getElementarySchoolInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoSeq UNKNOWN exception id = " << userId);
	}
	return ElementarySchoolInfoSeq();
}

ElementarySchoolInfoPtr UserNetworkReaderAdapter::getElementarySchoolInfoById(Ice::Int userId, int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getElementarySchoolInfoById(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoById Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoById std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoById UNKNOWN exception id = " << userId);
	}
	return NULL;
}

ElementarySchoolInfoPtr UserNetworkReaderAdapter::getElementarySchoolInfoBySchoolId(Ice::Int userId, int schoolId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getElementarySchoolInfoBySchoolId(userId, schoolId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoBySchoolId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoBySchoolId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getElementarySchoolInfoBySchoolId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

//--------------------------------------------------------------
JuniorHighSchoolInfoPtr UserNetworkReaderAdapter::getJuniorHighSchoolInfoById(int userId, int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getJuniorHighSchoolInfoById(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoById Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoById std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoById UNKNOWN exception id = " << userId);
	}
	return NULL;
}

JuniorHighSchoolInfoPtr UserNetworkReaderAdapter::getJuniorHighSchoolInfoBySchoolId(int userId, int id) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getJuniorHighSchoolInfoBySchoolId(userId, id);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoBySchoolId Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoBySchoolId std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoBySchoolId UNKNOWN exception id = " << userId);
	}
	return NULL;
}

JuniorHighSchoolInfoSeq UserNetworkReaderAdapter::getJuniorHighSchoolInfoSeq(int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getJuniorHighSchoolInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfoSeq UNKNOWN exception id = " << userId);
	}
	return JuniorHighSchoolInfoSeq();
}

JuniorHighSchoolInfoPtr UserNetworkReaderAdapter::getJuniorHighSchoolInfo(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getJuniorHighSchoolInfo(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getJuniorHighSchoolInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

//--------------------------------------------------------------
RegionInfoPtr UserNetworkReaderAdapter::getRegionInfo(Ice::Int userId, Ice::Int regionId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getRegionInfo(userId, regionId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionInfo Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionInfo std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionInfo UNKNOWN exception id = " << userId);
	}
	return NULL;
}

RegionInfoSeq UserNetworkReaderAdapter::getRegionInfoSeq(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getRegionInfoSeq(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionInfoSeq Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionInfoSeq std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getRegionInfoSeq UNKNOWN exception id = " << userId);
	}
	return RegionInfoSeq();
}

//--------------------------------------------------------------
NetworkBigObjectPtr UserNetworkReaderAdapter::getBigObject(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getBigObject(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getBigObject Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getBigObject std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getBigObject UNKNOWN exception id = " << userId);
	}
	return NULL;
}

NetworkBigObjectNPtr UserNetworkReaderAdapter::getBigObjectN(Ice::Int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getBigObjectN(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getBigObjectN Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getBigObjectN std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getBigObjectN UNKNOWN exception id = " << userId);
	}
	return NULL;
}

NetworkDataPtr UserNetworkReaderAdapter::getNetworkFullData(int userId) {
	NetworkReaderPrx prx = getNetworkReader(userId);
	try {
		if (prx) {
			return prx->getNetworkFullData(userId);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkFullData Ice::Exception id = " << userId << " " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkFullData std::exception id = " << userId << " " << e.what());
	} catch (...) {
		MCE_WARN("UserNetworkReaderAdapter::getNetworkFullData UNKNOWN exception id = " << userId);
	}
	return NULL;
}

// 以下add/remove/set/setData方法仅供NetworkWriter调用
//--------------------------------------------------------------
void UserNetworkReaderAdapter::addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addNetwork(userId, network);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addNetwork Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setNetwork(userId, networkId, network);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setNetwork Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeNetwork(Ice::Int userId, Ice::Int networkId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeNetwork(userId, networkId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeNetwork Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeNetworkByStage(Ice::Int userId, Ice::Int stage) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeNetworkByStage(userId, stage);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeNetworkByStage Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& HighSchoolInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addHighSchoolInfo(userId, HighSchoolInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setHighSchoolInfo(userId, id, HighSchoolInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeHighSchoolInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeHighSchoolInfo(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllHighSchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllHighSchoolInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addUniversityInfo(userId, universityInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addUniversityInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setUniversityInfo(userId, id, universityInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setUniversityInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeUniversityInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeUniversityInfo(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeUniversityInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeUniversityInfoByUnivId(userId, universityId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeUniversityInfoByUnivId Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllUniversityInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllUniversityInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllUniversityInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addWorkplaceInfo(userId, workplaceInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addWorkplaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setWorkplaceInfo(userId, id, workplaceInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setWorkplaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeWorkplaceInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeWorkplaceInfo(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeWorkplaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeWorkplaceInfoByWorkId(userId, workId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeWorkplaceInfoByWorkId Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllWorkplaceInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllWorkplaceInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllWorkplaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addWorkspaceInfo(userId, workspaceInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addWorkspaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setWorkspaceInfo(userId, id, workspaceInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setWorkspaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeWorkspaceInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeWorkspaceInfo(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeWorkspaceInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addCollegeInfo(userId, info);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addCollegeInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setCollegeInfo(userId, id, info);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setCollegeInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeCollegeInfo(Ice::Int userId, Ice::Int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeCollegeInfo(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeCollegeInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllCollegeInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllCollegeInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllCollegeInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addElementarySchoolInfo(userId, info);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addElementarySchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeElementarySchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeElementarySchoolInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeElementarySchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllElementarySchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllElementarySchoolInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllElementarySchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeElementarySchoolInfoById(Ice::Int userId, int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeElementarySchoolInfoById(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeElementarySchoolInfoById Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setElementarySchoolInfo(Ice::Int userId, int id, const MyUtil::Str2StrMap& data) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setElementarySchoolInfo(userId, id, data);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setElementarySchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setJuniorHighSchoolInfo(userId, id, data);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setJuniorHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllJuniorHighSchoolInfo(int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllJuniorHighSchoolInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllJuniorHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeJuniorHighSchoolInfoById(int userId, int id) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeJuniorHighSchoolInfoById(userId, id);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeJuniorHighSchoolInfoById Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addJuniorHighSchoolInfo(userId, info);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addJuniorHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeJuniorHighSchoolInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeJuniorHighSchoolInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeJuniorHighSchoolInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->addRegionInfo(userId, regionInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::addRegionInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeRegionInfo(Ice::Int userId, Ice::Int regionId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeRegionInfo(userId, regionId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeRegionInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::removeAllRegionInfo(Ice::Int userId) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->removeAllRegionInfo(userId);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::removeAllRegionInfo Exception id = " << userId << " " << e.what());
		}
	}
}

void UserNetworkReaderAdapter::setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo) {
	if (userId <= 0) {
		return;
	}
	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setRegionInfo(userId, regionId, regionInfo);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setRegionInfo Exception id = " << userId << " " << e.what());
		}
	}
}

//--------------------------------------------------------------
void UserNetworkReaderAdapter::setData(int userId, const Ice::ObjectPtr& userData) {
	if (userId <= 0 || !userData) {
		return;
	}

	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(userId, userData));

	std::vector<NetworkReaderPrx> allProxies = getAllProxySeq(userId);
	for (std::vector<NetworkReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it) {
		try {
			(*it)->ice_timeout(UNR_TIME_OUT)->setData(res);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserNetworkReaderAdapter::setData Exception id = " << userId << " " << e.what());
		}
	}
}

