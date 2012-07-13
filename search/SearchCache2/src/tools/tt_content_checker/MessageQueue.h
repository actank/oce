#ifndef __MESSAGE_QUEUE_H___
#define __MESSAGE_QUEUE_H___

#include "common_header.h"

#define QUEUE_MAX_LENGTH 100 //消息队列最大长度 

typedef std::map<int, xce::searchcache::SearchCacheDataPtr> Id2SearchCacheDataPtrMap;
typedef boost::shared_ptr<Id2SearchCacheDataPtrMap> Id2SearchCacheDataPtrMapPtr;

struct MessageUnit {
  MessageUnit() { }
  MessageUnit(int min_id, int max_id, Id2SearchCacheDataPtrMapPtr id_2_search_cache_data_ptr_map_ptr) :min_id_(min_id), max_id_(max_id), id_2_search_cache_data_ptr_map_ptr_(id_2_search_cache_data_ptr_map_ptr){ };
  MessageUnit(const MessageUnit& a) {
    min_id_ = a.min_id_;
    max_id_ = a.max_id_;
    id_2_search_cache_data_ptr_map_ptr_ = a.id_2_search_cache_data_ptr_map_ptr_;
  }
  MessageUnit& operator=(const MessageUnit& a) {
    min_id_ = a.min_id_;
    max_id_ = a.max_id_;
    id_2_search_cache_data_ptr_map_ptr_ = a.id_2_search_cache_data_ptr_map_ptr_;
    return *this;
  }

  int min_id_;
  int max_id_;
  Id2SearchCacheDataPtrMapPtr id_2_search_cache_data_ptr_map_ptr_;
};


typedef queue<MessageUnit> MessageUnitSeq;

class MessageQueue {
  public:
    MessageQueue(size_t total_valid_num);
    void Get(MessageUnit& data);
    void Put(MessageUnit& data);
  protected:
    /// @brief 消息队列缓冲 
    MessageUnitSeq data_;
    /// @brief  互斥信号量， 一次只有一个线程访问缓冲
    pthread_mutex_t mutex_;
    /// @brief  同步信号量， 当满了时阻止生产者放产品
    sem_t producer_sem_; 
    /// @brief  同步信号量， 当没产品时阻止消费者消费
    sem_t consumer_sem_;   
    /// @brief 已经处理过的实际处在的有效用户id数，用于显示进度及断点续传
    size_t total_valid_num_;
};
#endif
