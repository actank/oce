
#ifndef _XCE_PHOTO_MEMC_CLIENT_H_
#define _XCE_PHOTO_MEMC_CLIENT_H_

#include "PhotoMemory.h"

#include "Singleton.h"
#include "photo.pb.h"

#include "MemcachedClientPool.h"

namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;

class PhotoMemcClient : public Singleton<PhotoMemcClient> {
public:
  ~PhotoMemcClient();
  PhotoMemcClient();


  //Content
  PhotoContentPtr GetPhotoContent(Ice::Long id);
  PhotoContentDict GetPhotoContentDict(const MyUtil::LongSeq& ids);
  bool SetPhotoContent(const PhotoContentPtr& content);
  bool SetPhotoContentSeq(const PhotoContentSeq& contentSeq);
  bool SetEmptyPhotoContent(Ice::Long id);
  // 串行化的结果
  bool SerializePhotoContent(const PhotoContentPtr& content, string * serialized);


  //PhotoSeq
  ContentItemSeq GetPhotoSeq(int uid);
  //MyUtil::LongSeq GetPhotoListDict(const MyUtil::IntSeq& uids);
  bool SetPhotoSeq(int uid,const ContentItemSeq& seq);
  bool SetPhotoSeqList(const UserPhotoSeqList& list);
  bool SetEmptyPhotoSeq(int id);
  // 串行化的结果
  bool SerializePhotoSeq(const ContentItemSeq& seq,
      string * serialized);

  //RankInfo
  PhotoRankInfoPtr GetPhotoRankInfo(Ice::Long id);
  PhotoRankInfoDict GetPhotoRankInfoDict(const MyUtil::LongSeq& ids);
  bool SetPhotoRankInfo(const PhotoRankInfoPtr& content);
  bool SetPhotoRankInfoSeq(const PhotoRankInfoSeq& rankInfoSeq);
  bool SetEmptyPhotoRankInfo(Ice::Long id);
  // 串行化的结果
  bool SerializePhotoRankInfo(const PhotoRankInfoPtr& content,
      string * serialized);

protected:
  PoolManager pool_;
};

}
}
#endif //_XCE_PHOTO_MEMC_CLIENT_H_
