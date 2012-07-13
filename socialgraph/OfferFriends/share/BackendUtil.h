#ifndef __BACKEND_UTIL_H__
#define __BACKEND_UTIL_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <OfferFriends.h>
#include <Singleton.h>

namespace xce {
namespace offerfriends {

//**************************************************************************************************************

class BackendData: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	OfferDataNSeq get(int);
	void add(int, const OfferDataNSeq&);
	void remove(int type, int userId);
public:
	std::map<int, OfferDataNSeq> _typedOfferDataNSeq;
};
typedef IceUtil::Handle<BackendData> BackendDataPtr;

class BackendProducer: public MyUtil::Singleton<BackendProducer>{
public:
  friend class MyUtil::Singleton<BackendProducer>;

	/*@brief 创建用户的BackendData
  * @param userId 用户id
  * @param weights 用户的preference的weights数据，不同type对应的权重 
  * @param applyList 用户的发送好友申请的列表
  * @param friendList 用户的好友列表
  * @param blockList 用户的黑名单
  * @param commBlockList 用户的commBlockList
  * @param createLog 简单log
  * @param createDetail 数据初始化相信log
  */
  BackendDataPtr create(int userId, 
												const map<int,int>& weights,
												const MyUtil::IntSeq& applyList,
												const MyUtil::IntSeq& friendList,
												const MyUtil::IntSeq& blockList,
												const MyUtil::IntSeq& commBlockList,
                				std::ostringstream& createLog,
												std::ostringstream& createDetail);

	/*@brief 创建用户的BackendData 专为高中用户推人使用
  * @param userId 用户id
  * @param weights 用户的preference的weights数据，不同type对应的权重 
  * @param applyList 用户的发送好友申请的列表
  * @param friendList 用户的好友列表
  * @param blockList 用户的黑名单
  * @param commBlockList 用户的commBlockList
  * @param createLog 简单log
  * @param createDetail 数据初始化相信log
  */
  BackendDataPtr createHighSchoolData(int userId, 
												const map<int,int>& weights,
												const MyUtil::IntSeq& applyList,
												const MyUtil::IntSeq& friendList,
												const MyUtil::IntSeq& blockList,
												const MyUtil::IntSeq& commBlockList,
                				std::ostringstream& createLog,
												std::ostringstream& createDetail);
private:
	/*@brief 创建用户的二度好友推荐列表
  * @param userId 用户id
  * @param commFriendLimit 二度好友的数量限额
  * @param weights 用户的preference的weights数据，不同type对应的权重 
  * @param applyList 用户的发送好友申请的列表
  * @param friendList 用户的好友列表
  * @param blockList 用户的黑名单
  * @param commBlockList 用户的commBlockList
  * @param createLog 简单log
  * @param createDetail 数据初始化相信log
  */
	OfferDataNSeq createFromCommonFriend(int userId,
																			 int commFriendLimit,
																			 const std::map<int, int>& weights,
																			 const MyUtil::IntSeq& applyList,
																			 const MyUtil::IntSeq& friendList,
																			 const MyUtil::IntSeq& blockList,
																			 const MyUtil::IntSeq& commBlockList,
																			 std::ostringstream& createLog,
																			 std::ostringstream& createDetail);

	/*@brief 创建用户的同资料推荐列表
  * @param userId 用户id
  * @param infoLimit 同资料推荐的数量限额
  * @param weights 用户的preference的weights数据，不同type对应的权重 
  * @param applyList 用户的发送好友申请的列表
  * @param friendList 用户的好友列表
  * @param blockList 用户的黑名单
  * @param commBlockList 用户的commBlockList
  * @param createLog 简单log
  * @param createDetail 数据初始化相信log
  */
	std::map<int, OfferDataNSeq> createFromSameInfoFriend(int userId,
																												int infoLimit,
																												const std::map<int,int>& weights,
																												const MyUtil::IntSeq& applyList,
																												const MyUtil::IntSeq& friendList,
																												const MyUtil::IntSeq& blockList,
																												const MyUtil::IntSeq& commBlockList,
																												std::ostringstream& createLog,
																												std::ostringstream& createDetail);

	/*@brief 创建用户的同资料推荐列表，专为高中用户使用该接口
  * @param userId 用户id
  * @param infoLimit 同资料推荐的数量限额
  * @param weights 用户的preference的weights数据，不同type对应的权重 
  * @param applyList 用户的发送好友申请的列表
  * @param friendList 用户的好友列表
  * @param blockList 用户的黑名单
  * @param commBlockList 用户的commBlockList
  * @param createLog 简单log
  * @param createDetail 数据初始化相信log
  */
	std::map<int, OfferDataNSeq> createFromSameInfoFriendForHighSchool(int userId,
																																		 int infoLimit,
																																		 const std::map<int,int>& weights,
																																		 const MyUtil::IntSeq& applyList,
																																		 const MyUtil::IntSeq& friendList,
																																		 const MyUtil::IntSeq& blockList,
																																		 const MyUtil::IntSeq& commBlockList,
																																		 std::ostringstream& createLog,
																																		 std::ostringstream& createDetail);


	/*@brief 创建用户的同ip推荐列表，专为高中用户使用该接口
  * @param userId 用户id
  * @param ipFriendList 同ip推荐的数量限额
  * @param weights 用户的preference的weights数据，不同type对应的权重 
  * @param applyList 用户的发送好友申请的列表
  * @param friendList 用户的好友列表
  * @param blockList 用户的黑名单
  * @param commBlockList 用户的commBlockList
  * @param createLog 简单log
  * @param createDetail 数据初始化相信log
  */
	OfferDataNSeq createFromIPFriend(int userId, 
																	 int ipFriendLimit, 
																	 const std::map<int,int>& weights,
																	 const MyUtil::IntSeq& applyList, 
																	 const MyUtil::IntSeq& friendList, 
																	 const MyUtil::IntSeq& blockList, 
																	 const MyUtil::IntSeq& commBlockList,
																	 std::ostringstream& createLog, 
																	 std::ostringstream& createDetail);

};

}
}

#endif
