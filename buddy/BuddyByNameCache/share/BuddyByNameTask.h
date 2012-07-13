#ifndef __BUDDYBYNAMETASK_H__
#define __BUDDYBYNAMETASK_H__

#include "BuddyByNameFactory.h"
#include "BuddyByNameReplicaAdapter.h"

namespace xce {
  namespace buddybyname {

    using namespace xce::buddy;
    using namespace xce::usercache;

    const int TASK_LEVEL_BUDDY_BYNAME_LOADER_LOAD = 1;
    const int TASK_LEVEL_BUDDY_BYNAME_LOADER_RELOAD = 2;
    const int TASK_LEVEL_BUDDY_BYNAME_LOADER_RENAME = 3;

    //-----------------------------------ReloadTask-----------------------------------------------
    class ReloadTask : virtual public MyUtil::Task {
      public:
        ReloadTask( Ice::Int id,
            Ice::Int level=TASK_LEVEL_BUDDY_BYNAME_LOADER_RELOAD ) :
          Task(level),_id(id){
          }
      protected:
        virtual void handle() {
          //BuddyByIdDataPtr obj;
          BuddyByNameDataPtr obj;
          try {
            //obj = BuddyByIdDataPtr::dynamicCast( BuddyByNameFactory::instance().create(_id) );
            obj = BuddyByNameDataPtr::dynamicCast(BuddyByNameFactory::instance().create(_id));
          } catch (Ice::Exception& e) {
            MCE_WARN("[ReloadTask] Ice::Exception (create:" << _id << ") : " << e.what());
            return;
          } catch (mysqlpp::Exception& e) {
            MCE_WARN("[ReloadTask] mysqlpp::Exception (create:" << _id << ") : " << e.what());
            return;
          } catch (std::exception& e) {
            MCE_WARN("[ReloadTask] std::excetpion (create:" << _id << ") : " << ": " << e.what());
            return;
          } catch (...) {
            MCE_WARN("[ReloadTask] unknown exception (create:" << _id << ") ." );
            return;
          }
          if( obj == 0 ){
            return;
          }
          MCE_INFO("ReloadTask " << _id << obj->names.size());
          SetDataHelper::instance().invokeDelay(_id,obj);
          //SetDataHelper::instance().invokeNow(_id,obj);
        }
      private:
        Ice::Int _id;
    };
    //-----------------------------------ReloadTask end-----------------------------------------------

    //-----------------------------------LoadTask-----------------------------------------------
    class LoadTask : virtual public MyUtil::Task {
      public:
        LoadTask( const MyUtil::Int2IntSeqMap& lists, 
            Ice::Int level=TASK_LEVEL_BUDDY_BYNAME_LOADER_LOAD ) :
          Task(level),_lists(lists){
          }
      protected:
        virtual void handle() {
          for( MyUtil::Int2IntSeqMap::const_iterator it = _lists.begin(); it != _lists.end(); ++it ){
            BuddyByNameDataPtr obj = new BuddyByNameData;
            BuddyByNameFactory::instance().getNameNoteSeq(it->second).swap(obj->names);
            MCE_INFO("LoadTask " << it->first <<"(" << it->second.size()<< ")");
            SetDataHelper::instance().invokeDelay(it->first,obj);
          }
          SetDataHelper::instance().invokeNow();
        }
      private:
        MyUtil::Int2IntSeqMap _lists;
    };
    //-----------------------------------LoadTask end-----------------------------------------------

    //-----------------------------------RenameTask-----------------------------------------------
    class RenameTask: virtual public MyUtil::Task {
      public:
        RenameTask( const MyUtil::IntSeq& buddies, Ice::Int theRenameOne, const std::string name,
            Ice::Int level=TASK_LEVEL_BUDDY_BYNAME_LOADER_RENAME ) :
          Task(level),_buddies(buddies),_theRenameOne(theRenameOne),_name(name){
          }
      protected:
        virtual void handle() {
          MyUtil::Int2IntSeqMap lists = BuddyByNameCacheAdapter::instance().getFriendLists( _buddies );

          for( MyUtil::Int2IntSeqMap::iterator it = lists.begin(); it != lists.end(); ++it ){
            if( it->second.empty() ){
              continue;
            }
            MyUtil::IntSeq::iterator itName = find( it->second.begin(),it->second.end(),_theRenameOne );
            if( itName!=it->second.end() ){
              it->second.erase(itName);
              BuddyByNameDataPtr obj = new BuddyByNameData;
              NameNoteSeq res = BuddyHelper::instance().getNameNoteSeqFromDB(it->second);
              NameNote note;
              note.id = _theRenameOne;
              note.name= _name;
              res.push_back( note );
              res.swap( obj->names );
              MCE_INFO("RenameTask " << it->first <<"(" << it->second.size()<< ")");
              SetDataHelper::instance().invokeDelay(it->first,obj);
            } else{
              MCE_WARN("RenameTask " << it->first <<"(" << it->second.size()<< ")  don't find rename id " 
                  << _theRenameOne << ", so do nothing!");
            }
            //		std::map<int, UserCachePtr> propMap = BuddyHelper::instance().getUserCache(it->second);
            /*
               NameCnvtrPtr cc = new NameCnvtr("utf-8","big5","gb2312");
               std::multimap<std::string,int,KeyCmp> friendMap = BuddyHelper::instance().getBuddyMapByName(it->second,propMap,cc);
               string newName = BuddyHelper::instance().cnvt(_name,cc);
               friendMap.insert( std::multimap<string,int>::value_type(newName,_theRenameOne) );
               BuddyByNameDataPtr obj = new BuddyByNameData;
               BuddyHelper::instance().getBuddyListByName(friendMap).swap( obj->friends );
               SetDataHelper::instance().invokeDelay(it->first,obj);
             */
          }
        }
      private:
        MyUtil::IntSeq _buddies;
        Ice::Int _theRenameOne;
        std::string _name;
    };
    //-----------------------------------RenameTask end-----------------------------------------------

  }
}
#endif
