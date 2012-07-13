/*
 * EDMFeedSender负责是服务对外的主接口，用户登录时的外部服务调用服务，返回user_id对应的EDM
 * manager的逻辑层包括：
 *    1. 用户合法性检测（未来应该包含安全上的检测，暂时只检测id值是否合法）
 *    2. 预防重复登录的cache（登陆时加入，登出时删除）（cache和服务绑定，按照user_id来分流）
 *    3. 用户发送历史检测（包含not_fit_user_storage，时间7天）(cache和服务绑定，按照user_id来分流）
 *    4. 调用FeedEdmCacheAdapter选择适合的edm
 *    6. 发送edm
 */

#ifndef __EDM_FEED_EDM_SENDER_I_H__
#define __EDM_FEED_EDM_SENDER_I_H__ 

#include "TaskManager.h"
#include "ServiceI.h"
#include "FeedEdmSender.h"
#include "FeedEdmCache.h"
#include "ValidateUser.h"
#include "FrequencyValidate.h"
#include "RFeed.h"
namespace xce {
namespace edm {
class FeedEdmSenderI: public FeedEdmSender, public MyUtil::Singleton<FeedEdmSenderI> {
  public:
    FeedEdmSenderI();
		//PC登录
    void Receive(Ice::Int user_id, const MyUtil::Str2StrMap& params ,const Ice::Current& = Ice::Current());
		//phone登录
    void ReceiveForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params ,const Ice::Current& = Ice::Current());
    void FlushCache(const Ice::Current& = Ice::Current());
//    void Test(Ice::Int user_id, const MyUtil::Str2StrMap& params ,const Ice::Current& = Ice::Current());
		void NoCheckReceive(int userid, const MyUtil::Str2StrMap& params, const Ice::Current& = Ice::Current());
		void ClearFrequencyCacheByUser(int userid, const Ice::Current& = Ice::Current());
  private:
		void ProcessReceive(int user_id, const MyUtil::Str2StrMap& params, bool ischeck = true);
    xce::feed::FeedSeedPtr _BuildSeed(const FeedSeedInfoPtr& info, int userid);
    bool _ValidateUser(Ice::Int user_id);
    int _ValidateFrequencyLimit(Ice::Int user_id);
    std::string _GetAreaCode(const std::string& ip);
    std::string _GetAreaCode2P(const std::string& ip);
    FeedUserInfoPtr _GetUserInfo(Ice::Int user_id, int limit_check);
    void _RemoveLogInFootPrint(int user_id);
    //IceUtil::Mutex mutex_;
};

}
}
#endif
