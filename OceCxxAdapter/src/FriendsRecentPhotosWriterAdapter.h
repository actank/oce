/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosWriterAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2011 01:58:07 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __FRIENDS_RECENT_PHOTOS_MANAGER_ADAPTER_H__
#define __FRIENDS_RECENT_PHOTOS_MANAGER_ADAPTER_H__

#include <AdapterI.h>
#include <ServiceI.h>
#include <FriendsRecentPhotos.h>
#include <IceUtil/IceUtil.h>
#include <ext/hash_set>

namespace friendsrecentphotos {
  using namespace xce::friendsphotos;
  class FriendsRecentPhotosWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::friendsphotos::FriendsRecentPhotosWriterPrx>,
      public MyUtil::Singleton<FriendsRecentPhotosWriterAdapter>, IceUtil::Thread {
    public:
      FriendsRecentPhotosWriterAdapter() : 
         MyUtil::ReplicatedClusterAdapterI<xce::friendsphotos::FriendsRecentPhotosWriterPrx> ("ControllerFriendsRecentPhotosWriter", 120, 300) {}
      void reloadUserPhotos(const MyUtil::IntSeq& userIds);
    protected:
      void run();
    private:
      FriendsRecentPhotosWriterPrx getFriendsRecentPhotosWriterPrx(int id = 0);
      vector<FriendsRecentPhotosWriterPrx> getAllFriendsRecentPhotosWriterPrx(int id = 0);
      IceUtil::Monitor<IceUtil::Mutex> _mutex;
      hash_set<int> _userIdSet;
  };
}

#endif
