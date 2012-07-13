#ifndef __OFFER_LOADER_H__
#define __OFFER_LOADER_H__

#include <IceUtil/IceUtil.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"

namespace xce {
namespace offerfriends {

class OfferFriendsPreloader : virtual public MyUtil::Singleton<OfferFriendsPreloader> {
public:
	OfferFriendsPreloader(){}
	~OfferFriendsPreloader(){}
};

}
}

#endif
