/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosManagerAdapter.h
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

namespace friendsrecentphotos {
  using namespace xce::friendsphotos;
  class FriendsRecentPhotosManagerAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::friendsphotos::FriendsRecentPhotosManagerPrx>,
      public MyUtil::Singleton<FriendsRecentPhotosManagerAdapter> {
    public:
      FriendsRecentPhotosManagerAdapter() : 
        MyUtil::ReplicatedClusterAdapterI<xce::friendsphotos::FriendsRecentPhotosManagerPrx> ("ControllerFriendsRecentPhotosManager", 120, 300,
          new MyUtil::OceChannel, true, "FRPM") {}
      PhotoResultPtr getFriendsRecentPhotos(int userId, int begin, int limit);

      UserPhotosPtr getUserRecentPhotos(int userId);

      void addRecentPhotos(const UserPhotosSeq& userPhotos);

      void deleteUserPhotos(const UserPhotosSeq& userPhotos);

      void setData(const MyUtil::ObjectResultPtr& userPhotoData);
    private:
      FriendsRecentPhotosManagerPrx getFriendsRecentPhotosManagerPrx(int id = 0);
      vector<FriendsRecentPhotosManagerPrx> getAllFriendsRecentPhotosManagerPrx(int id = 0);
  };
}

#endif
