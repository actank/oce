/*
 * =====================================================================================
 *
 *       Filename:  BuddyFriendsPreloaderI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年05月26日 16时46分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yang.yu@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __BUDDYFRIENDSPRELOADER_H__
#define __BUDDYFRIENDSPRELOADER_H__

#include <IceUtil/IceUtil.h>

namespace xce {
  namespace buddyfriendspreloader {

    class BuddyFriendsPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<BuddyFriendsPreloader> {
      public:
        BuddyFriendsPreloader() {}
        virtual ~BuddyFriendsPreloader() {}
    };

  }
}
#endif
