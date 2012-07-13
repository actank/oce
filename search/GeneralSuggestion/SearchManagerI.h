#ifndef __GENERAL_SUGGESTION_MANAGER_H__
#define __GENERAL_SUGGESTION_MANAGER_H__

#include <GeneralSuggestion.h>
#include "IceExt/src/ServiceI.h"
#include "util/cpp/TimeCost.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <IceUtil/IceUtil.h>
#include "search/GeneralSuggestion/SearchUtil.h"
#include "search/GeneralSuggestion/SearchDataI.h"
#include <algorithm>

namespace xce {
  namespace generalsuggestion {

    using namespace MyUtil;
    using namespace xce::search;
    using namespace std;
    using namespace xce::suggestiondata;

    const size_t maxSize = 300;

    class SearchManagerI : public SearchManager, public MyUtil::Singleton<SearchManagerI> {		
      public:
        SearchManagerI(){};
        virtual ~SearchManagerI(){};
        virtual SuggestionResultPtr search(int,const string&,int begin, int limit, const StrSeq&, const Ice::Current& crt=Ice::Current());
        virtual void load(int,const IndexItemSeq&, const MyUtil::Str2IntMap& config,const Ice::Current& crt=Ice::Current());
        void load(string type, int id);
        void reload(string type, int id);
        set<string> newSet;
        void initialize();

        SearchDataIPtr getCachedData(string key) {
          SearchDataIPtr sp = static_cache_.locate(key, 0);
          if (!sp) {
            sp = dynamic_cache_.locate(key, 0);
          }
          return sp;
        }

        void addDynamicData(string key, SearchDataIPtr sp) {
          dynamic_cache_.add(sp, key);
        }

        void addStaticData(string key, SearchDataIPtr sp) {
          static_cache_.add(sp, key);
        }

      private:
        SizedEvictor<string,SearchDataIPtr> static_cache_;
        SizedEvictor<string,SearchDataIPtr> dynamic_cache_;
        friend class MyUtil::Singleton<SearchManagerI>;
        map<string,vector<string> > dict_;
    };
    typedef IceUtil::Handle<SearchManagerI> SearchManagerIPtr;

    class LoadThread : public virtual IceUtil::Thread {
      private:
        int id_;
        string type_;
      public:
        LoadThread(string type, int id) : id_(id), type_(type) {
        }

        void run() {
          ostringstream os;
          os << "Update " << type_ << " ID: " << id_;
          TimeCost tc = TimeCost::create(os.str(), 1);
          string key = StrUtil::getKey(type_, id_);

          SearchDataIPtr sp = NULL;
          SearchDataIPtr oldSp = SearchManagerI::instance().getCachedData(key);

          // 动态数据有可能被逐出
          if (oldSp) {
            sp = oldSp->clone();
          } else if (SearchDataIUtil::isDynamicData(type_)) {
            sp = SearchDataIUtil::getSearchDataIPtr(type_);
          } else {
            MCE_INFO("Object Not Exist In Cache For " << type_);
            return;
          }
          sp->load(id_);
          if (SearchDataIUtil::isDynamicData(type_)) {
            SearchManagerI::instance().addDynamicData(key, sp);
          } else {
            SearchManagerI::instance().addStaticData(key, sp);
          }
          tc.step("Time: ");
          return;
        }
    };
    typedef IceUtil::Handle<LoadThread> LoadThreadPtr;
  };
};
#endif
