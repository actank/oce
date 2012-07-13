#ifndef _ONLINE_EVENT_MEDIATOR_ICE_
#define _ONLINE_EVENT_MEDIATOR_ICE_

#include <Util.ice>

module xce {
module mediator {
module onlineevent{
	const int OFFLINE = 0;

	const int ONLINEWEBPAGGER = 1;
	const int ONLINEXNT = 2;
	const int ONLINEWAP = 3;

	const int OFFLINEWEBPAGGER = -1;
	const int OFFLINEXNT = -2;
	const int OFFLINEWAP = -3;

	interface OnlineEventMediatorManager {
		void sendOnlineEventSingle(int userId, int onlineType);
		void sendOnlineEventBatch(MyUtil::Int2IntMap data);
	};
};
};
};
#endif
