#ifndef _SOCIALGRAPH_WORKER_ICE_
#define _SOCIALGRAPH_WORKER_ICE_

#include <SocialGraphMessage.ice>

module xce {
module socialgraph {

interface AutoGrouping {
	Info GetInfo(int userId);
};

};
};

#endif
