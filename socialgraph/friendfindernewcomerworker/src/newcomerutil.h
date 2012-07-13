#ifndef _NEWCOMER_UTIL_H_
#define _NEWCOMER_UTIL_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ObjectCacheI.h"
#include "Util.h"
#include "QueryRunner.h"
//#include <FriendFinder.h>
#include <FriendFinderInfoCache.h>
#include <FriendFinderByIP.h>
#include <IPRecord.h>

namespace xce {
namespace socialgraph {
namespace friendfindernewcomer {

class NewComerUtil {
public:
	static MyUtil::IntSeq getFriendSeq(int userId);
	static MyUtil::Int2IntSeqMap getFriendSeqBatch(const MyUtil::IntSeq& ids);
	static MyUtil::Int2IntMap getFriendCountBatch(const MyUtil::IntSeq& ids);
	static MyUtil::IntSeq getApplySeq(int userId);
	static MyUtil::IntSeq getBlockSeq(int userId);
        static MyUtil::IntSeq getCommBlockSeq(int userId);
/*
	static com::xiaonei::service::CommonFriendSeq getCommonFriend(int userId, int commLimit);
	static void rebuildFriendFinder(int userId);
*/
	static xce::friendfinder::IPNoteSeq getFreqIP(int userId, int ipLimit);
        static MyUtil::IntSeq getUsersWithMaskByIP(Ice::Long ip,Ice::Int limit,const MyUtil::IntSeq& mask);
        static xce::friendfinder::NetworkInfo getNetworkInfoWithHostage(int userId, int resultSize, const std::vector<int>& friendsList);
        static xce::friendfinder::NetworkInfo getNetworkInfoWithHostageFromDB(int userId, int resultSize, const std::vector<int>& friendsList);
        static MyUtil::IntSeq getUnivData(int userId, const MyUtil::StrSeq& condition);
        static MyUtil::IntSeq getHighschoolData(int userId, const MyUtil::StrSeq& condition);
        static MyUtil::IntSeq getJuniorschoolData(int userId, const MyUtil::StrSeq& condition);
        static MyUtil::IntSeq getPhotoIdsFrom(const MyUtil::IntSeq& ids);
	static std::map<int,bool> getPhotoStateBatch(const MyUtil::IntSeq& ids);
	static bool isPhotoStateTrue(int userId );
	static std::map<int,bool> getStatusBatch(const MyUtil::IntSeq& ids);
	static bool isStatusTrue(int userId );
	static std::map<int,bool> getStateStarBatch(const MyUtil::IntSeq& ids);
	static bool isStateStarTrue(int userId );
	static std::map<int,bool> getStateGuideBatch(const MyUtil::IntSeq& ids);
	static bool isStateGuideTrue(int userId );
	static std::map<int,bool> getOnlineStateBatch(const MyUtil::IntSeq& ids);
	static bool isOnlineStateTrue(int userId );
	static std::map<int,int> getViewCountMap(const MyUtil::IntSeq& ids);
	static std::string getSchoolName(int userId,int schoolId);
private:
        static void filterHostFriends(int userId, std::vector<int>& infoFriends, const std::vector<int>& friendsList);
};

//***************************************************************************************************************

class ApplyResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	ApplyResultHandler(MyUtil::IntSeq& result): _result(result) {}
	~ApplyResultHandler();

protected:
	virtual bool handle(mysqlpp::Row& row) const;
	
private:
	MyUtil::IntSeq& _result;
};

class BlockResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BlockResultHandler(MyUtil::IntSeq& results, const std::string& toField);
        virtual ~BlockResultHandler();
protected:
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::IntSeq& _results;
        std::string _toField;
};

class ViewCountHandler: public com::xiaonei::xce::ResultHandler {
public:
	ViewCountHandler(std::map<int, int>& viewCountMap): 
		_viewCountMap(viewCountMap) {}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::map<int, int>& _viewCountMap;
};

} 
}
}

#endif
