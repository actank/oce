#ifndef __EDM_FEED_EDM_SENDER__
#define __EDM_FEED_EDM_SENDER__

#include <Util.ice>

module xce {
module edm {

interface FeedEdmSender {
  void Receive(int userId, MyUtil::Str2StrMap params);
	void ReceiveForPhone(int userId, MyUtil::Str2StrMap params);
  void FlushCache();

	//---------------------FOR DEBUG---------------------------
	void NoCheckReceive(int userid, MyUtil::Str2StrMap params);
	void ClearFrequencyCacheByUser(int userid);
};

};
};
#endif
