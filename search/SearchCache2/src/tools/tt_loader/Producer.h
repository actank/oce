#ifndef __PRODUCER_H_
#define __PRODUCER_H_

#include "common_header.h"
#include "MessageQueue.h"

class Producer: virtual public IceUtil::Thread {
 public:
   Producer(MessageQueue& message_queue, int mysql_select_count, int begin_id, int processed_valid_num);
   virtual void run();
 protected:
   MessageQueue& message_queue_;
   /// @brief mysql select 查询次数
   int mysql_select_count_;
   /// @brief 本次起始用户id
   int begin_id_; 
   /// @brief 已经处理过的有效用户id数(也即插入到TT中的用户数)
   int processed_valid_num_;
};



class BatchBasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
 public:
   BatchBasicStatusResultHandlerI(int& max_id, Id2SearchCacheDataPtrMapPtr& id_2_search_cache_data_ptr_map_ptr);
   virtual bool handle(mysqlpp::Row& row) const;
 private:
   int& _max_id;
   Id2SearchCacheDataPtrMapPtr& id_2_search_cache_data_ptr_map_ptr_;
};

#endif
