#ifndef __USER_NETWORK_READER_ADAPTER_H__
#define __USER_NETWORK_READER_ADAPTER_H__

#include "UserNetwork.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

const int UNR_TIME_OUT = 10000;

class UserNetworkReaderAdapter : public ReplicatedClusterAdapterI<NetworkReaderPrx>,
	public Singleton<UserNetworkReaderAdapter> {
public:
	UserNetworkReaderAdapter() : ReplicatedClusterAdapterI<NetworkReaderPrx>("ControllerUserNetworkReader",120,300){
	}

	// 以下get方法兼容NetworkManager提供的接口
	//--------------------------------------------------------------
	NetworkReaderPrx getNetworkReader(int userId);
	NetworkReaderPrx getNetworkReaderOneway(int userId);

	//oldwr中不再保留缓存，oldwr的读全由reader提供，为oldwr增设所有的get接口
	//--------------------------------------------------------------
	NetworkPtr getNetwork(Ice::Int userId, Ice::Int id);
	NetworkPtr getRegionalNetwork(Ice::Int userId);
	NetworkPtr getNetworkByNetId(Ice::Int userId, Ice::Int networkId);
	NetworkSeq getNetworkSeq(Ice::Int userId);
	NetworkSeq getNetworkSeqByStage(Ice::Int userId, Ice::Int stage);
	NetworkSeq getNetworkSeqByStatus(Ice::Int userId, Ice::Int status);
	NetworkSeq getNetworkSeqByStageAndStatus(Ice::Int userId, Ice::Int stage, const MyUtil::IntSeq& status);
	NetworkSeq getNetworkSeqByStatuses(Ice::Int userId, const MyUtil::IntSeq& status);
	Ice::Int getNetworkCountByStage(Ice::Int userId, Ice::Int stage);
	int getValidNetworkCountByStage(Ice::Int userId, Ice::Int stage);
	bool isExistInNetwork(Ice::Int userId, Ice::Int networkId);

	HighSchoolInfoPtr getHighSchoolInfo(Ice::Int userId, Ice::Int id);
	HighSchoolInfoPtr getHighSchoolInfoBySchoolId(Ice::Int userId, Ice::Int id);
	HighSchoolInfoSeq getHighSchoolInfoSeq(Ice::Int userId);
	Ice::Int getHighSchoolInfoCount(Ice::Int userId);

	UniversityInfoPtr getUniversityInfo(Ice::Int userId, Ice::Int id);
	UniversityInfoPtr getUniversityInfoByUnivId(Ice::Int userId, Ice::Int id);
	UniversityInfoSeq getUniversityInfoSeq(Ice::Int userId);
	Ice::Int getUniversityInfoCount(Ice::Int userId);

	WorkplaceInfoPtr getWorkplaceInfo(Ice::Int userId, Ice::Int id);
	WorkplaceInfoPtr getWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int id);
	WorkplaceInfoSeq getWorkplaceInfoSeq(Ice::Int userId);
	Ice::Int getWorkplaceInfoCount(Ice::Int userId);

	WorkspaceInfoPtr getWorkspaceInfo(Ice::Int userId, Ice::Int id);
	WorkspaceInfoPtr getWorkspaceInfoByWorkId(Ice::Int userId, Ice::Int id);
	WorkspaceInfoSeq getWorkspaceInfoSeq(Ice::Int userId);

	CollegeInfoPtr getCollegeInfo(Ice::Int userId, Ice::Int id);
	CollegeInfoPtr getCollegeInfoByCollegeId(Ice::Int userId, Ice::Int id);
	CollegeInfoSeq getCollegeInfoSeq(Ice::Int userId);
	Ice::Int getCollegeInfoCount(Ice::Int userId);

	ElementarySchoolInfoPtr getElementarySchoolInfo(Ice::Int userId);
	ElementarySchoolInfoSeq getElementarySchoolInfoSeq(Ice::Int userId);
	ElementarySchoolInfoPtr getElementarySchoolInfoById(Ice::Int userId, int id);
	ElementarySchoolInfoPtr getElementarySchoolInfoBySchoolId(Ice::Int userId, int schoolId);

	JuniorHighSchoolInfoPtr getJuniorHighSchoolInfoById(int userId, int id);
	JuniorHighSchoolInfoPtr getJuniorHighSchoolInfoBySchoolId(int userId, int id);
	JuniorHighSchoolInfoSeq getJuniorHighSchoolInfoSeq(int userId);
	JuniorHighSchoolInfoPtr getJuniorHighSchoolInfo(Ice::Int userId);

	RegionInfoPtr getRegionInfo(Ice::Int userId, Ice::Int regionId);
	RegionInfoSeq getRegionInfoSeq(Ice::Int userId);

	NetworkBigObjectPtr getBigObject(Ice::Int userId);
	NetworkBigObjectNPtr getBigObjectN(Ice::Int userId);
	NetworkDataPtr getNetworkFullData(int userId);

	// 以下add/remove/set/setData方法仅供NetworkWriter调用
	//--------------------------------------------------------------
	void addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network);
	void setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network);
	void removeNetwork(Ice::Int userId, Ice::Int networkId);
	void removeNetworkByStage(Ice::Int userId, Ice::Int stage);

	void addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& HighSchoolInfo);
	void setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo);
	void removeHighSchoolInfo(Ice::Int userId, Ice::Int id);
	void removeAllHighSchoolInfo(Ice::Int userId);

	void addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo);
	void setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo);
	void removeUniversityInfo(Ice::Int userId, Ice::Int id);
	void removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId);
	void removeAllUniversityInfo(Ice::Int userId);

	void addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo);
	void setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo);
	void removeWorkplaceInfo(Ice::Int userId, Ice::Int id);
	void removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workId);
	void removeAllWorkplaceInfo(Ice::Int userId);

	void addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo);
	void setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo);
	void removeWorkspaceInfo(Ice::Int userId, Ice::Int id);

	void addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info);
	void setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info);
	void removeCollegeInfo(Ice::Int userId, Ice::Int id);
	void removeAllCollegeInfo(Ice::Int userId);

	void addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info);
	void removeElementarySchoolInfo(Ice::Int userId);
	void removeAllElementarySchoolInfo(Ice::Int userId);
	void removeElementarySchoolInfoById(Ice::Int userId, int id);
	void setElementarySchoolInfo(Ice::Int userId, int id, const MyUtil::Str2StrMap& data);

	void setJuniorHighSchoolInfo(int userId, int id, const MyUtil::Str2StrMap& data);
	void removeAllJuniorHighSchoolInfo(int userId);
	void removeJuniorHighSchoolInfoById(int userId, int id);
	void addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info);
	void removeJuniorHighSchoolInfo(Ice::Int userId);

	void addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo);
	void removeRegionInfo(Ice::Int userId, Ice::Int regionId);
	void removeAllRegionInfo(Ice::Int userId);
	void setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo);

	void setData(int userId, const Ice::ObjectPtr& userData);
};

}
}
}
}

#endif

