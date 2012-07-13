#ifndef __BUDDY_CORE_LOGICI_H__
#define __BUDDY_CORE_LOGICI_H__

#include <BuddyCore.h>
#include <BuddyRelationCache.h>
#include <BuddyByApplicantCache.h>
#include <IceUtil/IceUtil.h>
#include <Date.h>
#include "QueryRunner.h"
#include "ServiceI.h"
#include "Checker/share/OperaData.h"

namespace com {

  namespace xiaonei {

    namespace service {

      namespace buddy {

        const static string BUDDY_CORE_LOGIC = "BCL";

        //IM
        const int TASK_LEVEL_PRESENCESUBSCRIBE = 1;
        const int TASK_LEVEL_PRESENCESUBSCRIBED = 2;
        const int TASK_LEVEL_PRESENCEUNSUBSCRIBED = 3;
        const int TASK_LEVEL_REMOVEBUDDYNOTIFY = 4;
        const int TASK_LEVEL_ADDBUDDYONLINESTAT = 5;
        const int TASK_LEVEL_REMOVEBUDDYONLINESTAT = 6;
        const int TASK_LEVEL_RELOADBLOCKUSER = 7;

        const int TASK_LEVEL_NOTIFYBUDDYAPPLYCHANGE = 71;

        //XFeed
        const int TASK_LEVEL_DELIVERSUPPLY = 8;
        const int TASK_LEVEL_DELINSQL = 9;

        //Apply & Apply_Count
        //const int TASK_LEVEL_REBUILDBUDDYAPPLYCACHE = 10;
        //const int TASK_LEVEL_REMOVEBUDDYAPPLYCACHE = 11;
        const int TASK_LEVEL_INCUSERCOUNTGUESTREQUEST = 12;
        const int TASK_LEVEL_DECUSERCOUNTGUESTREQUEST = 13;

        //people you may know

        const int TASK_LEVEL_APPLYNEWCOMER = 15;
        const int TASK_LEVEL_ACCEPTNEWCOMER = 16;
        const int TASK_LEVEL_APPLYOFFER = 17;
        const int TASK_LEVEL_ACCEPTOFFER = 18;
        const int TASK_LEVEL_REMOVEMSNFINDER = 19;
        const int TASK_LEVEL_REMOVEFRIENDFINDER = 20;
        const int TASK_LEVEL_REBUILDFRIENDFINDER = 21;

        //Recommend
        const int TASK_LEVEL_REMOVERECOMMENDFRIEND = 22;

        //FriendDist
        const int TASK_LEVEL_REBUILDFRIENDDIST = 23;

        //BuddyGroup
        const int TASK_LEVEL_REMOVEBUDDYGROUP = 24;

        //New Buddy
        const int TASK_LEVEL_ADDBUDDYBYIDLOADER = 28;
        const int TASK_LEVEL_REMOVEBUDDYBYIDCACHE = 29;
        const int TASK_LEVEL_REMOVEBUDDYBYNAMECACHE = 30;
        const int TASK_LEVEL_REMOVEBUDDYBYONLINETIMECACHE = 31;
        const int TASK_LEVEL_REMOVEBUDDYBYADDTIMECACHE = 32;
        const int TASK_LEVEL_DECFRIENDCOUNTCACHE = 33;
        const int TASK_LEVEL_RELOADBUDDYBYIDLOADER = 34;
        const int TASK_LEVEL_NOTIFYANTISPAMADDAPPLY = 35;

        const int TASK_LEVEL_NOTIFYFRIENDOFFRIENDSEARCH = 36;

        const int TASK_LEVEL_ADDBUDDYBYAPPLICANTCACHE = 37;
        const int TASK_LEVEL_REMOVEBUDDYBYAPPLICANTCACHE = 38;

        const int TASK_LEVEL_SETBUDDYRELATIONCACHE_APPLY = 39;
        const int TASK_LEVEL_SETBUDDYRELATIONCACHE_FROM_TO = 40;
        const int TASK_LEVEL_SETBUDDYRELATIONCACHE_BLOCK = 41;


        const int TASK_LEVEL_SETBUDDYRELATIONCACHE_ACCEPTAPPLY = 42;
        const int TASK_LEVEL_SETBUDDYRELATIONCACHE_DENYAPPLY = 43;
        const int TASK_LEVEL_SETBUDDYRELATIONCACHE_REMOVEBLOCK = 44;

        const int TASK_LEVEL_REMOVECARDRELATIONCACHE_FRIEND = 45;
        const int TASK_LEVEL_CHANGEBUDDYNOTIFYFEED = 46;

        const int TASK_LEVEL_CHECKADDBUDDY = 47;

        const int TASK_LEVEL_REMOVECOMMOMFRIEND = 48;

        const int TASK_LEVEL_CHECKREMOVEBUDDY = 49;
        const int TASK_LEVEL_CHECKADDAPPLY = 50;
        const int TASK_LEVEL_CHECKREMOVEAPPLY = 51;
        const int TASK_LEVEL_SOCALGRAPHLOGICPEPORT = 52;
        const int TASK_LEVEL_BUDDYAPPLYNOTIFYFEED = 53;

        const int TASK_LEVEL_ADDBUDDYFRIENDCACHE = 54;
        const int TASK_LEVEL_REMOVEBUDDYFRIENDCACHE = 55;

        const int TASK_LEVEL_ADDBUDDYAPPLYNOTIFYRELOADER = 56;
        const int TASK_LEVEL_REMOVEBUDDYAPPLYNOTIFYRELOADER = 57;
        
        const int TASK_LEVEL_BECOMEFRIENDNOTIFYBYIDRELOADER = 58;
        const int TASK_LEVEL_REMOVEFRIENDNOTIFYBYIDRELOADER = 59;

        const int TASK_LEVEL_XOAADDAPPLYTASK = 60;
        const int TASK_LEVEL_XOAACCEPTAPPLYTASK = 61;
        const int TASK_LEVEL_XOADENYAPPLYTASK = 62;
        const int TASK_LEVEL_XOAADDFRIENDTASK = 63;
        const int TASK_LEVEL_XOAREMOVEFRIENDTASK = 64;
        const int TASK_LEVEL_XOAADDBLOCKTASK = 65;
        const int TASK_LEVEL_XOAREMOVEBLOCKTASK = 66;
        const int TASK_LEVEL_XOARELOADTASK = 67;

        const int TASK_LEVEL_REBUILDBUDDYAPPLYCACHEN = 69;//tmp
        const int TASK_LEVEL_REMOVEBUDDYAPPLYCACHEN = 70;//tmp

        const int TASK_LEVEL_REMOVERCDCACHETASK = 68;
        const int TASK_LEVEL_BECOMEFRIENDSTASK = 71;
        const int TASK_LEVEL_BLOCKTASK = 72;
  
        //69,70已使用
          
        /* @brief	接口总类
         */
        class BuddyCoreLogicI : virtual public BuddyCoreLogic {
          public:

            virtual void addApply(const Applyship& apply, const Ice::Current&);

            virtual void acceptApply(const Applyship& apply, const Ice::Current&);

            virtual void denyApply(const Applyship& apply, const Ice::Current&);

            virtual void removeFriend(const Friendship& buddy, const Ice::Current&);

            virtual void addBlock(const Blockship& block, const Ice::Current&);

            virtual void removeBlock(const Blockship& block, const Ice::Current&);

            virtual void addFriend(const Ice::Int from, const Ice::Int to,
                const Ice::Current&);

            virtual void reload(const Ice::Int userId, const Ice::Current&);
        };

        // ------------------------------------------------
        class DelInSqlTask : public MyUtil::Task {
          public:
            DelInSqlTask(const string& source, int from, int to, ::Ice::Int level=TASK_LEVEL_DELINSQL ):
              MyUtil::Task(level),
              _source(source),
              _from(from),
              _to(to)  {
              }
            virtual void handle();
          private:
            string _source;
            int _from;
            int _to;
        };

        // -------------------------------------------------
        class PresenceSubscribeTask : public MyUtil::Task {
          public:
            PresenceSubscribeTask(const string& id, int from, int to, const string& status, ::Ice::Int level=TASK_LEVEL_PRESENCESUBSCRIBE ):
              MyUtil::Task(level),
              _id(id),
              _from(from),
              _to(to),
              _status(status) {
              }
            virtual void handle();
          private:
            string _id;
            int _from;
            int _to;
            string _status;
        };

class PresenceSubscribeWithPostscriptTask : public MyUtil::Task {
 public:
  PresenceSubscribeWithPostscriptTask(const std::string& id, int from, int to, 
                                      const std::string& status, const std::string& postscript,
                                      Ice::Int level = TASK_LEVEL_PRESENCESUBSCRIBE) : 
    MyUtil::Task(level),
    id_(id),
    from_(from),
    to_(to),
    status_(status),
    postscript_(postscript) {

  }
  virtual void handle();
 private:
  std::string id_;
  int from_;
  int to_;
  std::string status_;
  std::string postscript_;
};

        class PresenceSubscribedTask : public MyUtil::Task {
          public:
            PresenceSubscribedTask(const string& id, int from, int to, ::Ice::Int level=TASK_LEVEL_PRESENCESUBSCRIBED ):
              MyUtil::Task(level),
              _id(id),
              _from(from),
              _to(to) {
              }
            virtual void handle();
          private:
            string _id;
            int _from;
            int _to;
        };

        class PresenceUnsubscribedTask : public MyUtil::Task {
          public:
            PresenceUnsubscribedTask(const string& id, int from, int to, ::Ice::Int level=TASK_LEVEL_PRESENCEUNSUBSCRIBED ):
              MyUtil::Task(level),
              _id(id),
              _from(from),
              _to(to) {
              }
            virtual void handle();
          private:
            string _id;
            int _from;
            int _to;
        };

        // -------------------------------------------

        /* @brief RebuildBuddyApplyCacheTask
         */
        /*
           class RebuildBuddyApplyCacheTask : public MyUtil::Task {
           public:
           RebuildBuddyApplyCacheTask(int id, ::Ice::Int level=TASK_LEVEL_REBUILDBUDDYAPPLYCACHE ) :
           MyUtil::Task(level),
           _id(id) {
           }
           virtual void handle();
           private:
           int _id;
           };

           class RemoveBuddyApplyCacheTask : public MyUtil::Task {
           public:
           RemoveBuddyApplyCacheTask(int id, int applicant, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYAPPLYCACHE ) :
           MyUtil::Task(level),
           _id(id),
           _applicant(applicant) {
           }
           virtual void handle();
           private:
           int _id;
           int _applicant;
           };
         */

        class RebuildBuddyApplyCacheNTask : public MyUtil::Task {
          public:
            RebuildBuddyApplyCacheNTask(int id, const Ice::Context& ctx, ::Ice::Int level=TASK_LEVEL_REBUILDBUDDYAPPLYCACHEN ) :
              MyUtil::Task(level),
              _id(id),_ctx(ctx) {
              }
            virtual void handle();
          private:
            int _id;
            Ice::Context _ctx;
        };

        class AddBuddyApplyNotifyReloaderTask : public MyUtil::Task {
          public:
            AddBuddyApplyNotifyReloaderTask(int id, const Ice::Context& ctx, ::Ice::Int level=TASK_LEVEL_ADDBUDDYAPPLYNOTIFYRELOADER ) :
              MyUtil::Task(level),
              _id(id),_ctx(ctx) {
              }
            virtual void handle();
          private:
            int _id;
            Ice::Context _ctx;
        };

        class RemoveBuddyApplyCacheNTask : public MyUtil::Task {
          public:
            RemoveBuddyApplyCacheNTask(int id, int applicant, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYAPPLYCACHEN ) :
              MyUtil::Task(level),
              _id(id),
              _applicant(applicant) {
              }
            virtual void handle();
          private:
            int _id;
            int _applicant;
        };

        class RemoveBuddyApplyNotifyReloaderTask : public MyUtil::Task {
          public:
            RemoveBuddyApplyNotifyReloaderTask(int id, int applicant, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYAPPLYNOTIFYRELOADER ) :
              MyUtil::Task(level),
              _id(id),
              _applicant(applicant) {
              }
            virtual void handle();
          private:
            int _id;
            int _applicant;
        };

        class BecomeFriendNotifyByIdReloaderTask : public MyUtil::Task {
          public:
            BecomeFriendNotifyByIdReloaderTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_BECOMEFRIENDNOTIFYBYIDRELOADER ) :
              MyUtil::Task(level),
              _host(host),_guest(guest) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;
        };

        class RemoveFriendNotifyByIdReloaderTask : public MyUtil::Task {
          public:
            RemoveFriendNotifyByIdReloaderTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_REMOVEFRIENDNOTIFYBYIDRELOADER ) :
              MyUtil::Task(level),
              _host(host),_guest(guest) {
              } 
            virtual void handle(); 
          private:
            int _host;
            int _guest;
        };
 
        class AddBuddyByApplicantCacheTask : public MyUtil::Task {
          public:
            AddBuddyByApplicantCacheTask(const ::com::xiaonei::service::buddy::Applyship& apply, const Ice::Context& ctx, ::Ice::Int level=TASK_LEVEL_ADDBUDDYBYAPPLICANTCACHE) :
              MyUtil::Task(level) {
                _id = apply.applicant;
                _accepter.accepter = apply.accepter;
                MyUtil::Date date(const_cast<Ice::Context&>(ctx)["TIME"]);
                _accepter.time = date.toSeconds();

              }
            virtual void handle();
          private:
            int _id;
            ::xce::buddy::AccepterInfo _accepter;
        };

        class RemoveBuddyByApplicantCacheTask : public MyUtil::Task {
          public:
            RemoveBuddyByApplicantCacheTask(int id, int accepterId, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYBYAPPLICANTCACHE ) :
              MyUtil::Task(level),
              _id(id),
              _accepterId(accepterId) {
              }
            virtual void handle();
          private:
            int _id;
            int _accepterId;
        };

        /*
           class IncUserFriendCountTask : public MyUtil::Task {
           public:
           IncUserFriendCountTask(int id, ::Ice::Int level=TASK_LEVEL_INCUSERFRIENDCOUNT ) :
           MyUtil::Task(level),
           _id(id) {
           }
           virtual void handle();
           private:
           int _id;
           };

           class DecUserFriendCountTask : public MyUtil::Task {
           public:
           DecUserFriendCountTask(int id, ::Ice::Int level=TASK_LEVEL_DECUSERFRIENDCOUNT ) :
           MyUtil::Task(level),
           _id(id) {
           }
           virtual void handle();
           private:
           int _id;
           };
         */

        class RemoveBuddyNotifyTask : public MyUtil::Task {
          public:
            RemoveBuddyNotifyTask(int from, int to, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYNOTIFY ) :
              MyUtil::Task(level),
              _from(from),
              _to(to) {
              }
            virtual void handle();
          private:
            int _from;
            int _to;
        };

        // --------------------------------------------
        class IncUserCountGuestRequestTask : public MyUtil::Task {
          public:
            IncUserCountGuestRequestTask(int id, ::Ice::Int level=TASK_LEVEL_INCUSERCOUNTGUESTREQUEST ):
              MyUtil::Task(level),
              _id(id) {
              }
            virtual void handle();
          private:
            int _id;
        };

        class DecUserCountGuestRequestTask : public MyUtil::Task {
          public:
            DecUserCountGuestRequestTask(int id, ::Ice::Int level=TASK_LEVEL_DECUSERCOUNTGUESTREQUEST ):
              MyUtil::Task(level),
              _id(id) {
              }
            virtual void handle();
          private:
            int _id;
        };

        // ---------------------------------------

        class ApplyOfferTask : public MyUtil::Task {
          public:
            ApplyOfferTask(int applicant, int acceptor, ::Ice::Int level=TASK_LEVEL_APPLYOFFER ):
              MyUtil::Task(level),
              _applicant(applicant),
              _acceptor(acceptor) {
              }
            virtual void handle();
          private:
            int _applicant;
            int _acceptor;
        };

        class AcceptOfferTask : public MyUtil::Task {
          public:
            AcceptOfferTask(int acceptor, int applicant, ::Ice::Int level=TASK_LEVEL_ACCEPTOFFER ):
              MyUtil::Task(level),
              _acceptor(acceptor),
              _applicant(applicant) {
              }
            virtual void handle();
          private:
            int _acceptor;
            int _applicant;
        };

        class ApplyFriendFinderNewComerTask : public MyUtil::Task {
          public:
            ApplyFriendFinderNewComerTask(int applicant, int acceptor, ::Ice::Int level=TASK_LEVEL_APPLYOFFER ):
              MyUtil::Task(level),
              _applicant(applicant),
              _acceptor(acceptor) {
              }
            virtual void handle();
          private:
            int _applicant;
            int _acceptor;
        };

        class AcceptFriendFinderNewComerTask : public MyUtil::Task {
          public:
            AcceptFriendFinderNewComerTask(int acceptor, int applicant, ::Ice::Int level=TASK_LEVEL_ACCEPTOFFER ):
              MyUtil::Task(level),
              _acceptor(acceptor),
              _applicant(applicant) {
              }
            virtual void handle();
          private:
            int _acceptor;
            int _applicant;
        };

        class RemoveMSNFinderTask : public MyUtil::Task {
          public:
            RemoveMSNFinderTask(int userId, int friendId, ::Ice::Int level=TASK_LEVEL_REMOVEMSNFINDER ):
              MyUtil::Task(level),
              _userId(userId),
              _friendId(friendId) {
              }
            virtual void handle();
          private:
            int _userId;
            int _friendId;
        };

        class RemoveFriendFinderTask : public MyUtil::Task {
          public:
            RemoveFriendFinderTask(int userId, int friendId, ::Ice::Int level=TASK_LEVEL_REMOVEFRIENDFINDER ):
              MyUtil::Task(level),
              _userId(userId),
              _friendId(friendId) {
              }
            virtual void handle();
          private:
            int _userId;
            int _friendId;
        };

        class RemoveCommonFriendTask : public MyUtil::Task {
          public:
            RemoveCommonFriendTask(int userId, int friendId, ::Ice::Int level=TASK_LEVEL_REMOVECOMMOMFRIEND ):
              MyUtil::Task(level),
              _userId(userId),
              _friendId(friendId) {
              }
            virtual void handle();
          private:
            int _userId;
            int _friendId;
        };

        class RebuildFriendFinderTask : public MyUtil::Task {
          public:
            RebuildFriendFinderTask(int userId, ::Ice::Int level=TASK_LEVEL_REBUILDFRIENDFINDER ):
              MyUtil::Task(level),
              _userId(userId) {
              }
            virtual void handle();
          private:
            int _userId;
        };

        // ---------------------------------------
        class AddBuddyOnlineStatTask : public MyUtil::Task {
          public:
            AddBuddyOnlineStatTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_ADDBUDDYONLINESTAT ) :
              MyUtil::Task(level),
              _host(host),
              _guest(guest) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;

        };

        class RemoveBuddyOnlineStatTask : public MyUtil::Task {
          public:
            RemoveBuddyOnlineStatTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYONLINESTAT ) :
              MyUtil::Task(level),
              _host(host),
              _guest(guest) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;

        };

        // ---------------------------------------
        class DeliverSupplyTask : public MyUtil::Task {
          public:
            DeliverSupplyTask(int supplier, int accepter, const Ice::Context& ctx, ::Ice::Int level=TASK_LEVEL_DELIVERSUPPLY ):
              MyUtil::Task(level),
              _supplier(supplier),
              _accepter(accepter),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _supplier;
            int _accepter;
            Ice::Context _ctx;
        };

        // ---------------------------------------
        class RemoveBuddyGroupTask : public MyUtil::Task {
          public:
            RemoveBuddyGroupTask(int from, int to, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYGROUP ) :
              MyUtil::Task(level),
              _from(from),
              _to(to) {
              }
            virtual void handle();
          private:
            int _from;
            int _to;
        };

        // ---------------------------------------
        class ReloadBlockUserTask : public MyUtil::Task {
          public:
            ReloadBlockUserTask(int id, ::Ice::Int level=TASK_LEVEL_RELOADBLOCKUSER ):
              MyUtil::Task(level),
              _id(id) {
              }
            virtual void handle();
          private:
            int _id;
        };

        // ---------------------------------------
        class RemoveRecommendFriendTask : public MyUtil::Task {
          public:
            RemoveRecommendFriendTask(int ownerId, int otherId, ::Ice::Int level=TASK_LEVEL_REMOVERECOMMENDFRIEND ):
              MyUtil::Task(level),
              _ownerId(ownerId),_otherId(otherId) {
              }
            virtual void handle();
          private:
            int _ownerId;
            int _otherId;
        };

        // ---------------------------------------
        class RebuildFriendDistTask : public MyUtil::Task {
          public:
            RebuildFriendDistTask(int id, ::Ice::Int level=TASK_LEVEL_REBUILDFRIENDDIST ):
              MyUtil::Task(level),
              _id(id) {
              }
            virtual void handle();
          private:
            int _id;
        };

        // ---------------------------------------

        class AddBuddyByIdLoaderTask : public MyUtil::Task {
          public:
            AddBuddyByIdLoaderTask (int userid,int friendid, ::Ice::Int level=TASK_LEVEL_ADDBUDDYBYIDLOADER ):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };
        class RemoveBuddyByIdCacheTask : public MyUtil::Task {
          public:
            RemoveBuddyByIdCacheTask (int userid,int friendid, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYBYIDCACHE ):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };

        class AddBuddyFriendCacheTask : public MyUtil::Task {
          public:
            AddBuddyFriendCacheTask (int userid,int friendid, ::Ice::Int level= TASK_LEVEL_ADDBUDDYFRIENDCACHE):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };


        class RemoveBuddyFriendCacheTask : public MyUtil::Task {
          public:
            RemoveBuddyFriendCacheTask (int userid,int friendid, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYFRIENDCACHE ):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };

        class RemoveBuddyByNameCacheTask : public MyUtil::Task {
          public:
            RemoveBuddyByNameCacheTask (int userid,int friendid, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYBYNAMECACHE ):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };
        class RemoveBuddyByOnlineTimeCacheTask : public MyUtil::Task {
          public:
            RemoveBuddyByOnlineTimeCacheTask (int userid,int friendid, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYBYONLINETIMECACHE ):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };
        class RemoveBuddyByAddTimeCacheTask : public MyUtil::Task {
          public:
            RemoveBuddyByAddTimeCacheTask (int userid,int friendid, ::Ice::Int level=TASK_LEVEL_REMOVEBUDDYBYADDTIMECACHE ):
              MyUtil::Task(level),
              _userid(userid),
              _friendid(friendid){
              }
            virtual void handle();
          private:
            int _userid;
            int _friendid;
        };
        class DecFriendCountCacheTask : public MyUtil::Task {
          public:
            DecFriendCountCacheTask (int userid, ::Ice::Int level=TASK_LEVEL_DECFRIENDCOUNTCACHE ):
              MyUtil::Task(level),
              _userid(userid){
              }
            virtual void handle();
          private:
            int _userid;
        };
        class ReloadBuddyByIdLoaderTask : public MyUtil::Task {
          public:
            ReloadBuddyByIdLoaderTask (int id, ::Ice::Int level=TASK_LEVEL_RELOADBUDDYBYIDLOADER ):
              MyUtil::Task(level),
              _id(id) {
              }
            virtual void handle();
          private:
            int _id;
        };

        class NotifyBuddyApplyChangeTask : public MyUtil::Task {
          public:
            NotifyBuddyApplyChangeTask(int id, ::Ice::Int level=TASK_LEVEL_NOTIFYBUDDYAPPLYCHANGE ):
              MyUtil::Task(level),
              _id(id) {
              }
            virtual void handle();
          private:
            int _id;
        };

        class NotifyAntispamAddApplyTask : public MyUtil::Task {
          public:
            NotifyAntispamAddApplyTask(int applicant, int accepter, const Ice::Context& ctx,::Ice::Int level=TASK_LEVEL_NOTIFYANTISPAMADDAPPLY ):
              MyUtil::Task(level),
              _applicant(applicant),
              _accepter(accepter),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _applicant;
            int _accepter;
            Ice::Context _ctx;
        };

        class SetBuddyRelationCacheTask : public MyUtil::Task {
          public:
            SetBuddyRelationCacheTask(const Applyship& apply, ::xce::buddy::BuddyDesc desc, ::Ice::Int level=TASK_LEVEL_SETBUDDYRELATIONCACHE_APPLY) : MyUtil::Task(level) {
              _relation.from = apply.applicant;
              _relation.to = apply.accepter;
              _desc = desc;
            }

            SetBuddyRelationCacheTask(const Ice::Int from, const Ice::Int to,  ::xce::buddy::BuddyDesc desc, ::Ice::Int level=TASK_LEVEL_SETBUDDYRELATIONCACHE_FROM_TO) : MyUtil::Task(level) {
              _relation.from = from;
              _relation.to = to;
              _desc = desc;
            }

            SetBuddyRelationCacheTask(const Blockship& block, ::xce::buddy::BuddyDesc desc, ::Ice::Int level=TASK_LEVEL_SETBUDDYRELATIONCACHE_BLOCK) : MyUtil::Task(level) {
              _relation.from = block.from;
              _relation.to = block.to;
              _desc = desc;
            }

            virtual void handle();
          private:
            ::xce::buddy::Relationship _relation;
            ::xce::buddy::BuddyDesc _desc;
        };

        class RemoveCardRelationCacheTask : public MyUtil::Task {
          public:
            RemoveCardRelationCacheTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_REMOVECARDRELATIONCACHE_FRIEND ):
              MyUtil::Task(level),
              _host(host),
              _guest(guest) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;
        };

        class ChangeBuddyNotifyFeedTask : public MyUtil::Task {
          public:
            ChangeBuddyNotifyFeedTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_CHANGEBUDDYNOTIFYFEED ):
              MyUtil::Task(level),
              _host(host),
              _guest(guest) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;
        };

        class BuddyApplyNotifyFeedTask : public MyUtil::Task {
          public:
            BuddyApplyNotifyFeedTask(int host, int guest, ::Ice::Int level=TASK_LEVEL_BUDDYAPPLYNOTIFYFEED ):
              MyUtil::Task(level),
              _host(host),
              _guest(guest) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;
        };

        class CheckAddBuddyTask : public MyUtil::Task {
          public:
            CheckAddBuddyTask(const ::xce::check::OperaDataPtr& key, const std::string& value, ::Ice::Int level=TASK_LEVEL_CHECKADDBUDDY ):
              MyUtil::Task(level),
              _key(key),
              _value(value) {
              }
            virtual void handle();
          private:
            const ::xce::check::OperaDataPtr _key;
            const std::string _value;
        };

        class CheckRemoveBuddyTask : public MyUtil::Task {
          public:
            CheckRemoveBuddyTask(const ::xce::check::OperaDataPtr& key, const std::string& value, ::Ice::Int level=TASK_LEVEL_CHECKREMOVEBUDDY ):
              MyUtil::Task(level),
              _key(key),
              _value(value) {
              }
            virtual void handle();
          private:
            const ::xce::check::OperaDataPtr _key;
            const std::string _value;
        };


        class CheckAddApplyTask : public MyUtil::Task {
          public:
            CheckAddApplyTask(const Applyship& apply,  ::Ice::Int level=TASK_LEVEL_CHECKADDAPPLY ):
              MyUtil::Task(level),
              _apply(apply) {
              }
            virtual void handle();
          private:
            Applyship _apply;
        };


        class CheckRemoveApplyTask : public MyUtil::Task {
          public:
            CheckRemoveApplyTask(const Applyship& apply,  ::Ice::Int level=TASK_LEVEL_CHECKREMOVEAPPLY ):
              MyUtil::Task(level),
              _apply(apply) {
              }
            virtual void handle();
          private:
            Applyship _apply;
        };
        
        class SocalGraphLogicReportTask : public MyUtil::Task {
          public:
            SocalGraphLogicReportTask(int host, int guest, com::xiaonei::service::buddy::BuddyDesc desc, ::Ice::Int level=TASK_LEVEL_SOCALGRAPHLOGICPEPORT ):
              MyUtil::Task(level),
              _host(host),_guest(guest),_desc(desc) {
              }
            virtual void handle();
          private:
            int _host;
            int _guest;
            com::xiaonei::service::buddy::BuddyDesc _desc;
        };


        // ---------------------------------------

        // BuddyXoaLogic
        class XoaAddApplyTask : public MyUtil::Task {
          public:
            XoaAddApplyTask(int applicant, int apply, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOAADDAPPLYTASK ):
              MyUtil::Task(level),
              _applicant(applicant),
              _apply(apply),
              _ctx(ctx){
              }
            virtual void handle();
          private:
            int _applicant;
            int _apply;
            Ice::Context _ctx;
        };

        class XoaAcceptApplyTask : public MyUtil::Task {
          public:
        	XoaAcceptApplyTask(int applicant, int apply, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOAACCEPTAPPLYTASK ):
              MyUtil::Task(level),
              _applicant(applicant),
              _apply(apply),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _applicant;
            int _apply;
            Ice::Context _ctx;
        };

        class XoaDenyApplyTask : public MyUtil::Task {
          public:
        	XoaDenyApplyTask(int applicant, int apply, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOADENYAPPLYTASK ):
              MyUtil::Task(level),
              _applicant(applicant),
              _apply(apply),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _applicant;
            int _apply;
            Ice::Context _ctx;
        };

        class XoaAddFriendTask : public MyUtil::Task {
          public:
        	XoaAddFriendTask(int from, int to, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOAADDFRIENDTASK ):
              MyUtil::Task(level),
              _from(from),
              _to(to),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _from;
            int _to;
            Ice::Context _ctx;
        };

        class XoaRemoveFriendTask : public MyUtil::Task {
          public:
        	XoaRemoveFriendTask(int from, int to, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOAREMOVEFRIENDTASK ):
              MyUtil::Task(level),
              _from(from),
              _to(to),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _from;
            int _to;
            Ice::Context _ctx;
        };

        class XoaAddBlockTask : public MyUtil::Task {
          public:
        	XoaAddBlockTask(int block, int blocked, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOAADDBLOCKTASK ):
              MyUtil::Task(level),
              _block(block),
              _blocked(blocked),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _block;
            int _blocked;
            Ice::Context _ctx;
        };

        class XoaRemoveBlockTask : public MyUtil::Task {
          public:
        	XoaRemoveBlockTask(int block, int blocked, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOAREMOVEBLOCKTASK ):
              MyUtil::Task(level),
              _block(block),
              _blocked(blocked),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _block;
            int _blocked;
            Ice::Context _ctx;
        };
  
        class XoaReloadTask : public MyUtil::Task {
          public:
          XoaReloadTask(int userId, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_XOARELOADTASK ):
              MyUtil::Task(level),
              _userId(userId),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _userId;
            Ice::Context _ctx;
        };

        class RemoveRcdCacheTask : public MyUtil::Task {
          public:
          RemoveRcdCacheTask(int host, int guest, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_REMOVERCDCACHETASK ):
              MyUtil::Task(level),
              _host(host),
			  _guest(guest),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _host;
			int _guest;
            Ice::Context _ctx;
        };

        class BecomeFriendsTask : public MyUtil::Task {
          public:
          BecomeFriendsTask(int host, int guest, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_BECOMEFRIENDSTASK ):
              MyUtil::Task(level),
              _host(host),
			  _guest(guest),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _host;
			int _guest;
            Ice::Context _ctx;
        };

        class BlockTask : public MyUtil::Task {
          public:
          BlockTask(int host, int guest, const Ice::Context& ctx, ::Ice::Int level = TASK_LEVEL_BLOCKTASK ):
              MyUtil::Task(level),
              _host(host),
			  _guest(guest),
              _ctx(ctx) {
              }
            virtual void handle();
          private:
            int _host;
			int _guest;
            Ice::Context _ctx;
        };
    
        // ---------------------------------------

      }
    }
  }
}

#endif
