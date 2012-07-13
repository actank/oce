#ifndef __APNS_ICE__
#define __APNS_ICE__
#include <Util.ice>
module xce{
module notify{
module apns{
	class APNSManager{
		//wap
		void addUser(int userId, string token);
		//notify
		void addIphoneNotifyBySeq(MyUtil::IntSeq userIds, string msg);
		//pusher
		//void removeUserByToken(string token);
		//test
		//void showId();
	};
};
};
};
#endif

