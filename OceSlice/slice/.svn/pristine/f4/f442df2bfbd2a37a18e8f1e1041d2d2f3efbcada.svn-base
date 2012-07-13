#ifndef _PHOTOSTATEFILTER_ICE_
#define _PHOTOSTATEFILTER_ICE_

#include <Util.ice>

module xce {
module photostatefilter {

	dictionary<int,bool> Int2BoolMap;
	interface PhotoStateFilterManager {
		MyUtil::ByteSeq getPhotoStateMinMax(int minId,int maxId);
		Int2BoolMap getPhotoStateBatch(MyUtil::IntSeq ids);
		MyUtil::IntSeq getPhotoIdsFrom(MyUtil::IntSeq ids);
		bool isPhotoStateTrue(int userId);
		void updatePhotoState(int userId,string photo);
		void updatePhotoStateBatch(MyUtil::Int2StrMap photomap);
	};
};
};

#endif
