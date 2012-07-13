#ifndef PHOTOSTATEFILTER_H
#define PHOTOSTATEFILTER_H

#include <PhotoStateFilter.h>
#include <AdapterI.h>
#include <bitset>

namespace xce {
namespace photostatefilter {
namespace adapter {

const int PHOTOSTATE_FILTER_SIZE = 2147483647;

class PhotoStateFilterAdapter :
	public MyUtil::ReplicatedClusterAdapterI<PhotoStateFilterManagerPrx>,
		public MyUtil::Singleton<PhotoStateFilterAdapter> {
public:
	PhotoStateFilterAdapter() : MyUtil::ReplicatedClusterAdapterI<PhotoStateFilterManagerPrx>("ControllerPhotoStateFilter", 120, 3000) {}
	MyUtil::ByteSeq getPhotoStateMinMax(int minId,int maxId);
        xce::photostatefilter::Int2BoolMap getPhotoStateBatch(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getPhotoIdsFrom(const MyUtil::IntSeq& ids);
        void getAllPhotoState(std::bitset<PHOTOSTATE_FILTER_SIZE>& s);
        bool isPhotoStateTrue(int userId);
        void updatePhotoState(int userId,const std::string& photo);
        void updatePhotoStateBatch(const MyUtil::Int2StrMap& photomap);
};

}
}
}

#endif
