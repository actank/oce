/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosWriterI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2011 07:03:30 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#ifndef __FRIENDS_RECENT_PHOTOS_WRITERI_H__
#define __FRIENDS_RECENT_PHOTOS_WRITERI_H__

#include "FriendsRecentPhotos.h"
#include "ServiceI.h"

namespace xce {
namespace friendsphotos {

  class FriendsRecentPhotosWriterI : public FriendsRecentPhotosWriter, public MyUtil::Singleton<FriendsRecentPhotosWriterI> {
    public:
      virtual void addRecentPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& = Ice::Current());

      virtual void deleteUserPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& = Ice::Current());

      virtual void reloadUserPhotos(const MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());
    private:
      void insertEmptyData(const vector<int>& userIds);
      //opCode = 1为增加，opCode = 2为删除
      void opUserPhotosSeq(const UserPhotosSeq& userPhotosSeq, int opCode);
  };
}
}

#endif
