#ifndef __CONSUMER_H_
#define __CONSUMER_H_

#include "common_header.h"
#include "MessageQueue.h"
#include "OceCxxAdapter/src/SearchLogicAdapter.h"
#include "OceCxxAdapter/src/DistSearchLogicAdapter.h"
#include "OceCxxAdapter/src/DistSearchCacheAdapter.h"
#include "search/SearchCache2/src/DistSearchCacheLogic/DbOperation.h"

extern std::map<int, boost::shared_ptr<xce::searchcache::IDbCache> > db_cache_map_;

class Consumer: virtual public IceUtil::Thread {
  public:
    Consumer(MessageQueue& message_queue);
   
    string splitProfilePrivacy(std::string profile_privacy);

    inline int itoa(int i,char*string) {
      int power,j; j=i;
      int count = 0;
      for(power=1;j>=10;j/=10) power*=10;
      for(;power>0;power/=10) {
        *string++ ='0' + i/power;
        i%=power;
        count ++;
      }
      *string='\0';
      return count;
    }


    void transformData(xce::searchcache::SearchCacheDataPtr original, xce::searchcache::dboperation::SearchCacheMemDataPtr destination); 

    void transform(xce::searchcache::dboperation::SearchCacheMemDataPtr original, SearchCacheData& destination); 
    bool CompareSearchCacheData(SearchCacheData& tt_data, SearchCacheData& mysql_data, bool isTT);
    void ShowCompareSearchCacheData(SearchCacheData& tt_data, SearchCacheData& mysql_data);
    void ShowSearchCacheData(SearchCacheData& data);
    inline void UpdateDataInTT(int user_id) {
      UpdateUnitSeq updateUnits;
      {
        UpdateUnit updateUnit;
        updateUnit.id = user_id;
        updateUnit.table = "";
        updateUnits.push_back(updateUnit);
      }
      SearchLogicAdapter::instance().doUpdate(updateUnits);
      MCE_INFO("TT数据不一致 id=" << user_id);
    }

    inline void UpdateDataInSearchCache(int user_id) {
      UpdateUnitSeq updateUnits;
      {
        UpdateUnit updateUnit;
        updateUnit.id = user_id;
        updateUnit.table = "";
        updateUnits.push_back(updateUnit);
      }
      DistSearchLogicAdapter::instance().doUpdate(updateUnits);
      MCE_DEBUG("更新SearchCache id=" << user_id);
    }
    
    inline void UpdateUnsafeDataInSearchCache(int user_id, string status_value) {
      UpdateUnitSeq updateUnits;
      {
        UpdateUnit updateUnit;
        updateUnit.id = user_id;
        updateUnit.table = "user_passport";
        updateUnit.fieldPairMap["STATUS"] = status_value; 
        updateUnits.push_back(updateUnit);
      }
      DistSearchLogicAdapter::instance().doUpdate(updateUnits);
      MCE_DEBUG("Id=" << user_id<<" status>3,在SearchCache中去掉其信息!");
    }
    virtual void run();
  protected:
    MessageQueue&  message_queue_;
};

class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
 public:
   BatchResultHandlerI(std::string id,  void (xce::searchcache::SearchCacheData::*p)(mysqlpp::Row& row), Id2SearchCacheDataPtrMap& id_2_search_cache_data_ptr_map);
   virtual bool handle(mysqlpp::Row& row) const;
   std::string _id;
   void (xce::searchcache::SearchCacheData::*_pFunc)(mysqlpp::Row& row);
   Id2SearchCacheDataPtrMap& id_2_search_cache_data_ptr_map_;
};
//***************************************************************************
class BatchVectorResultHandlerI: public com::xiaonei::xce::ResultHandler {
 public:
   BatchVectorResultHandlerI(std::string idField, std::map<int, std::vector<mysqlpp::Row> >&);
   virtual bool handle(mysqlpp::Row& row) const;
   std::string _idField;
   std::map<int, std::vector<mysqlpp::Row> >& _id2RowsMap;
};

class DbRead :public MyUtil::Singleton<DbRead> {
 public:
   DbRead()  {  };
   boost::shared_ptr<xce::searchcache::IDbCache> getDbHandler(int userId) {
     map<int, boost::shared_ptr<xce::searchcache::IDbCache> >::iterator it = db_cache_map_.find(userId % 4/*_cluster*/);
     if (it == db_cache_map_.end())
       return boost::shared_ptr<xce::searchcache::IDbCache>((xce::searchcache::IDbCache*)NULL);
     return (*it).second;
   }
};


#endif
