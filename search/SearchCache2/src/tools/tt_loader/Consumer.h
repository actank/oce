#ifndef __CONSUMER_H_
#define __CONSUMER_H_

#include "common_header.h"
#include "MessageQueue.h"

extern std::map<int, boost::shared_ptr<xce::searchcache::IDbCache> > db_cache_map_;

class Consumer: virtual public IceUtil::Thread {
  public:
    Consumer(MessageQueue& message_queue);
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
