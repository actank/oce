#ifndef BUDDYGROUPI_H_
#define BUDDYGROUPI_H_

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include "BuddyGroup.h"
#include "ServiceI.h"
#include <set>
#include <vector>
#include <map>
#include <IceUtil/Mutex.h>
#include <Singleton.h>
#include <algorithm>
#include "BuddyGroupWrapper.h"

namespace com {
  namespace xiaonei {
    namespace model {
      using namespace std;
      using namespace MyUtil;

      class BuddyGroupManagerI : public BuddyGroupManager,
      public Singleton<BuddyGroupManagerI> {
        public:
          virtual void addGroup(Ice::Int host, const string& groupName,
              const Ice::Current& = Ice::Current());

          virtual void renameGroup(Ice::Int host, const string& oldName,
              const string& newName, const Ice::Current& = Ice::Current());
          virtual void renameGroupWithGid(Ice::Int host, Ice::Int groupId,
              const string& newName, const Ice::Current& = Ice::Current());

          virtual void removeGroup(Ice::Int host, const string& groupName,
              const Ice::Current& = Ice::Current());
          virtual void removeGroupWithGid(Ice::Int host, Ice::Int groupId,
              const Ice::Current& = Ice::Current());

          virtual void removeGroupForce(Ice::Int host, const string& groupName,
              const Ice::Current& = Ice::Current());

          virtual void cleanGroup(Ice::Int host, const string& groupName,
              const Ice::Current& = Ice::Current());
          virtual void cleanGroupWithGid(Ice::Int host, Ice::Int groupId,
              const Ice::Current& = Ice::Current());

          virtual void addBuddyToGroup(Ice::Int host, const string& groupName,
              Ice::Int buddy, const Ice::Current& = Ice::Current());
          virtual void addBuddyToGroupWithGid(Ice::Int host, Ice::Int groupId,
              Ice::Int buddy, const Ice::Current& = Ice::Current());

          virtual void addBuddiesToGroup(Ice::Int host, const string& groupName,
              const MyUtil::IntSeq & buddies, const Ice::Current& = Ice::Current());
          virtual void addBuddiesToGroupWithGid(Ice::Int host, Ice::Int groupId,
              const MyUtil::IntSeq & buddies, const Ice::Current& = Ice::Current());

          virtual void setBuddiesToGroup(Ice::Int host, const string& groupName,
              const MyUtil::IntSeq & buddies, const Ice::Current& = Ice::Current());
          virtual void setBuddiesToGroupWithGid(Ice::Int host, Ice::Int groupId,
              const MyUtil::IntSeq & buddies, const Ice::Current& = Ice::Current());

          virtual void removeBuddyFromGroup(Ice::Int host, const string& groupName,
              Ice::Int buddy, const Ice::Current& = Ice::Current());
          virtual void removeBuddyFromGroupWithGid(Ice::Int host, Ice::Int groupId,
              Ice::Int buddy, const Ice::Current& = Ice::Current());

          virtual void removeBuddy(Ice::Int host, Ice::Int buddy,
              const Ice::Current& = Ice::Current());

          virtual int changeBuddyGroups(Ice::Int host, Ice::Int buddy,
              const MyUtil::StrSeq& nameSeq, const Ice::Current& = Ice::Current());
          virtual int changeBuddyGroupsWithGid(Ice::Int host, Ice::Int buddy,
              const MyUtil::IntSeq& groupIdSeq, const Ice::Current& = Ice::Current());

          virtual void changeBuddyGroupsForWeb(Ice::Int host, Ice::Int buddy,
              const StrSeq& nameSeq, const Ice::Current& = Ice::Current());
          virtual void changeBuddyGroupsForWebWithGid(Ice::Int host, Ice::Int buddy,
              const IntSeq& groupIdSeq, const Ice::Current& = Ice::Current());

          //	virtual int buddyChangeGroups(Ice::Int host, Ice::Int buddy,
          //			const MyUtil::StrSeq& nameSeq, const Ice::Current& = Ice::Current());
          virtual BuddiedGroupsMap getBuddiedGroupsMap(Ice::Int host,
              const Ice::Current& = Ice::Current());
          virtual BuddyGroupsMap getBuddyGroupsMap(Ice::Int host,
              const Ice::Current& = Ice::Current());
          virtual MyUtil::StrSeq getGroupList(Ice::Int host,
              const Ice::Current& = Ice::Current());
          virtual MyUtil::Int2StrMap getGroupListData(Ice::Int host,
              const Ice::Current& = Ice::Current());
          virtual MyUtil::IntSeq getBuddyList(Ice::Int host, const string& groupName,
              Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());
          virtual MyUtil::IntSeq getBuddyListWithGid(Ice::Int host, const Ice::Int groupId,
              Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());
          virtual Ice::Int getCountByGroup(Ice::Int host, const string& groupName,
              const Ice::Current& = Ice::Current());

          virtual Ice::Int getCountByGroupId(Ice::Int host, const Ice::Int groupId,
              const Ice::Current& = Ice::Current());

          virtual MyUtil::StrSeq getBuddyGroups(Ice::Int host, Ice::Int guest,
              const Ice::Current& = Ice::Current());
          virtual MyUtil::Int2StrMap getBuddyGroupsData(Ice::Int host, Ice::Int guest,
              const Ice::Current& = Ice::Current());

          virtual MyUtil::Str2IntMap getAllGroupsBuddyCount(int host,
              const Ice::Current& = Ice::Current());
          virtual MyUtil::Int2IntMap getAllGroupsBuddyCountData(int host,
              const Ice::Current& = Ice::Current());

          virtual void load(Ice::Int host, const Ice::Current& = Ice::Current());
          virtual void saveLoad(::Ice::Int, const Ice::Current& = Ice::Current());

          static string getTableName(const string & table, int host) {
            ostringstream tableName;
            tableName<<table<<"_"<<(host%100);
            return tableName.str();
          }

          void notifyIm(bool notifyIm) {
            _notifyIm = notifyIm;
          }

        protected:
          //	IceUtil::Mutex _changeGroupLock;
          void _removeGroup(Ice::Int host, const string& groupName);
          void _cleanGroup(Ice::Int host, const string& groupName);
          MyUtil::StrSeq _getBuddyGroups(Ice::Int host, Ice::Int guest);
          bool _notifyIm;
      };

      class BuddyGroupFactoryI : public ServantFactory {
        public:
          virtual Ice::ObjectPtr create(Ice::Int id);
      };
      typedef IceUtil::Handle<BuddyGroupFactoryI> BuddyGroupFactoryIPtr;

      class CleanIllegalDataTask : public MyUtil::Task {
        int _host;
        map<int,string> _groups;
        public:
        CleanIllegalDataTask(int host, map<int,string> groups) :
          _host(host) {
            _groups.swap(groups);
          }
        ;
        virtual void handle();

      };


    }
    ;
  }
  ;
}
;

#endif /*BUDDYGROUPI_H_*/
