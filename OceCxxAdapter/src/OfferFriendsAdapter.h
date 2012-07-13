#ifndef __OFFER_FRIENDS_ADAPTER_H__
#define __OFFER_FRIENDS_ADAPTER_H__

#include "OfferFriends.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace offerfriends{
namespace adapter{

using namespace MyUtil;
using namespace ::xce::offerfriends;

class OfferFriendsCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<OfferFriendsCacheNManagerPrx>, public MyUtil::Singleton<OfferFriendsCacheAdapter> {
public:
	OfferFriendsCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI <OfferFriendsCacheNManagerPrx>("ControllerOfferFriendsCacheN", 120, 1000) {
	}

	OfferDataNSeq getFastOffer(int userId, int limit);
	OfferDataNSeq getBestOffer(int userId, int limit);
	OfferDataNSeq getRandomOffer(int userId, int limit);

	OfferDataNSeq getBestOfferByStage(int stage, int userId, int limit);

	RmInfo apply(int applicant, int acceptor);
	RmInfo accept(int acceptor, int applicant);
	RmInfo denyOffer(int hostId, int guestId);
	RmInfo ignoreOffer(int hostId, int guestId);

        void load(Ice::Int,const Ice::ObjectPtr&);
	void setData(const MyUtil::ObjectResultPtr&);
};

class OfferFriendsLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<OfferFriendsLoaderNPrx>, public MyUtil::Singleton<OfferFriendsLoaderAdapter> {
public:
        OfferFriendsLoaderAdapter() :
                MyUtil::ReplicatedClusterAdapterI <OfferFriendsLoaderNPrx>("ControllerOfferFriendsLoaderN", 120, 300) {
        }

	void apply(int applicant, int acceptor);
	void accept(int acceptor, int applicant);
	void denyOffer(int hostId, int guestId);
	void ignoreOffers(int userId, const MyUtil::IntSeq& ids);

	void report(GetInfo info);

        void rebuild(Ice::Int,bool);
        void rebuildBatch(const MyUtil::IntSeq&,bool);
	std::string debug(Ice::Int);
        void reload(Ice::Int);
        void reloadBatch(const MyUtil::IntSeq&);
};

}
}
}
#endif

