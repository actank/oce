#ifndef __BuddyCoreI_h__
#define __BuddyCoreI_h__

#include <BuddyCore.h>
#include "ServiceI.h"
#include "../../BuddyEntry/src/BuddyEntry.h"

namespace com {

  namespace xiaonei {

    namespace service {

      namespace buddy {

        const static string DB_BUDDY_FRIEND = "buddycore_friend";
        //old database
        const static string DB_BUDDY_APPLY = "buddycore_apply";
        //new database
        const static string DB_BUDDY_APPLICANT = "buddycore_applicant";
        const static string DB_BUDDY_ACCEPTER = "buddycore_accepter";

        const static string DB_BUDDY_BLOCK = "buddycore_block";
        const int TABLE_FRIEND_COUNT = 100;
        const int TABLE_ACCEPTER_COUNT = 100;
        const int TABLE_APPLICANT_COUNT = 100;
        const static string TABLE_FRIEND = "relation_";
        const static string TABLE_APPLY = "buddyapply";
        const static string TABLE_APPLY_ACCEPTER = "buddyapply_accepter_";
        const static string TABLE_APPLY_APPLICANT = "buddyapply_applicant_";
        const static string TABLE_BLOCK = "relation_block";

        const static int ENTRY_SEGMENT = 0;

        const static int TASK_LEVEL_DUPCLEANUP = 1;
        const static int TASK_LEVEL_SYNCUSERCOUNT = 2;
        const static int TASK_LEVEL_BATCHPROCESS = 3;
        const static int TASK_LEVEL_MISSEDIDPROCESS = 4;

        const static string BUDDYCORE_MANAGER   = "BCM";
        const static string BUDDYCORESYNC_MANAGER   = "BCSM";

        class BuddyCoreManagerI: virtual public BuddyCoreManager,virtual public MyUtil::Singleton<BuddyCoreManagerI> {
          public:

            virtual void addApply(const Applyship&, const ApplyInfo& info,
                const Ice::Current& = Ice::Current());

            virtual OpState addApplyWithState(const Applyship&, const ApplyInfo& info,
                const Ice::Current& = Ice::Current());

            virtual void addApplyBatch(const ApplyshipMap&, const ApplyInfoMap&,
                const Ice::Current& = Ice::Current());

            virtual OpStateMap addApplyWithStateBatch(const ApplyshipMap&,
                const ApplyInfoMap&, const Ice::Current& = Ice::Current());

            virtual void acceptApply(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual OpState acceptApplyWithState(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual void acceptAllApplied(::Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual void denyApply(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual OpState denyApplyWithState(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual void denyAllApplied(::Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual void denyAllApply(::Ice::Int, const Ice::Current& =
                Ice::Current());	

            virtual ::Ice::Int getAppliedCount(::Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual ::MyUtil::IntSeq getAppliedSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
                const Ice::Current& = Ice::Current());

            virtual ::Ice::Int getApplyCount(::Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual ::MyUtil::IntSeq getApplySeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
                const Ice::Current& = Ice::Current());
            virtual ::MyUtil::Int2IntSeqMap getApplySeqBatch(const ::MyUtil::IntSeq&,
                const Ice::Current& = Ice::Current());

            virtual void removeFriend(const Friendship&, const Ice::Current& =
                Ice::Current());

            virtual OpState removeFriendWithState(const Friendship&,
                const Ice::Current& = Ice::Current());

            virtual ::Ice::Int getFriendCount(::Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual ::MyUtil::IntSeq getFriendSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
                const Ice::Current& = Ice::Current());
            virtual ::MyUtil::Int2IntSeqMap getFriendSeqBatch(const ::MyUtil::IntSeq&,
                const Ice::Current& = Ice::Current());

            virtual void addBlock(const Blockship&, const Ice::Current& =
                Ice::Current());

            virtual OpState addBlockWithState(const Blockship&, const Ice::Current& =
                Ice::Current());

            virtual void removeBlock(const Blockship&, const Ice::Current& =
                Ice::Current());

            virtual OpState removeBlockWithState(const Blockship&, const Ice::Current& =
                Ice::Current());

            virtual ::Ice::Int getBlockCount(::Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual ::MyUtil::IntSeq getBlockSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
                const Ice::Current& = Ice::Current());
            virtual ::MyUtil::Int2IntSeqMap getBlockSeqBatch(const ::MyUtil::IntSeq&,
                const Ice::Current& = Ice::Current());

            virtual void addFriend(const Ice::Int, const Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual OpState addFriendWithState(const Ice::Int, const Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual BuddyDesc getRelation(const Relationship&, const Ice::Current& =
                Ice::Current());
            virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());

          private:
            BuddyDesc translate(uint32_t desc);
            EntryListHolderPtr locateEntryListHolder(uint32_t from);
            void buildContext( Ice::Context& ctx, MyUtil::Str2StrMap& map );

            friend class MyUtil::Singleton<BuddyCoreManagerI>;
            BuddyCoreManagerI(){
              ;	
            }
        };

        class BuddyCoreSyncManagerI: virtual public BuddyCoreSyncManager, virtual public MyUtil::Singleton<BuddyCoreSyncManagerI> {
          public:
            virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());

            virtual void syncAddFriend(const Ice::Int, const Ice::Int, const Ice::Current& =
                Ice::Current());

            virtual void syncAddApply(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual void syncAcceptApply(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual void syncDenyApply(const Applyship&, const Ice::Current& =
                Ice::Current());

            virtual void syncRemoveFriend(const Friendship&, const Ice::Current& =
                Ice::Current());

            virtual void syncAddBlock(const Blockship&, const Ice::Current& =
                Ice::Current());

            virtual void syncRemoveBlock(const Blockship&, const Ice::Current& =
                Ice::Current());
          private:
            friend class MyUtil::Singleton<BuddyCoreSyncManagerI>;
            BuddyCoreSyncManagerI(){
              ;
            }
            bool isLocalSync(const Ice::Current& current);
            bool isBiblock(const Ice::Current& current);
            EntryListHolderPtr findEntryListHolder(uint32_t fromId);
            void removeEntryListHolder(uint32_t fromId);
            void syncUserCountGuestRequest(uint32_t userId);
        };

        class BuddyDatabaseHelper: public MyUtil::Singleton<BuddyDatabaseHelper> {
          public:
            bool insertBuddyApply(int applicant, int accepter,
                const MyUtil::Str2StrMap& props);
            bool insertBuddyApplyN(int applicant, int accepter,
                const MyUtil::Str2StrMap& props);
            bool deleteBuddyApply(int applicant, int accepter);
            bool deleteBuddyApplyN(int applicant, int accepter);
            bool insertRelationBi(int from, int to);
            bool deleteRelationBi(int from, int to);
            bool insertBlock(int from, int to);
            bool deleteBlock(int from, int to);
          private:
            string getRelationTable(int from);
        };

        class EntryListHolderFactory: public MyUtil::ServantFactory {
          public:
            virtual Ice::ObjectPtr create(Ice::Int id);
          private:
            inline void deduplicate(int userId, EntrySet& building,
                EntryList& duplicates);
        };

        class BuddyCoreResultHandler: public com::xiaonei::xce::ResultHandler {
          public:
            BuddyCoreResultHandler(int userId, EntrySet& building,
                EntryList& duplicates, const std::string& toField,
                const uint32_t desc) :
              _userId(userId), _building(building), _duplicates(duplicates),
              _toField(toField), _desc(desc) {
              }
            virtual ~BuddyCoreResultHandler() {
            }
          private:
            virtual bool handle(mysqlpp::Row& row) const {
              int to = (int) row[_toField.c_str()];
              if (!_building.insert(Entry(to, _desc)).second) {
                _duplicates.push_back(Entry(to, _desc));
              }
              return true;
            }

            int _userId;
            EntrySet& _building;
            EntryList& _duplicates;
            std::string _toField;
            uint32_t _desc;
        };

        class DupCleanupTask: public MyUtil::Task {
          public:
            DupCleanupTask(int userId, EntryList dup) :
              MyUtil::Task(TASK_LEVEL_DUPCLEANUP), _userId(userId), _dup(dup) {
              }
            virtual void handle();
          private:
            void handle(Entry entry);
            int _userId;
            EntryList _dup;
        };

        class AddApplyTask: virtual public MyUtil::Task {
          public:
            AddApplyTask(BuddyCoreManagerI* bm,
                const com::xiaonei::service::buddy::Applyship apply,
                const com::xiaonei::service::buddy::ApplyInfo info,
                const Ice::Current current) :
              _bm(bm), _apply(apply), _info(info), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            com::xiaonei::service::buddy::Applyship _apply;
            com::xiaonei::service::buddy::ApplyInfo _info;
            Ice::Current _current;
        };

        class AcceptApplyTask: virtual public MyUtil::Task {
          public:
            AcceptApplyTask(BuddyCoreManagerI* bm,
                const com::xiaonei::service::buddy::Applyship apply,
                const int tasklevel,
                const Ice::Current current) :
              MyUtil::Task(tasklevel),_bm(bm), _apply(apply), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            com::xiaonei::service::buddy::Applyship _apply;
            Ice::Current _current;
        };

        class DenyApplyTask: virtual public MyUtil::Task {
          public:
            DenyApplyTask(BuddyCoreManagerI* bm,
                const com::xiaonei::service::buddy::Applyship apply,
                const int tasklevel,
                const Ice::Current current) :
              MyUtil::Task(tasklevel),_bm(bm), _apply(apply), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            com::xiaonei::service::buddy::Applyship _apply;
            Ice::Current _current;
        };

        class RemoveFriendTask: virtual public MyUtil::Task {
          public:
            RemoveFriendTask(BuddyCoreManagerI* bm,
                const com::xiaonei::service::buddy::Friendship buddy,
                const Ice::Current current) :
              _bm(bm), _buddy(buddy), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            com::xiaonei::service::buddy::Friendship _buddy;
            Ice::Current _current;
        };

        class AddBlockTask: virtual public MyUtil::Task {
          public:
            AddBlockTask(BuddyCoreManagerI* bm,
                const com::xiaonei::service::buddy::Blockship block,
                const Ice::Current current) :
              _bm(bm), _block(block), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            com::xiaonei::service::buddy::Blockship _block;
            Ice::Current _current;
        };

        class RemoveBlockTask: virtual public MyUtil::Task {
          public:
            RemoveBlockTask(BuddyCoreManagerI* bm,
                const com::xiaonei::service::buddy::Blockship block,
                const Ice::Current current) :
              _bm(bm), _block(block), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            com::xiaonei::service::buddy::Blockship _block;
            Ice::Current _current;
        };

        class AddFriendTask: virtual public MyUtil::Task {
          public:
            AddFriendTask(BuddyCoreManagerI* bm,
                const Ice::Int from,
                const Ice::Int to,
                const Ice::Current current) :
              _bm(bm), _from(from), _to(to), _current(current) {
              }
            virtual void handle();
          private:
            BuddyCoreManagerI* _bm;
            Ice::Int _from;
            Ice::Int _to;
            Ice::Current _current;
        };

        class SyncUserCountGuestRequestTask: virtual public MyUtil::Task {
          public:
            SyncUserCountGuestRequestTask(int userId) :
              Task(TASK_LEVEL_SYNCUSERCOUNT), _userId(userId) {
              }
            virtual void handle();
          private:
            int _userId;
        };

        class MissedIdProcessTask : virtual public MyUtil::Task { 
          public:
            MissedIdProcessTask(int userId) :
              Task(TASK_LEVEL_MISSEDIDPROCESS), _userId(userId) {
              }
            virtual void handle();
          private:
            int _userId;
        };
      
      }
    }
  }
}

#endif
