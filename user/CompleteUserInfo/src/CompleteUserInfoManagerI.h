#ifndef __COMPLETE_USER_INFO_MANAGERI_H__
#define __COMPLETE_USER_INFO_MANAGERI_H__

#include <string>
#include <map>
#include "CompleteUserInfo.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include <time.h>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/TopicI.h"
#include "IceExt/src/ServiceI.h"

using namespace std;

namespace xce {
	namespace completeuserinfo {

		#define DEFAULT_REGISTER_TIME time(NULL)
		#define DEFAULT_LASTLOGIN_TIME time(NULL)
		#define DEFAULT_LASTLOGOUT_TIME time(NULL)
		const string DEFAULT_MSN = "";	
		const int DEFAULT_STAR = 0;
		const string DEFAULT_NAME = "";
		const string DEFAULT_HEAD_URL = "0/0/men_head.gif";
		const string DEFAULT_MAIN_URL = "0/0/men_main.gif";
		const string DEFAULT_LARGE_URL = "0/0/men_main.gif";
		const string DEFAULT_TINY_URL = "0/0/men_tiny.gif";
		const int DEFAULT_STAGE = 0;
		const int DEFAULT_BASIC_CONFIG = 3;
		const int DEFAULT_POKE_CONFIG = 3;
		const int DEFAULT_WANT_SEE_CSS = 2;
		const string DEFAULT_RECV_FEED_CONFIG = "555555555555515";
		const int DEFAULT_BROWSE_CONFIG = 99;
		const int DEFAULT_REQUEST_FRIEND_CONFIG = 99;
		const int DEFAULT_EMAIL_CONFIG = 262137;
		const string DEFAULT_PROFILE_PRIVACY = "{a:3,c:0,e:3,f:0,g:3,j:0,k:0,0:0,m:0,p:3,q:0,s:0,t:0,w:3,h:0,n:0,d:0,i:0}";

		const int CHECK_USERINFO_SIZE = 11;
	
		class CompleteUserInfoManagerI : virtual public CompleteUserInfoManager, public MyUtil::Singleton<CompleteUserInfoManagerI> {
			public:
				CompleteUserInfoManagerI() {
					init();
				}
				virtual void completeUserInfo(const vector<int>& userIds, const Ice::Current& = Ice::Current());
				virtual void setValid(bool valid, const Ice::Current& ctx = Ice::Current());
				virtual bool isValid(const Ice::Current& ctx = Ice::Current());
			private:
				void init();
				void createDefaultUserName(int userId);
				void createDefaultUserRight(int userId);
				void createDefaultUserState(int userId);
				void createDefaultUserUrl(int userId);
				void createDefaultUserBorn(int userId);
				void createDefaultUserStage(int userId);
				void createDefaultUserDoing(int userId) ;
				void createDefaultUserTime(int userId);
				void createDefaultUserContact(int userId);
				void createDefaultUserConfig(int userId);
				bool getUserName(int userId);
				bool getUserRight(int userId);
				bool getUserState(int userId);
				bool getUserUrl(int userId);
				bool getUserBorn(int userId);
				bool getUserStage(int userId);
				bool getUserDoing(int userId) ;
				bool getUserTime(int userId);
				bool getUserContact(int userId);
				bool getUserConfig(int userId);
				bool getUserPassport(int userId);

				struct FpIndex { void (CompleteUserInfoManagerI::*createfp_)(int userId); 
								 bool (CompleteUserInfoManagerI::*getfp_)(int); string name_; };
				vector<FpIndex> fpIndex_;
				bool valid_;
				IceUtil::RWRecMutex validMutex_;
		};
	}
}

#endif

