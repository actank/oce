#ifndef REST_MEDIATOR_ICE
#define REST_MEDIATOR_ICE

#include <RestLogic.ice>

module talk
{
module rest
{
	class RestMediatorManager{
		void restRequest(long reqId, int userid, string path, MyUtil::Str2StrMap paras);
		//void useImNotify(int userid);
		//void updateUserDoing(int userid, string doing);
	};
};
};

#endif
