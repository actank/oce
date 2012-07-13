#ifndef __USER_NETWORK_WRITER_I_H__
#define __USER_NETWORK_WRITER_I_H__

#include "UserNetwork.h"
#include "DataSet.h"
#include "ObjectCacheI.h"
#include "../share/UserNetworkDataI.h"

#include <QueryRunner.h>
#include "util/cpp/TimeCost.h"
namespace mop
{
namespace hi
{
namespace svc
{
namespace model
{
using namespace MyUtil;

const string NETWORK_WRITER = "NW";
const int TASK_LEVEL_SYNCCREATE = 10;

//----------------------------------------------------------------------------
class NetworkWriterI : virtual public NetworkWriter
{
public:
	NetworkWriterI();
	virtual ~NetworkWriterI();

	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current()); 
	
	virtual void reloadNetworkSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addNetwork(Ice::Int userId,const MyUtil::Str2StrMap& network ,const Ice::Current& = Ice::Current());
    virtual void setNetwork(Ice::Int userId,Ice::Int networkId ,const MyUtil::Str2StrMap& network ,const Ice::Current& = Ice::Current());
    virtual void removeNetwork(Ice::Int userId, Ice::Int networkId ,const Ice::Current& = Ice::Current());
    virtual void removeNetworkByStage(Ice::Int userId, Ice::Int stage ,const Ice::Current& = Ice::Current());

    virtual void reloadHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& HighSchoolInfo ,const Ice::Current& = Ice::Current());
    virtual void setHighSchoolInfo(Ice::Int userId,Ice::Int id , const MyUtil::Str2StrMap& HighSchoolInfo ,const Ice::Current& = Ice::Current());
    virtual void removeHighSchoolInfo(Ice::Int userId, Ice::Int id ,const Ice::Current& = Ice::Current());
    virtual void removeAllHighSchoolInfo(Ice::Int userId,const Ice::Current& = Ice::Current());

    virtual void reloadUniversityInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addUniversityInfo(Ice::Int userId,const MyUtil::Str2StrMap& universityInfo ,const Ice::Current& = Ice::Current());
    virtual void setUniversityInfo(Ice::Int userId,Ice::Int id ,const MyUtil::Str2StrMap& universityInfo ,const Ice::Current& = Ice::Current());
    virtual void removeUniversityInfo(Ice::Int userId, Ice::Int id ,const Ice::Current& = Ice::Current());
    virtual void removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId, const Ice::Current& = Ice::Current());
    virtual void removeAllUniversityInfo(Ice::Int userId ,const Ice::Current& = Ice::Current());

    virtual void reloadWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addWorkplaceInfo(Ice::Int userId,const MyUtil::Str2StrMap& workplaceInfo ,const Ice::Current& = Ice::Current());
    virtual void setWorkplaceInfo(Ice::Int userId,Ice::Int id ,const MyUtil::Str2StrMap& workplaceInfo ,const Ice::Current& = Ice::Current());
    virtual void removeWorkplaceInfo(Ice::Int userId, Ice::Int id ,const Ice::Current& = Ice::Current());
    virtual void removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workId ,const Ice::Current& = Ice::Current());
    virtual void removeAllWorkplaceInfo(Ice::Int userId ,const Ice::Current& = Ice::Current());

    virtual Ice::Int addWorkspaceInfo(Ice::Int userId,const MyUtil::Str2StrMap& workspaceInfo ,const Ice::Current& = Ice::Current());
    virtual void setWorkspaceInfo(Ice::Int userId,Ice::Int id ,const MyUtil::Str2StrMap& workspaceInfo ,const Ice::Current& = Ice::Current());
    virtual void removeWorkspaceInfo(Ice::Int userId, Ice::Int id ,const Ice::Current& = Ice::Current());

    virtual void reloadCollegeInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addCollegeInfo(Ice::Int userId,const MyUtil::Str2StrMap& info ,const Ice::Current& = Ice::Current());
    virtual void setCollegeInfo(Ice::Int userId,Ice::Int id ,const MyUtil::Str2StrMap& info ,const Ice::Current& = Ice::Current());
    virtual void removeCollegeInfo(Ice::Int userId, Ice::Int id ,const Ice::Current& = Ice::Current());
    virtual void removeAllCollegeInfo(Ice::Int userId ,const Ice::Current& = Ice::Current());

    virtual void reloadElementarySchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addElementarySchoolInfo(Ice::Int userId,const MyUtil::Str2StrMap& info ,const Ice::Current& = Ice::Current());
    virtual void removeElementarySchoolInfo(Ice::Int userId ,const Ice::Current& = Ice::Current());
    virtual void removeAllElementarySchoolInfo(Ice::Int userId ,const Ice::Current& = Ice::Current());
    virtual void removeElementarySchoolInfoById(Ice::Int userId ,int id, const Ice::Current& = Ice::Current());
    virtual void setElementarySchoolInfo(Ice::Int userId ,int id, const MyUtil::Str2StrMap& data, const Ice::Current& = Ice::Current());

    virtual void setJuniorHighSchoolInfo(int userid, int id, const MyUtil::Str2StrMap& data, const Ice::Current& = Ice::Current());
    virtual void removeAllJuniorHighSchoolInfo(int userid, const Ice::Current& = Ice::Current());
    virtual void removeJuniorHighSchoolInfoById(int userid, int id, const Ice::Current& = Ice::Current());
    virtual void reloadJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addJuniorHighSchoolInfo(Ice::Int userId,const MyUtil::Str2StrMap& info ,const Ice::Current& = Ice::Current());
    virtual void removeJuniorHighSchoolInfo(Ice::Int userId ,const Ice::Current& = Ice::Current());

    virtual void reloadRegionInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual Ice::Int addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& = Ice::Current());
    virtual void removeRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& = Ice::Current());
    virtual void removeAllRegionInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual void setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& = Ice::Current());

    //加载详细信息
    virtual void reloadAll(Ice::Int userId, const Ice::Current& = Ice::Current());
private:

};

}
}
}
}
#endif

