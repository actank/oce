/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosPreloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2011 09:19:38 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __FRIENDS_RECENT_PHOTOS_PRELOADER_H__
#define __FRIENDS_RECENT_PHOTOS_PRELOADER_H__

#include "FriendsRecentPhotos.h"
#include "ServiceI.h"

#include "ServiceI.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"

namespace xce {
namespace friendsphotos {
  class FriendsRecentPhotosPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<FriendsRecentPhotosPreloader> {
    public:
      FriendsRecentPhotosPreloader() {}
      virtual ~FriendsRecentPhotosPreloader() {}
  };

  class FriendsRecentPhotosPreloaderFactory : virtual public xce::generalpreloader::IdListFactory {
    public:
      virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
  };

  class BatchFriendsRecentPhotosIdProducer : public virtual xce::generalpreloader::Producer {
    public:
      virtual std::vector<int> produce(int beginId, int load_batch_size, int& maxId, int& tableMod);
  };

}
}

#endif
