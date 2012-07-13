#ifndef __RMESSAGEGATE_ICE__
#define __RMESSAGEGATE_ICE__

#include <Notify.ice>

module xce{
module notify{

class FeedGate;

class RmessageGate extends FeedGate {
	string GetHtmlContent(int uid);
};
};
};

#endif
