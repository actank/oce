#ifndef __USER_NETWORK_READER_I_H__
#define __USER_NETWORK_READER_I_H__

#include "UserNetwork.h"
#include "DataSet.h"
#include "ObjectCacheI.h"
#include "../share/UserNetworkDataI.h"
#include "../share/UserNetworkDataFactory.h"
#include "UserNetworkWriterAdapter.h"

#include <QueryRunner.h>
#include "util/cpp/TimeCost.h"
namespace mop {
namespace hi {
namespace svc {
namespace model {
using namespace MyUtil;
using namespace mop::hi::svc::adapter;

const int NETWORK_DATA = 0;
const string NETWORK_READER = "NR";
const int TASK_LEVEL_SYNCCREATE = 10;
const int TASK_LEVEL_FILL = 11;
const long MAX_TIMEOUT = 300;

//--------------------------------------------------------------
class NetworkReaderI: virtual public NetworkReader {
public:

	NetworkReaderI();
	virtual ~NetworkReaderI();

	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());

	virtual NetworkPtr getNetwork(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual NetworkPtr getRegionalNetwork(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual NetworkPtr getNetworkByNetId(Ice::Int userId, Ice::Int networkId, const Ice::Current& = Ice::Current());
	virtual NetworkSeq getNetworkSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual NetworkSeq getNetworkSeqByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& = Ice::Current());
	virtual NetworkSeq getNetworkSeqByStatus(Ice::Int userId, Ice::Int status, const Ice::Current& = Ice::Current());
	virtual NetworkSeq getNetworkSeqByStageAndStatus(Ice::Int userId, Ice::Int stage, const MyUtil::IntSeq& status, const Ice::Current& = Ice::Current());
	virtual NetworkSeq getNetworkSeqByStatuses(Ice::Int userId, const MyUtil::IntSeq& status, const Ice::Current& = Ice::Current());
	virtual void addNetwork(Ice::Int userId, const MyUtil::Str2StrMap& network, const Ice::Current& = Ice::Current());
	virtual void setNetwork(Ice::Int userId, Ice::Int networkId, const MyUtil::Str2StrMap& network, const Ice::Current& = Ice::Current());
	virtual void removeNetwork(Ice::Int userId, Ice::Int networkId, const Ice::Current& = Ice::Current());
	virtual void removeNetworkByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& = Ice::Current());
	virtual Ice::Int getNetworkCountByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& = Ice::Current());
	virtual int getValidNetworkCountByStage(Ice::Int userId, Ice::Int stage, const Ice::Current& = Ice::Current());
	virtual bool isExistInNetwork(Ice::Int userid, Ice::Int networkid, const Ice::Current& = Ice::Current());

	virtual HighSchoolInfoPtr getHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual HighSchoolInfoPtr getHighSchoolInfoBySchoolId(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual HighSchoolInfoSeq getHighSchoolInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& = Ice::Current());
	virtual void setHighSchoolInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& HighSchoolInfo, const Ice::Current& = Ice::Current());
	virtual void removeHighSchoolInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void removeAllHighSchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual Ice::Int getHighSchoolInfoCount(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual UniversityInfoPtr getUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual UniversityInfoPtr getUniversityInfoByUnivId(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual UniversityInfoSeq getUniversityInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addUniversityInfo(Ice::Int userId, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& = Ice::Current());
	virtual void setUniversityInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& universityInfo, const Ice::Current& = Ice::Current());
	virtual void removeUniversityInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void removeUniversityInfoByUnivId(Ice::Int userId, Ice::Int universityId, const Ice::Current& = Ice::Current());
	virtual void removeAllUniversityInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual Ice::Int getUniversityInfoCount(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual WorkplaceInfoPtr getWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual WorkplaceInfoPtr getWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual WorkplaceInfoSeq getWorkplaceInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addWorkplaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& = Ice::Current());
	virtual void setWorkplaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workplaceInfo, const Ice::Current& = Ice::Current());
	virtual void removeWorkplaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void removeWorkplaceInfoByWorkId(Ice::Int userId, Ice::Int workId, const Ice::Current& = Ice::Current());
	virtual void removeAllWorkplaceInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual Ice::Int getWorkplaceInfoCount(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual WorkspaceInfoPtr getWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual WorkspaceInfoPtr getWorkspaceInfoByWorkId(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual WorkspaceInfoSeq getWorkspaceInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addWorkspaceInfo(Ice::Int userId, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& = Ice::Current());
	virtual void setWorkspaceInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& workspaceInfo, const Ice::Current& = Ice::Current());
	virtual void removeWorkspaceInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());

	virtual CollegeInfoPtr getCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual CollegeInfoPtr getCollegeInfoByCollegeId(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual CollegeInfoSeq getCollegeInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addCollegeInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& = Ice::Current());
	virtual void setCollegeInfo(Ice::Int userId, Ice::Int id, const MyUtil::Str2StrMap& info, const Ice::Current& = Ice::Current());
	virtual void removeCollegeInfo(Ice::Int userId, Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void removeAllCollegeInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual Ice::Int getCollegeInfoCount(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual ElementarySchoolInfoPtr getElementarySchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addElementarySchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& = Ice::Current());
	virtual void removeElementarySchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void removeAllElementarySchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void removeElementarySchoolInfoById(Ice::Int userId, int id, const Ice::Current& = Ice::Current());
	virtual void setElementarySchoolInfo(Ice::Int userId, int id, const MyUtil::Str2StrMap& data, const Ice::Current& = Ice::Current());
	virtual ElementarySchoolInfoSeq getElementarySchoolInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual ElementarySchoolInfoPtr getElementarySchoolInfoById(Ice::Int userId, int id, const Ice::Current& = Ice::Current());
	virtual ElementarySchoolInfoPtr getElementarySchoolInfoBySchoolId(Ice::Int userId, int schoolId, const Ice::Current& = Ice::Current());

	virtual JuniorHighSchoolInfoPtr getJuniorHighSchoolInfoById(int userId, int id, const Ice::Current& = Ice::Current());
	virtual JuniorHighSchoolInfoPtr getJuniorHighSchoolInfoBySchoolId(int userId, int id, const Ice::Current& = Ice::Current());
	virtual JuniorHighSchoolInfoSeq getJuniorHighSchoolInfoSeq(int userId, const Ice::Current& = Ice::Current());
	virtual void setJuniorHighSchoolInfo(int userid, int id, const MyUtil::Str2StrMap& data, const Ice::Current& = Ice::Current());
	virtual void removeAllJuniorHighSchoolInfo(int userid, const Ice::Current& = Ice::Current());
	virtual void removeJuniorHighSchoolInfoById(int userid, int id, const Ice::Current& = Ice::Current());
	virtual JuniorHighSchoolInfoPtr getJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addJuniorHighSchoolInfo(Ice::Int userId, const MyUtil::Str2StrMap& info, const Ice::Current& = Ice::Current());
	virtual void removeJuniorHighSchoolInfo(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual RegionInfoPtr getRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& = Ice::Current());
	virtual RegionInfoSeq getRegionInfoSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void addRegionInfo(Ice::Int userId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& = Ice::Current());
	virtual void removeRegionInfo(Ice::Int userId, Ice::Int regionId, const Ice::Current& = Ice::Current());
	virtual void removeAllRegionInfo(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual void setRegionInfo(Ice::Int userId, Ice::Int regionId, const MyUtil::Str2StrMap& regionInfo, const Ice::Current& = Ice::Current());

	virtual NetworkBigObjectPtr getBigObject(Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual NetworkBigObjectNPtr getBigObjectN(Ice::Int userId, const Ice::Current& = Ice::Current());

	virtual NetworkDataPtr getNetworkFullData(Ice::Int userId, const Ice::Current& = Ice::Current());

	bool isValid();
private:

	NetworkCacheDataPtr getNetworkCacheData(int userId);

	NetworkDataIPtr getNetworkData(int userId);

	IceUtil::Mutex _mutex;
	bool _isValid;

};

class CreateTask: public MyUtil::Task {
public:
	CreateTask(::Ice::Int userId, IceUtil::Monitor<IceUtil::Mutex>& mutex, NetworkCacheDataPtr& data) :
		MyUtil::Task(TASK_LEVEL_SYNCCREATE), _userId(userId), _mutex(mutex), _data(data) {
		memset(&_createTime, 0, sizeof(timeval));
		gettimeofday(&_createTime, NULL);
	}
protected:
	virtual void handle() {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		timeval timeNow;
		memset(&timeNow, 0, sizeof(timeval));
		gettimeofday(&timeNow, NULL);
		long minusTime = (timeNow.tv_sec * 1000 - _createTime.tv_sec * 1000) + (timeNow.tv_usec / 1000 - _createTime.tv_usec / 1000);

		if (minusTime > MAX_TIMEOUT) {
			ostringstream inMsg;

			inMsg << "CreateTask::handle schedule over time userId = " << _userId;

			TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_DEBUG);

			_mutex.notify();

			NetworkCacheDataPtr dataI = ServiceI::instance().findObject<NetworkCacheDataPtr> (NETWORK_DATA, _userId);
			if (!dataI) {
				ServiceI::instance().locateObject<NetworkCacheDataPtr> (NETWORK_DATA, _userId);
				UserNetworkWriterAdapter::instance().reloadAll(_userId);
			}
		} else {
			ostringstream inMsg;

			inMsg << "CreateTask::handle userId = " << _userId;

			TimeCost tc = TimeCost::create(inMsg.str(), TimeCost::LOG_LEVEL_DEBUG);

			_data = ServiceI::instance().findObject<NetworkCacheDataPtr> (NETWORK_DATA, _userId);
			if (!_data) {
				_data = ServiceI::instance().locateObject<NetworkCacheDataPtr> (NETWORK_DATA, _userId);
				UserNetworkWriterAdapter::instance().reloadAll(_userId);
			}
			_mutex.notify();
		}
	}
private:
	::Ice::Int _userId;
	IceUtil::Monitor<IceUtil::Mutex>& _mutex;
	NetworkCacheDataPtr& _data;
	timeval _createTime;
};

class FillTask: virtual public ::MyUtil::Task {
public:
        FillTask(const ::MyUtil::ObjectResultPtr& userData): MyUtil::Task(TASK_LEVEL_FILL),
			     _userData(userData)
	{
	}
        virtual void handle()
		{
			MCE_INFO("[FillTask::handle]" << "  set Data size : " << _userData->data.size());
	
    ObjectMap results;

	try{
		for (ObjectMap::const_iterator it = _userData->data.begin(); it != _userData->data.end(); ++it) {
			NetworkDataIPtr rs = new NetworkDataI(NetworkDataPtr::dynamicCast(it->second));

			NetworkCacheDataPtr res = new NetworkCacheData(rs->serialize());

			results.insert(make_pair(it->first, res));

		}
		MyUtil::ServiceI::instance().getObjectCache()->addObjects(NETWORK_DATA, results);
	}catch(Ice::Exception& e)
	{
		MCE_WARN("NetworkReaderI::setData Exception : " << e.what());
	}
		}

private:
        ::MyUtil::ObjectResultPtr _userData;
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

