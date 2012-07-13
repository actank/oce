#ifndef __BUDDYBYNAMEMOD_H__
#define __BUDDYBYNAMEMOD_H__

#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "BuddyByNameShare.h"

namespace xce {
  namespace buddybyname {

    using namespace xce::buddy;
    using namespace xce::usercache;
    using namespace MyUtil;

    //---------------------------------------------------------------------------------------------

    class LoaderHelper : virtual public IceUtil::Shared {
      public:
        virtual NameNoteSeq getNameNoteSeq(const std::vector<int>& buddies) = 0;
    };
    typedef IceUtil::Handle<LoaderHelper> LoaderHelperPtr;

    class LoaderFromUserCache : virtual public LoaderHelper {
      public:
        virtual NameNoteSeq getNameNoteSeq(const std::vector<int>& buddies) {
          MCE_DEBUG("LoaderFromUserCache::getNameNoteSeq,buddies.size() " << buddies.size());
          std::map<int, UserCachePtr> propMap = BuddyHelper::instance().getUserCache(buddies);
          return BuddyHelper::instance().getNameNoteSeq(buddies,propMap);
        }
    };
    typedef IceUtil::Handle<LoaderFromUserCache> LoaderFromUserCachePtr;

    class LoaderFromDB : virtual public LoaderHelper {
      public:
        virtual NameNoteSeq getNameNoteSeq(const std::vector<int>& buddies) {
          MCE_DEBUG("LoaderFromDB::getNameNoteSeq,buddies.size() " << buddies.size());
          return BuddyHelper::instance().getNameNoteSeqFromDB(buddies);
        }
    };
    typedef IceUtil::Handle<LoaderFromDB> LoaderFromDBPtr;

  }
}
#endif
