#ifndef __BUDDY_CACHE_DATA_H__
#define __BUDDY_CACHE_DATA_H__

#include "Buddy.pb.h"

namespace xce{
  namespace distcache{
    namespace buddycache{
      class BuddyCacheData :     
        public xce::distcache::buddycache::FriendData,        
        public Ice::Object
      {
        public:
          BuddyCacheData(){}
      };

      typedef IceUtil::Handle<BuddyCacheData> BuddyCacheDataPtr;
    }
  }
}

#endif
