#ifndef _SEARCHINTERFACE_ICE
#define _SEARCHINTERFACE_ICE

#include <BasicClientCondition.ice>
#include <IndexResult.ice>
#include <ContentSearch.ice>

module com {
  module xiaonei {
    module search5 {
      module search {
        struct ProhibitedWord {
          int id;
          string word;
          string startTime;
          string endTime;
        };
        ["java:type:java.util.ArrayList"] sequence<ProhibitedWord> ProhibitedWordList;

        interface BasicSearcher {
          void registry();
          int setOpenDay(int openDay);
          com::xiaonei::search5::result::SearchResult search(com::xiaonei::search5::query::client::BasicClientCondition condition, int limit);
          MyUtil::StringArray getDelIdentityList(); 
          bool enablePush();
          void push(com::xiaonei::search5::logicserver::IndexUpdateDataSeq datas);
          bool lock();
          bool unlock();
          MyUtil::IntArray getMod();
          void reload();
          // 提供给安全的接口
          int addFilterTimes(MyUtil::StrList times);
          int addDangerUserIds(MyUtil::IntList ids);
          int addProhibitedWords(ProhibitedWordList words);
          bool isFiltering();
          bool safeFilterReload();
        };

        interface SubSearcher extends BasicSearcher {
          void flush();
        };

        interface SmartIndexManager {
          com::xiaonei::search5::result::SearchResult search(com::xiaonei::search5::query::client::BasicClientCondition condition, int offset, int limit);
          int setOpenDay(int openday);
        };
      };
    };
  };
};

#endif
