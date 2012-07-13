#ifndef __USERNETWORK_ADAPTER_H__
#define __USERNETWORK_ADAPTER_H__

#include "UserNetwork.h"
#include "Channel.h"
#include "Singleton.h"
#include "DataWrapperI.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

class UserNetworkAdapter: public ReplicatedClusterAdapterI<NetworkManagerPrx> , public Singleton<UserNetworkAdapter> {
public:
	UserNetworkAdapter() :
		ReplicatedClusterAdapterI<NetworkManagerPrx> ("ControllerUserNetwork", 120, 300) {
	}

	//以下get方法兼容NetworkManager提供的接口
	NetworkBigObjectNPtr getBigObjectN(int userId);
	HighSchoolInfoSeq getHighSchoolInfoSeq(int userId);
	NetworkManagerPrx getNetworkManager(int userId);
	NetworkManagerPrx getNetworkManagerOneway(int userId);

	//以下get方法为checker提供
	NetworkDataPtr getNetworkFullData(int userId);

	//以下reload/add/remove/set方法供UserNetWorkWriter(agent)调用
	void reloadNetworkSeq(Ice::Int userId);
	Ice::Int addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network);
	void setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network);
	void removeNetwork(Ice::Int userId, Ice::Int networkId);
	void removeNetworkByStage(Ice::Int userId, Ice::Int stage);

	void reloadHighSchoolInfoSeq(Ice::Int userId);
	Ice::Int addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& HighSchoolInfo);
	void setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo);
	void removeHighSchoolInfo(Ice::Int userId, Ice::Int id);
	void removeAllHighSchoolInfo(Ice::Int userId);

	void reloadUniversityInfoSeq(Ice::Int userId);
	Ice::Int addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo);
	void setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo);
	void removeUniversityInfo(Ice::Int userId, Ice::Int id);
	void removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId);
	void removeAllUniversityInfo(Ice::Int userId);

	void reloadWorkplaceInfoSeq(Ice::Int userId);
	Ice::Int addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo);
	void setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo);
	void removeWorkplaceInfo(Ice::Int userId, Ice::Int id);
	void removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workId);
	void removeAllWorkplaceInfo(Ice::Int userId);

	Ice::Int addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo);
	void setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo);
	void removeWorkspaceInfo(Ice::Int userId, Ice::Int id);

	void reloadCollegeInfoSeq(Ice::Int userId);
	Ice::Int addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info);
	void setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info);
	void removeCollegeInfo(Ice::Int userId, Ice::Int id);
	void removeAllCollegeInfo(Ice::Int userId);

	void reloadElementarySchoolInfo(Ice::Int userId);
	Ice::Int addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info);
	void removeElementarySchoolInfo(Ice::Int userId);
	void removeAllElementarySchoolInfo(Ice::Int userId);
	void removeElementarySchoolInfoById(Ice::Int userId, int id);
	void setElementarySchoolInfo(Ice::Int userId, int id, const MyUtil::Str2StrMap& data);

	void setJuniorHighSchoolInfo(int userid, int id, const MyUtil::Str2StrMap& data);
	void removeAllJuniorHighSchoolInfo(int userid);
	void removeJuniorHighSchoolInfoById(int userid, int id);
	void reloadJuniorHighSchoolInfo(Ice::Int userId);
	Ice::Int addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info);
	void removeJuniorHighSchoolInfo(Ice::Int userId);

	void reloadRegionInfo(Ice::Int userId);
	Ice::Int addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo);
	void removeRegionInfo(Ice::Int userId, Ice::Int regionId);
	void removeAllRegionInfo(Ice::Int userId);
	void setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo);

private:
};

}
;
}
;
}
;
}
;

#endif

