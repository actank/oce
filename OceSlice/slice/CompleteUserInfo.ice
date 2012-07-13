#ifndef __COMPLETE_USER_INFO_ICE__
#define __COMPLETE_USER_INFO_ICE__

#include <Util.ice>

module xce {
	module completeuserinfo {
		interface CompleteUserInfoManager {
			void completeUserInfo(MyUtil::IntSeq userIds);
			void setValid(bool valid);
			bool isValid();
		};
	};
};
#endif
