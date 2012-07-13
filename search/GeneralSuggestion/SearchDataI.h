#ifndef __GENERAL_SEARCH_DATA_H__
#define __GENERAL_SEARCH_DATA_H__

#include <GeneralSuggestion.h>
#include "IceExt/src/ServiceI.h"
#include "util/cpp/TimeCost.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <IceUtil/IceUtil.h>
#include "search/GeneralSuggestion/SearchUtil.h"
#include "FriendOfFriendIndexerAdapter.h"
#include <algorithm>

namespace xce {
  namespace suggestiondata {

    using namespace MyUtil;
    using namespace xce::search;
    using namespace std;

    const int FRIENDINPUT_DATA = 0;
    const string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";

    const static int STATIC_DATA_TIMEOUT = 60 * 60 * 24;
    const static int DYNAMIC_DATA_TIMEOUT = 1 * 60;

    const static string FRIEND = "friend_name";
    const static string UNIVERSITY = "university_name";
    const static string WORKPLACE = "workplace_name";
    const static string APP = "app_name";
    const static string PAGE = "page_name";
    const static string RECOMMEND_USER = "recommend_user_name";

    class SearchDataI;
    typedef IceUtil::Handle<SearchDataI> SearchDataIPtr;
        
    class StrUtil {
      public:
        static  bool startsWith(const string& base, const string& pre) {
          if (base.size()<pre.size()) {
            return false;
          }
          int pc=pre.size();
          int po=0;
          while (--pc >= 0) {
            if (base[po] != pre[po++] ) {
              return false;
            }
          }
          return true;
        }
        static bool isAlphabet(const string& ch){
          if((ch >="a" && ch <= "z")||(ch >= "0" && ch <= "9"))
            return true;
          else
            return false;
        }
        static string lower(string& str){
          return str;
        }

        static bool mayPinyin(const string& str, const vector<string>& words) {
          if(words.size()<str.size()){
            return false;
          } else {
            for(size_t i=0;i<words.size();++i)
              if(words[i]<"a"||words[i]>"z")
                return false;
            return true;
          }
        }

        static string getKey(string type, int userid) {
          string key = type;
          if (type == FRIEND || type == RECOMMEND_USER) {
            ostringstream oss;
            oss<< userid << "_" << type;
            key = oss.str();
          }
          return key;
        }
    };

    class PostingsList {
      public:
        PostingsList(bool isPinyin):isPinyin_(isPinyin){
        }
        ScorerPtr getScorer(const vector<string>& words, size_t limit){
          vector<ScorerPtr> sps;
          size_t count = 0;
          for(vector<string>::const_iterator it=words.begin();it!=words.end() && count < limit;++it,++count){
            ScorerPtr sp = 0; 
            bool exist = false;
            if(isPinyin_){
              sp = new PrefixScorer;
              map<string,DocInfoListPtr>::iterator itt=invertIndex_.lower_bound(*it);
              while(itt!=invertIndex_.end() && StrUtil::startsWith(itt->first,*it)){
                sp->addScorer(new Scorer(itt->second)); 
                exist = true;
                ++itt;
              }
            }else{
              map<string,DocInfoListPtr>::iterator itt=invertIndex_.find(*it);
              if(itt!=invertIndex_.end()){
                sp = new Scorer(itt->second);
                exist = true;
              } 
            } 
            if(exist){
              sps.push_back(sp);
            }else{
              break;
            } 
          }
          if(sps.size() == limit){
            if(limit==1){
              return sps[0];
            }else{
              MCE_DEBUG("new phrase query size " << limit);
              PhraseScorerPtr psp = new PhraseScorer();
              for(vector<ScorerPtr>::iterator it=sps.begin();it!=sps.end();++it){
                psp->addScorer(*it);
              }
              return psp;
            }
          }else{
            return 0;
          }
        }
        void addTerm(string word,int doc,int pos){
          map<string,DocInfoListPtr>::iterator it=invertIndex_.find(word);
          if(it == invertIndex_.end()){
            pair<map<string,DocInfoListPtr>::iterator,bool> res = invertIndex_.insert(make_pair<string,DocInfoListPtr>(word,new    DocInfoList));
            if(res.second)
              it =res.first;
          }
          it->second->push(doc,pos);
        }

      private:
        bool isPinyin_;
        map<string,DocInfoListPtr> invertIndex_;
    };

    class StringSeq : public IceUtil::Shared{
      public:
        vector<string> seq_;
    };
    typedef IceUtil::Handle<StringSeq> StringSeqPtr;

    class PinYinResultHandler : virtual public com::xiaonei::xce::ResultHandler,
    public MyUtil::Singleton<PinYinResultHandler> {
      public:
        PinYinResultHandler(map<string,StringSeqPtr>& dict, set<string>& pinyin):dict_(dict),pinyin_(pinyin){}
      protected: 
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        map<string,StringSeqPtr>& dict_; 
        set<string>& pinyin_;
    };

    class Dictionary : public MyUtil::Singleton<Dictionary> {
      public:
        void init(){
          using namespace com::xiaonei::xce;
          Statement sql;
          sql << "SELECT hanzi, pinyin FROM pinyin";
          PinYinResultHandler handler(dict_,pinyin_);
          QueryRunner(DB_FRIENDINPUT_PINYIN, CDbRServer).query(sql, handler);
        }
        StringSeqPtr getPinyin(const string& hz){
          map<string,StringSeqPtr>::iterator it = dict_.find(hz);
          if(it!=dict_.end()){
            return it->second;
          }else{
            return NULL;
          }
        }
        bool checkSyntax(const string& prefixPy){
          set<string>::iterator it = lower_bound(pinyin_.begin(),pinyin_.end(),prefixPy);
          if(it!=pinyin_.end() && StrUtil::startsWith(*it,prefixPy)){
            return true;
          }else{
            return false;
          }
        }
      private:
        map<string,StringSeqPtr> dict_;
        set<string> pinyin_;
        friend class MyUtil::Singleton<Dictionary>;
    };



    class SearchDataI : public IceUtil::Shared{
      public:
        SearchDataI():hzIndex_(false), pyIndex_(true){};
        void add(const Str2StrMapSeq& docs,const Str2IntMap& config, int timeout);
        virtual void load(int id) {return;}
        virtual void load() {return;}
        virtual SearchDataIPtr clone() {return NULL;}
        Str2StrMapSeq search(const string& query,int begin,int limit);
        virtual void splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,CollectorPtr collector);
        void getSubStr(const string& pinyin,unsigned int start,int num,vector<string> subStr, vector<string>& resultStr);
        ScorerPtr getScorer(const vector<string>& words, size_t limit){
          return pyIndex_.getScorer(words,limit);
        }
        bool timeout()  {
          return outTime_ < time(NULL) ;
        }

      private:
        PostingsList hzIndex_;
        PostingsList pyIndex_;

        Str2StrMapSeq stores_;

        time_t outTime_;
        void splitWord(const string&, vector<string>&);
        bool isPyIndex(int config){
          return config&(1 << 0);
        }
        bool isHzIndex(int config){
          return config&(1 << 1);
        }
        bool isStore(int config){
          return config&(1 << 2);
        }
    };

    class FriendSearchDataI : public SearchDataI {
      private:
        int maxSize;
        void getUserName(int uid, map<int, string>& friends);
      public:
        FriendSearchDataI() : maxSize(300){}
        void load(int);
        void load();
        SearchDataIPtr clone() {
          return new FriendSearchDataI;
        }
    };



    class PageSearchDataI : public SearchDataI {
      public:
        void load(int);
        void load();
        SearchDataIPtr clone() {
          return new PageSearchDataI;
        }
        void splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,CollectorPtr collector);
    };

    class PageDataResultHandler : virtual public com::xiaonei::xce::ResultHandler,
    public MyUtil::Singleton<PageDataResultHandler> {
      public:
        PageDataResultHandler(map<int,string>& pages):id2Page(pages) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        map<int,string>& id2Page;
    };

    class AppSearchDataI : public SearchDataI {
      public:
        void load(int);
        void load();
        SearchDataIPtr clone() {
          return new AppSearchDataI;
        }
    };

    class AppDataResultHandler : virtual public com::xiaonei::xce::ResultHandler,
    public MyUtil::Singleton<AppDataResultHandler> {
      public:
        AppDataResultHandler(map<int,string>& apps):id2App(apps) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        map<int,string>& id2App;
    };

    class UniversitySearchDataI : public SearchDataI {
      public:
        void load(int);
        void load();
        SearchDataIPtr clone() {
          return new UniversitySearchDataI;
        }
    };

    class UniversityDataResultHandler : virtual public com::xiaonei::xce::ResultHandler,
    public MyUtil::Singleton<UniversityDataResultHandler> {
      public:
        UniversityDataResultHandler(map<int,string>& universities):id2Universities(universities) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        map<int,string>& id2Universities;
    };

    class WorkplaceSearchDataI : public SearchDataI {
      public:
        void load(int);
        void load();
        SearchDataIPtr clone() {
          return new WorkplaceSearchDataI;
        }
        void splitPY(const string& pinyin,unsigned int start,int num,vector<string> subStr,CollectorPtr collector);
    };

    class WorkplaceDataResultHandler : virtual public com::xiaonei::xce::ResultHandler,
    public MyUtil::Singleton<WorkplaceDataResultHandler> {
      public:
        WorkplaceDataResultHandler(map<int, string>& workplace):id2Workplaces(workplace) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        map<int, string>& id2Workplaces;
    };

    class RecommendUserSearchDataI : public SearchDataI {
      public:
        void load(int);
        void load();
        SearchDataIPtr clone() {
          return new RecommendUserSearchDataI;
        }
    };

    class SearchDataIUtil {
      public:
        static SearchDataIPtr getSearchDataIPtr(string type) {
          SearchDataIPtr p = NULL;
          if (type == FRIEND) {
            p = new FriendSearchDataI;
          } else if (type == RECOMMEND_USER) {
            p = new RecommendUserSearchDataI;
          } else if (type == PAGE) {
            p = new PageSearchDataI;
          } else if (type == APP) {
            p = new AppSearchDataI;
          } else if (type == UNIVERSITY) {
            p = new UniversitySearchDataI;
          } else if (type == WORKPLACE) {
            p = new WorkplaceSearchDataI;
          }
          return p;
        }

        static bool isDynamicData(string type) {
          return (FRIEND == type || RECOMMEND_USER == type);
        }
    };
  }
}

#endif
