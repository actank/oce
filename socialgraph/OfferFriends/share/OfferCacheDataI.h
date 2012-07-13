#ifndef __OFFER_CACHE_DATA_I_H__
#define __OFFER_CACHE_DATA_I_H__

#include "OfferFriends.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <iostream>

namespace xce {
namespace offerfriends {

	class OfferCacheDataI: virtual public ::xce::offerfriends::OfferCacheDataN {
	public:
		OfferCacheDataI();
		OfferCacheDataI(const ::xce::offerfriends::OfferCacheDataNPtr& obj);
	public:
		MyUtil::IntSeq getIds();
		OfferDataNSeq getFast(GetInfo&,int limit = -1);
		OfferDataNSeq getBest(GetInfo&,const std::map<int,bool>&,int limit = -1);
		OfferDataNSeq getRandom(GetInfo&,int limit = -1);
	private:
		OfferDataNSeq doRandom(const OfferDataNSeq&,int);
		OfferDataNSeq doOnlineFilter(GetInfo&,const OfferDataNSeq&,int,const std::map<int,bool>&);
	public:
		void remove(RmInfo&,int);
	private:
		IceUtil::Mutex _mutex;
		int _timeStamp;
	};
	typedef IceUtil::Handle<OfferCacheDataI> OfferCacheDataIPtr;

}
}

#endif
