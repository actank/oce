/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosManagerI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2011 06:44:03 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __FRIENDS_RECENT_PHOTOS_MANAGERI_H__
#define __FRIENDS_RECENT_PHOTOS_MANAGERI_H__

#include "FriendsRecentPhotos.h"
#include "ServiceI.h"

#include <set>

namespace xce {
namespace friendsphotos {
 
  using namespace std;

  class FriendsRecentPhotosManagerI : public FriendsRecentPhotosManager, public MyUtil::Singleton<FriendsRecentPhotosManagerI>, IceUtil::Thread {
    public:
      virtual PhotoResultPtr getFriendsRecentPhotos(int userId, int begin, int limit, const Ice::Current& = Ice::Current());

      virtual UserPhotosPtr getUserRecentPhotos(int userId, const Ice::Current& = Ice::Current());

      virtual void addRecentPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& = Ice::Current());

      virtual void deleteUserPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& = Ice::Current());

      virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

      virtual void setData(const MyUtil::ObjectResultPtr& userPhotoData, const Ice::Current& = Ice::Current());

      bool isValid();

    protected:
      void run();
    private:
      IceUtil::Mutex _mutex;
      bool _valid;
      //存储节点用户
      vector<int> getSpecialUsers();
      void addSpecialUsers(int userId);
      bool isSpecialUser(int userId);
      set<int> _specialUsers;
  };

  class UserPhotosInfoFactoryI : public MyUtil::ServantFactory {
    public:
      Ice::ObjectPtr create(Ice::Int id);
  };
  
  class UserFriendsFactoryI : public MyUtil::ServantFactory {
    public:
      Ice::ObjectPtr create(Ice::Int id);
  };

}
}

#endif
