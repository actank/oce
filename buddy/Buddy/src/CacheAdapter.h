#ifndef CACHEADAPTER_H_
#define CACHEADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"

#include <BuddyCommon.h>
#include <Buddy.h>
namespace xce
{
namespace buddy
{

const static bool Cache_LOCAL = false;
const static bool Cache_REMOTE = true;
const static std::string Cache_REMOTE_KEY = "Cache_REMOTE_KEY";
const static std::string Cache_REMOTE_VALUE = "1";
const static std::string Cache_LOCAL_KEY = "Cache_LOCAL_KEY";
const static std::string Cache_LOCAL_VALUE = "1";

const static bool Cache_BIBLOCK = true;
const static bool Cache_NORMALBLOCK = false;
const static std::string Cache_BIBLOCK_KEY = "Cache_BIBLOCK_KEY";
const static std::string Cache_BIBLOCK_VALUE = "1";

class CacheAdapter :public MyUtil::AdapterI,
            public MyUtil::AdapterISingleton<MyUtil::OceChannel, CacheAdapter>
{
public:
    void reload(::Ice::Int);
    void addFriend(const Relationship&, const Ice::Current&);
    void cacheAddApply(const Relationship&, const Ice::Current&);
    void cacheAcceptApply(const Relationship&, const Ice::Current&);
    void cacheDenyApply(const Relationship&, const Ice::Current&);
    void cacheRemoveFriend(const Relationship&, const Ice::Current&);
    void cacheAddBlock(const Relationship&, bool biblock, const Ice::Current&);
    void cacheRemoveBlock(const Relationship&, bool biblock, const Ice::Current&);

    BuddyCachePrx getRemoteCacheManager(int fromId);

protected:
    virtual string name()
    {
        return "Buddy";
    }
    virtual string endpoints()
    {
        return "@Buddy";
    }
    virtual size_t cluster()
    {
        return 10;
    }
    virtual void initialize();

private:
    BuddyCachePtr _localCacheManager;
    typedef vector<BuddyCachePrx> BuddyCacheSeq;
    BuddyCacheSeq _CacheManagers;

    void buildContext(Ice::Context&, bool remote = Cache_REMOTE,
                      bool biblock = Cache_NORMALBLOCK);
};

}
}

#endif /*CACHEADAPTER_H_*/
