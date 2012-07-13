#ifndef BUDDYGROUPWRAPPER_H_
#define BUDDYGROUPWRAPPER_H_

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <set>
#include <map>
#include <IceUtil/Mutex.h>
#include <Singleton.h>
#include "BinString.h"

namespace com {
  namespace xiaonei {
    namespace model {

#define GET_INT_VALUE(x) do {\
  x = *(int*)p;\
  p+=sizeof(int);\
  total -= sizeof(int);\
  if (total < 0) { \
    MCE_WARN("GET_INT_VALUE is Error! total<0"); \
    return ;\
  }\
}while(0)

#define GET_STRING_VALUE(x) do {\
  int strlen = *(int*)p;\
  p+=sizeof(int);\
  total -= sizeof(int);\
  if (total < 0 || strlen < 0) { \
    MCE_WARN("total=" << total << "strlen =" << strlen); \
    return ;\
  }\
  total -= strlen;\
  if (total < 0) { \
    MCE_WARN("total < 0"); \
    return ;\
  }\
  x = string(p, strlen);\
  p+=strlen;\
}while(0)


    class BuddyGroup : public Ice::Object {
      public:
        int groupId;
        std::set<int> buddyIds;
    };

typedef IceUtil::Handle<BuddyGroup> BuddyGroupPtr;

typedef std::map<std::string,BuddyGroupPtr> BuddyGroupMap;

typedef std::map<int,std::string> BuddyGroupIdMap;

class GroupMapWrapper : public Ice::Object {
  public:
    GroupMapWrapper(){
    }
    GroupMapWrapper(::xce::binstring::BinStringPtr& bp){
      unSerialize(bp);
    }

    ::xce::binstring::BinStringPtr serialize() /*const*/
    {
      ::xce::binstring::BinStringPtr binString = new ::xce::binstring::BinString();

      binString->add((int)value.size());
      //MCE_DEBUG("[GroupMapWrapper::serialize] mapsize: " + value.size());	
      for(BuddyGroupMap::const_iterator it = value.begin();
          it != value.end(); ++it) {
        //MCE_DEBUG("[GroupMapWrapper::serialize] groupname: "  + it->first);
        //MCE_DEBUG("[GroupMapWrapper::serialize] groupid: "  + it->second->groupId);
        //MCE_DEBUG("[GroupMapWrapper::serialize] buddysize: "  + it->second->buddyIds.size());
        binString->add(it->first);
        binString->add((int)it->second->groupId);
        binString->add((int)it->second->buddyIds.size());
        for(std::set<int>::const_iterator sit = it->second->buddyIds.begin();
            sit != it->second->buddyIds.end(); ++sit){
          //MCE_DEBUG("[GroupMapWrapper::serialize] buddy: "  + *sit);
          binString->add((int)*sit);
        }
      }
      binString->add((int)idmap.size());	
      for(BuddyGroupIdMap::const_iterator it = idmap.begin();
          it != idmap.end(); ++it) {
        binString->add((int)it->first);
        binString->add(it->second);
      }

      binString->compress();

      return binString;
    }	

    void unSerialize(::xce::binstring::BinStringPtr& bp)
    {
      /*char* p = NULL;

        int total = 0;

        bp->unCompress(&p, &total);

        if( p == NULL || total == 0)
        {
        MCE_WARN("GroupMapWrapper::unSerialize can't locate data from memery!");
        return;
        }*/

      string data = bp->unCompress();
      char *p = NULL;
      int total = 0;
      try {
        p = new char[data.length()];
        memcpy(p, data.c_str(), data.length());
        total = data.length();
      } catch (std::bad_alloc) {
        MCE_WARN("BuddyGroupWarpper::unSerialize out of memory ");
        return;
      }

      if( p == NULL || total == 0) 
      {
        MCE_WARN("GroupMapWrapper::unSerialize can't locate data from memery!");
        return;
      }	
      char *op = p;

      int mapsize=0;
      GET_INT_VALUE(mapsize);

      //todo
      std::string groupname = "";
      BuddyGroupPtr buddygroupptr = NULL;
      int buddygroupsize = 0;
      int buddygroupid = 0;
      for( int i=0;i <mapsize; i++){
        GET_STRING_VALUE(groupname);
        try{
          buddygroupptr = new BuddyGroup();
          GET_INT_VALUE(buddygroupptr->groupId);
          GET_INT_VALUE(buddygroupsize);
          for( int j=0; j< buddygroupsize; j++){
            GET_INT_VALUE(buddygroupid);
            buddygroupptr->buddyIds.insert(buddygroupid);	
          }	
        } catch (std::bad_alloc) {
          //打印日志
          MCE_WARN("GroupMapWrapper::unSerialize out of memory " );
          break;
        }
        value[groupname] = buddygroupptr;
      }
      int mapidsize = 0;
      GET_INT_VALUE(mapidsize);
      for( int i=0;i <mapidsize; i++){
        GET_INT_VALUE(buddygroupid);
        GET_STRING_VALUE(groupname);
        idmap[buddygroupid] = groupname;
      }
      delete op;
    }

  public: 
    BuddyGroupMap value;
    BuddyGroupIdMap idmap;	
};
typedef IceUtil::Handle<GroupMapWrapper> GroupMapWrapperPtr;

class BuddyGroupData : public Ice::Object, public IceUtil::Mutex {
  public:
    BuddyGroupData(){
    }
    BuddyGroupData(::xce::binstring::BinStringPtr& bp)
      : _bp(bp){
      }

    ::xce::binstring::BinStringPtr getBP() const{
      return _bp;
    }
    void setBP(::xce::binstring::BinStringPtr& bp) {
      _bp = bp;
    }

    ::xce::binstring::BinStringPtr saveGetBP() const{
      IceUtil::Mutex::Lock lock(*this);
      return _bp;
    }
    void saveSetBP(::xce::binstring::BinStringPtr& bp) {
      IceUtil::Mutex::Lock lock(*this);
      _bp = bp;
    }

  private:	
    ::xce::binstring::BinStringPtr _bp;
};
typedef IceUtil::Handle<BuddyGroupData> BuddyGroupDataPtr;


};
};
};
#endif
