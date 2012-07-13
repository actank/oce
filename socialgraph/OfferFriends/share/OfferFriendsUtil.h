#ifndef OFFERFRIENDSUTIL_H_
#define OFFERFRIENDSUTIL_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ObjectCacheI.h"
#include "Util.h"
#include "QueryRunner.h"
#include <FriendFinder.h>
#include <FriendFinderInfoCache.h>
#include <FriendFinderByIP.h>
#include <IPRecord.h>

namespace xce {
namespace offerfriends {

class OfferFriendsUtil {
public:
	/*@brief 获取用户的好友列表*/
	static MyUtil::IntSeq getFriendSeq(int userId);

	static MyUtil::Int2IntSeqMap getFriendSeqBatch(const MyUtil::IntSeq& ids);
	static MyUtil::Int2IntMap getFriendCountBatch(const MyUtil::IntSeq& ids);

	/*@brief 获取用户的发送好友申请的数据*/
	static MyUtil::IntSeq getApplySeq(int userId);

	/*@brief 获取用户的黑名单列表*/
	static MyUtil::IntSeq getBlockSeq(int userId);

	/*@brief 获取用户的发送好友申请的数据*/
	static void getUserLoginMap(const std::vector<int>& idlist, std::map<int, long>& userLoginMap);

	/*@brief 获取用户的common block列表*/
  static MyUtil::IntSeq getCommBlockSeq(int userId);
	static com::xiaonei::service::CommonFriendSeq getCommonFriend(int userId, int commLimit);
	static com::xiaonei::service::CommonFriendSeq calCommonFriend(int userId, const MyUtil::IntSeq& applyList, const MyUtil::IntSeq& friendList, const MyUtil::IntSeq& blockList, const MyUtil::IntSeq& commBlockList, int commLimit);
	static void rebuildFriendFinder(int userId);
	static xce::friendfinder::IPNoteSeq getFreqIP(int userId, int ipLimit);
  static MyUtil::IntSeq getUsersWithMaskByIP(Ice::Long ip,Ice::Int limit,const MyUtil::IntSeq& mask);
  static xce::friendfinder::NetworkInfo getNetworkInfoWithHostage(int userId, int resultSize, const std::vector<int>& friendsList);
  static xce::friendfinder::NetworkInfo getNetworkInfoWithHostageFromDB(int userId, int resultSize, const std::vector<int>& friendsList);
  static xce::friendfinder::HighSchoolInfo getHighSchoolInfo(int userId, int resultSize, const std::vector<int>& friendsList);

  static MyUtil::IntSeq getPhotoIdsFrom(const MyUtil::IntSeq& ids);
	static std::map<int,bool> getPhotoStateBatch(const MyUtil::IntSeq& ids);
	static std::map<int,bool> getStatusBatch(const MyUtil::IntSeq& ids);
	static std::map<int,bool> getStateStarBatch(const MyUtil::IntSeq& ids);
	static std::map<int,bool> getStateGuideBatch(const MyUtil::IntSeq& ids);
	static std::map<int,bool> getOnlineStateBatch(const MyUtil::IntSeq& ids);
	static std::map<int,int> getViewCountMap(const MyUtil::IntSeq& ids);
private:
  static void filterHostFriends(int userId, std::vector<int>& infoFriends, const std::vector<int>& friendsList);
  static void filterHostFriendsForHighSchool(int userId, xce::friendfinder::RecommendItemSeq& itemseq, 
																						 const std::vector<int>& friendsList);

	/*brief 排序中学用户，按最近登录时间进行排序,最近登录的优先*/
	static void SortSameDistrictRecommend(int userId, xce::friendfinder::HighSchoolInfo& highSchoolInfo);
	static void SortByUserLoginTime(int userId, xce::friendfinder::RecommendItemSeq& itemseq);

	/*brief 按照中学用户性别推荐比列，区分推荐用户,异性比例提高至70%*/
	static bool GetRecommendByRate(int userId, int resultSize, xce::friendfinder::RecommendItemSeq& itemseq);
	static std::map<int, int> GetUserGender(const std::vector<int>& ids);
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

//***************************************************************************************************************

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

//***************************************************************************************************************

class ViewCountHandler: public com::xiaonei::xce::ResultHandler {
public:
	ViewCountHandler(std::map<int, int>& viewCountMap): 
		_viewCountMap(viewCountMap) {}
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::map<int, int>& _viewCountMap;
};

//***************************************************************************************************************

class UserLoginHandler: public com::xiaonei::xce::ResultHandler {
public:
	UserLoginHandler(std::map<int, long>& userLoginMap):
		userLoginMap_(userLoginMap) {}

	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<int, long>& userLoginMap_;
};

//***************************************************************************************************************

class UserGenderHandler: public com::xiaonei::xce::ResultHandler {
public:
	UserGenderHandler(std::map<int, int>& user_gender_map):
		user_gender_map_(user_gender_map) { }
	
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<int, int>& user_gender_map_;
};
} 
}

#endif
