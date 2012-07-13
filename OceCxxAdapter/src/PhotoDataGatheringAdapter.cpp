
#include <cmath>
#include "PhotoDataGatheringAdapter.h"

using namespace MyUtil;
using namespace xce::feed;
using namespace std;


PhotoDataGatheringManagerPrx PhotoDataGatheringAdapter::getManager(int id) {
  return locate<PhotoDataGatheringManagerPrx> (_managers, "M", id, TWO_WAY);
}

PhotoDataGatheringManagerPrx PhotoDataGatheringAdapter::getManagerOneway(int id) {
  return locate<PhotoDataGatheringManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}


void  PhotoDataGatheringAdapter::GatherPhotoList(int ownerid,const MyUtil::LongSeq& photoIds,const MyUtil::Int2IntMap& friend2Rank)
{
        map<int, map<int, int> > splits;
        MyUtil::Int2IntMap::const_iterator it = friend2Rank.begin();
        for(; it!=friend2Rank.end(); ++it){
          splits[it->first%cluster()][it->first] = it->second;
        }

        map<int, map<int, int> >::iterator sIt = splits.begin();
        for(; sIt!=splits.end(); ++sIt){
          try{
              getManagerOneway(sIt->first)->GatherPhotoList(ownerid,photoIds,sIt->second);
          }catch(Ice::Exception& e){
            MCE_WARN("PhotoDataGatheringAdapter GatherPhotoList   " << sIt->first << ", " << e);
          }
        }

}

void  PhotoDataGatheringAdapter::GatherUpLoadPhotos(int ownerid,const MyUtil::LongSeq& photoIds)
{
  map<int, vector<long> > splits;
  for (int i = 0; i < photoIds.size(); i++) {
    splits[photoIds[i] % cluster()].push_back(photoIds[i]);
  }

  map<int, vector<long> >::iterator sIt = splits.begin();
  for (; sIt != splits.end(); ++sIt) {
    try {
      getManagerOneway(sIt->first)->GatherUpLoadPhotos(ownerid,sIt->second);
    } catch (Ice::Exception& e) {
      MCE_WARN("PhotoDataGatheringAdapter GatherUpLoadPhotos " << sIt->first << ", " << e);
    }
  }

}


void  PhotoDataGatheringAdapter::GatherPhotoData(int ownerid,const MyUtil::LongSeq& photoIds,const MyUtil::Int2IntMap& friend2Rank)
{

  GatherPhotoList(ownerid,photoIds,friend2Rank);
  GatherUpLoadPhotos(ownerid,photoIds);
}

void PhotoDataGatheringAdapter::SetRead(int uid,long photoid) {
  getManagerOneway(uid)->SetRead(uid,photoid);
}

void PhotoDataGatheringAdapter::IncCommentCount(long photoid) {
  getManagerOneway(photoid)->IncCommentCount(photoid);
}

void PhotoDataGatheringAdapter::IncTagCount(long photoid, int tagid) {
  getManagerOneway(photoid)->IncTagCount(photoid,tagid);
}


