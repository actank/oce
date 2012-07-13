
#ifndef PhotoDataGathering_H_
#define PhotoDataGathering_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "PhotoMemory.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class PhotoDataGatheringAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, PhotoDataGatheringAdapter> {
public:
  PhotoDataGatheringAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

PhotoDataGatheringManagerPrx getManager(int id);
PhotoDataGatheringManagerPrx getManagerOneway(int id);

  void GatherPhotoData(int ownerid,const MyUtil::LongSeq& photoIds,const MyUtil::Int2IntMap& friend2Rank);
  void SetRead(int uid,long photoid);
  void IncCommentCount(long photoid);
  void IncTagCount(long photoid, int tagid);


protected:
  void GatherPhotoList(int ownerid,const MyUtil::LongSeq& photoIds,const MyUtil::Int2IntMap& friend2Rank);
  void GatherUpLoadPhotos(int ownerid,const MyUtil::LongSeq& photoIds);

protected:
  virtual string name() {
    return "PhotoDataGathering";
  }
  virtual string endpoints() {
    return "@PhotoDataGathering";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<PhotoDataGatheringManagerPrx> _managersOneway;
  vector<PhotoDataGatheringManagerPrx> _managers;
};

};

};
#endif /* PhotoDataGathering_H_ */
