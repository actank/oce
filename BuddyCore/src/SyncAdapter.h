#ifndef SYNCMANAGERADAPTER_H_
#define SYNCMANAGERADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"

#include <BuddyCore.h>
#include "BuddyCoreI.h"

namespace com
{
  namespace xiaonei
  {
    namespace service
    {
      namespace buddy
      {

        const static bool SYNC_LOCAL = false;
        const static bool SYNC_REMOTE = true;
        const static std::string SYNC_REMOTE_KEY = "SYNC_REMOTE_KEY";
        const static std::string SYNC_REMOTE_VALUE = "1";
        const static std::string SYNC_LOCAL_KEY = "SYNC_LOCAL_KEY";
        const static std::string SYNC_LOCAL_VALUE = "1";

        const static bool SYNC_BIBLOCK = true;
        const static bool SYNC_NORMALBLOCK = false;
        const static std::string SYNC_BIBLOCK_KEY = "SYNC_BIBLOCK_KEY";
        const static std::string SYNC_BIBLOCK_VALUE = "1";
        const static std::string RELATION = "RELATION";

        enum Operation{
          ADDAPPLY,
          ACCEPTAPPLY,
          DENYAPPLY,
          REMOVEFRIEND,
          ADDBLOCK,
          REMOVEBLOCK,
          ADDFRIEND //backdoor
        };

        class SyncAdapter :
          //public MyUtil::AdapterI,
          //public MyUtil::AdapterISingleton<MyUtil::OceChannel, SyncAdapter>,
          public MyUtil::ReplicatedClusterAdapterI<BuddyCoreSyncManagerPrx>,
          public MyUtil::Singleton<SyncAdapter>
        {
          public:
            void reload(::Ice::Int);
            void syncAddFriend(const Ice::Int&, const Ice::Int&, const Ice::Current&);
            void syncAddApply(const Applyship&, const Ice::Current&);
            void syncAcceptApply(const Applyship&, const Ice::Current&);
            void syncDenyApply(const Applyship&, const Ice::Current&);
            void syncRemoveFriend(const Friendship&, const Ice::Current&);
            void syncAddBlock(const Blockship&, bool biblock, const Ice::Current&);
            void syncRemoveBlock(const Blockship&, bool biblock, const Ice::Current&);

            BuddyCoreSyncManagerPrx getRemoteSyncManager(int fromId);

          protected:
            virtual void initialize();

          private:
            SyncAdapter():
              MyUtil::ReplicatedClusterAdapterI<BuddyCoreSyncManagerPrx>("ControllerBuddyCore",120,300,new MyUtil::OceChannel,true,BUDDYCORESYNC_MANAGER){
                initialize();
              }
            friend class MyUtil::Singleton<SyncAdapter>;
            BuddyCoreSyncManagerPtr _localSyncManager;
            typedef vector<BuddyCoreSyncManagerPrx> BuddyCoreSyncManagerSeq;
            BuddyCoreSyncManagerSeq _syncManagers;

            void buildContext(Ice::Context&, Operation, bool remote = SYNC_REMOTE,
                bool biblock = SYNC_NORMALBLOCK);
        };

      }
    }
  }
}

#endif /*SYNCMANAGERADAPTER_H_*/
