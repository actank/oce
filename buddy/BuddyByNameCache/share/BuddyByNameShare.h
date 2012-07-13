#ifndef __BUDDYBYNAMESHARE_H__
#define __BUDDYBYNAMESHARE_H__

#include <Singleton.h>
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByNameReplicaAdapter.h"
#include <IceUtil/IceUtil.h>
#include "UserCacheAdapter.h"
#include "UserCache.h"
#include "BuddyByNameCache.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>

namespace xce {
  namespace buddybyname {

    using namespace xce::buddy;
    using namespace xce::usercache;
    using namespace xce::buddy::adapter;
    using namespace MyUtil;
    using namespace com::xiaonei::xce;
    using namespace std;
    using namespace mysqlpp;


    const size_t BUDDY_LIMIT = 300;

    class NameNoteI : virtual public Ice::Object {
      public:
        NameNoteI() {}
        ~NameNoteI() {}

        int id;
        string name;
    };
    typedef IceUtil::Handle<NameNoteI> NameNoteIPtr;

    //-----------------------------------BatchUserNamesResultHandler-----------------------------------------------
    class BatchUserNamesResultHandler : virtual public com::xiaonei::xce::ResultHandler {
      public:
        BatchUserNamesResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){
          MCE_DEBUG(" result.size " << _result->data.size());
        }
        virtual ~BatchUserNamesResultHandler(){}
      protected:
        virtual bool handle(mysqlpp::Row& row) const {
          int id = (int) row["id"];
          MCE_DEBUG("sqlFrom id " << id << " result.size " << _result->data.size());
          MyUtil::ObjectMap::iterator it = _result->data.find(id);
          if (it != _result->data.end()) {
            MCE_WARN("BatchUserNamesResultHandler::handle -> Same Id: " <<  id);
          } else {
            NameNoteIPtr note = new NameNoteI;
            note->id = id;
#ifndef NEWARCH
            note->name = row["name"].get_string();
#else
            note->name = row["name"].c_str();
#endif
            _result->data.insert(make_pair(id,note));
            MCE_DEBUG("sqlFrom id " << note->id << " name " << note->name << " result.size " << _result->data.size());
          }
          return true;
        }
      private:
        MyUtil::ObjectResultPtr _result;
    };

    //-----------------------------------BatchUserNamesResultHandler end-----------------------------------------------

    //-----------------------------------BuddyHelper-----------------------------------------------

    class BuddyHelper: public MyUtil::Singleton<BuddyHelper> {
      public:
        std::map<int,UserCachePtr> getUserCache(const MyUtil::IntSeq& buddies) {
          std::map<int, UserCachePtr> res;
          if( buddies.empty() ){
            return res;
          }
          MyUtil::IntSeq::const_iterator beginIt = buddies.begin();
          while( beginIt != buddies.end() ){
            std::map<int, UserCachePtr> propMap;
            if( beginIt+BUDDY_LIMIT >= buddies.end()-1 ){
              try{
                propMap = UserCacheAdapter::instance().GetUserCache(
                    MyUtil::IntSeq( beginIt, buddies.end() )
                    );
              }catch(Ice::Exception& e){
                MCE_WARN( "BuddyHelper getUserCache handle Ice::Exception " << e.what());
              }catch(std::exception& e){
                MCE_WARN( "BuddyHelper getUserCache handle std::exception " << e.what());
              }catch(...){
                MCE_WARN( "BuddyHelper getUserCache handle unknown exception.");
              }
              beginIt = buddies.end();
            } else{
              try{
                propMap = UserCacheAdapter::instance().GetUserCache(
                    MyUtil::IntSeq( beginIt, beginIt+BUDDY_LIMIT )
                    );
              }catch(Ice::Exception& e){
                MCE_WARN( "BuddyHelper getUserCache handle Ice::Exception " << e.what());
              }catch(std::exception& e){
                MCE_WARN( "BuddyHelper getUserCache handle std::exception " << e.what());
              }catch(...){
                MCE_WARN( "BuddyHelper getUserCache handle unknown exception.");
              }
              beginIt = beginIt + BUDDY_LIMIT;
            }
            res.insert( propMap.begin(), propMap.end() );
          }
          return res;
        }

        NameNoteSeq getNameNoteSeq(const MyUtil::IntSeq& buddies, const std::map<int, xce::usercache::UserCachePtr>& propMap) {
          NameNoteSeq res;
          for(MyUtil::IntSeq::const_iterator it=buddies.begin();it!=buddies.end();++it){
            NameNote note;
            note.id = *it;
            std::map<int, UserCachePtr>::const_iterator itMap = propMap.find(*it);
            if( (itMap != propMap.end()) && (0 != itMap->second) ){
              note.name = itMap->second->name();
            } else{
              note.name = "";
            }
            res.push_back( note );
          }
          return res;
        }

        NameNoteSeq getNameNoteSeqFromDB(const MyUtil::IntSeq& buddies) {
          NameNoteSeq final_result;

          if (buddies.empty()) 
            return final_result;

          ObjectResultPtr result = new ObjectResult;
          Statement sql;

          MyUtil::IntSeq::const_iterator it = buddies.begin();
          if( it == buddies.end() ){
            return final_result;
          }

          ostringstream ids;
          ids << *it;
          for(;it!=buddies.end();++it){
            ids << ", " << *it;
          }

          sql << "SELECT id, name FROM user_names WHERE id IN (" << ids.str() << ")";

          /*
             sql << "SELECT id, name FROM user_names WHERE id IN (";
             for (MyUtil::IntSeq::const_iterator it = buddies.begin(); it != buddies.end(); ++it) {
             if (it != buddies.begin()) {
             sql << ",";
             }
             sql << *it;
             }
             sql << ")";
           */
          BatchUserNamesResultHandler handler(result);
          QueryRunner("usercache_names_preloader",CDbRServer).query(sql, handler);

          ostringstream nameInfo;
          for(MyUtil::IntSeq::const_iterator it=buddies.begin();it!=buddies.end();++it){
            NameNote note;
            note.id = *it;
            std::map<long, Ice::ObjectPtr>::const_iterator itMap = result->data.find(*it);
            if( (itMap != result->data.end()) && (0 != itMap->second) ){
              note.name = NameNoteIPtr::dynamicCast(itMap->second)->name;
            } else{
              note.name = "";
            }
            nameInfo << note.id << "(" << note.name << ") ";
            final_result.push_back( note );
          }
          MCE_DEBUG("buddiesSize/resutlSize " << buddies.size() << "/" << result->data.size() << " names: " << nameInfo.str());
          return final_result;
        }
    };
    //-----------------------------------BuddyHelper end-----------------------------------------------

    //-----------------------------------setDataHelper-----------------------------------------------
    class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
      public:
        void initialize() {
          setter_ = new SetDataThread(*this);
          setter_->start(1024*1024).detach();
        }

        void invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ) {
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
          datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
          if (datas_.size() >= 1000) mutex_.notify();
        }

        void invokeNow() {
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
          mutex_.notify();
        }

        void invokeNow( Ice::Int id, const Ice::ObjectPtr& o ) {
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
          datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
          mutex_.notify();
        }

        void swap(std::vector< pair<long,Ice::ObjectPtr> >& empty_vector) {
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
          if (datas_.size() < 1000) mutex_.timedWait(IceUtil::Time::seconds(1));
          datas_.swap(empty_vector);
        }
      private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector< pair<long,Ice::ObjectPtr> > datas_;

        class SetDataThread: virtual public IceUtil::Thread {
          public:
            SetDataThread(SetDataHelper& helper) :
              helper_(helper) {
              }
            virtual void run() {
              while (true){
                std::vector< pair<long,Ice::ObjectPtr> > to_be_set_;
                helper_.swap( to_be_set_ );
                if( to_be_set_.empty() ){
                  MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs." );
                  continue;
                }
                ostringstream idstr;
                MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
                for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator it = to_be_set_.begin();
                    it != to_be_set_.end(); ++it ){
                  std::pair<std::map<long,Ice::ObjectPtr>::iterator,bool> ret;
                  ret = objs->data.insert(*it);
                  if(!ret.second){
                    MCE_WARN("[SetDataHelper::SetDataThread::run] setData: " << it->first << " insert " 
                        << BuddyByNameDataPtr::dynamicCast( it->second )->names.size() << " failed.");
                    objs->data[it->first] = it->second;
                  }
                  idstr << " " << it->first << "(" << BuddyByNameDataPtr::dynamicCast( it->second )->names.size() << ")";
                }
                MCE_INFO("[SetDataHelper::SetDataThread::run] setData:" << idstr.str() );
                BuddyByNameCacheAdapter::instance().setData(objs);
              }
            }
          private:
            SetDataHelper& helper_;
        };
        typedef IceUtil::Handle<SetDataThread> SetDataThreadPtr;

        SetDataThreadPtr setter_;
    };
    //-----------------------------------setDataHelper end-----------------------------------------------

  }
}
#endif
