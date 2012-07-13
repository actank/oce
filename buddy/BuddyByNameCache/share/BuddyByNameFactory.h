#ifndef __BUDDYBYNAMEFACTORY_H__
#define __BUDDYBYNAMEFACTORY_H__

#include "BuddyByNameMod.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h" 

namespace xce {
  namespace buddybyname {

    using namespace xce::buddy;
    using namespace xce::buddybyname;
    using namespace xce::usercache;
    using namespace xce::buddy::adapter;
    using namespace MyUtil;

    //-----------------------------------BuddyByNameFactory-----------------------------------------------
    class BuddyByNameFactory : virtual public xce::generalpreloader::IdListFactory,
    virtual public xce::cacheloader::IdListFactory,
    virtual public MyUtil::Singleton<BuddyByNameFactory> {
      public:
        enum LoaderMod {
          USERCACHE,
          DB
        };

      public:
        BuddyByNameFactory() {
          _loader = new LoaderFromUserCache;
        }

        virtual ~BuddyByNameFactory() {}

        void changeLoaderMod (LoaderMod mod) {
          IceUtil::RWRecMutex::WLock lock(_mutex);
          switch (mod) {
            case USERCACHE :
              MCE_INFO("changeLoaderMod->FromUserCache!!");
              _loader = new LoaderFromUserCache;
              break;
            case DB :
              MCE_INFO("changeLoaderMod->FromDB!!");
              _loader = new LoaderFromDB;
              break;
            default:
              MCE_INFO("changeLoaderMod nothing to do!!");
              break;
          }
        }

        virtual NameNoteSeq getNameNoteSeq(const MyUtil::IntSeq& ids) {
          return _loader->getNameNoteSeq(ids);
        }

        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
          ostringstream idsKey;
          ostringstream idsInfo;
          MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();

          MyUtil::Int2IntSeqMap buddymap = BuddyByIdCacheAdapter::instance().getFriendLists( ids );

          for( MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){
            idsKey << *idIt << " " ;

            MyUtil::IntSeq buddies;
            MyUtil::Int2IntSeqMap::iterator it = buddymap.find( *idIt );
            if( it != buddymap.end() ){
              if (it->second.empty()) {
                continue;
              } else {
                buddies = it->second;
              }
            } else{
              //res->data[*idIt] = 0;
              continue;
            }

            /*
               NameCnvtrPtr cc = new NameCnvtr("utf-8","big5","gb2312");
               std::multimap<std::string,int,KeyCmp> friendMap = BuddyHelper::instance().getBuddyMapByName(buddies,propMap,cc);
               BuddyByNameDataPtr obj= new BuddyByNameData;
               BuddyHelper::instance().getBuddyListByName(friendMap).swap( obj->friends );
               res->data[*idIt] = obj;
             */

            BuddyByNameDataPtr obj = new BuddyByNameData;
            {
              IceUtil::RWRecMutex::RLock lock(_mutex);
              //test
              ostringstream friendsInfo;
              for(MyUtil::IntSeq::const_iterator it = buddies.begin();
                  it != buddies.end(); ++it) {
                friendsInfo << *it << " ";
              }
              MCE_DEBUG("id " << *idIt << " friendsSize " << buddies.size() << " friendsInfo: " << friendsInfo.str());

              _loader->getNameNoteSeq(buddies).swap(obj->names);
              //test
              ostringstream namesInfo;
              for(vector<NameNote>::iterator it = obj->names.begin();
                  it != obj->names.end(); ++it) {
                namesInfo << (*it).id << "(" << (*it).name << ") "; 
              }
              MCE_DEBUG("swap names: " << namesInfo.str());
            }
            res->data[*idIt] = obj;
            idsInfo << *idIt << "(" << obj->names.size() << ") ";

          }
          MCE_DEBUG("idsResult: " << idsInfo.str() << " ==> result/ids size: " << res->data.size() << "/" << ids.size());
          return res;
        }

        Ice::ObjectPtr create(Ice::Int id) {
          MyUtil::IntSeq buddies;
          try {
            buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc( id, -1 );
          } catch( Ice::Exception e ){
            MCE_WARN("[BuddyByNameLoaderFactory::create] id=" << id << " found Ice::Exception : " << e.what() );
            return 0;
          } catch( ... ){
            MCE_WARN("[BuddyByNameLoaderFactory::create] id=" << id << " found Ice::Exception" );
            return 0;
          }

          /*
             NameCnvtrPtr cc = new NameCnvtr("utf-8","big5","gb2312");
             std::multimap<std::string,int,KeyCmp> friendMap = BuddyHelper::instance().getBuddyMapByName(buddies,propMap,cc);
             BuddyByNameDataPtr obj = new BuddyByNameData;
             BuddyHelper::instance().getBuddyListByName(friendMap).swap( obj->friends );
           */
          BuddyByNameDataPtr obj = new BuddyByNameData;
          {
            IceUtil::RWRecMutex::RLock lock(_mutex);
            _loader->getNameNoteSeq(buddies).swap(obj->names);
          }
          return obj;
        }
      private:
        LoaderHelperPtr _loader;
        IceUtil::RWRecMutex _mutex;
    };
    //-----------------------------------BuddyByNameFactory end-----------------------------------------------

  }
}

#endif
