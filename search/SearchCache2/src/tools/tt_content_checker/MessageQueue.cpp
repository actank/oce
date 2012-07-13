#include "MessageQueue.h"

MessageQueue::MessageQueue(size_t total_valid_num) : total_valid_num_(total_valid_num){
  // 初始化同步信号量
  int ini1 = sem_init(&consumer_sem_, 0, 0);
  int ini2 = sem_init(&producer_sem_, 0, QUEUE_MAX_LENGTH);  
  if(ini1 && ini2 != 0)
  {
    printf("sem init failed \n");
  }  

  //初始化互斥信号量
  int ini3 = pthread_mutex_init(&mutex_, NULL);
  if(ini3 != 0)
  {
    printf("mutex init failed \n");
  } 
}

void MessageQueue::Get(MessageUnit& data) {
  try  {
    sem_wait(&consumer_sem_);
    pthread_mutex_lock(&mutex_);
    data = data_.front();
    data_.pop();
    pthread_mutex_unlock(&mutex_);
    sem_post(&producer_sem_);
    MCE_INFO("to be processed: min_id=" << data.min_id_ << ", max_id=" << data.max_id_ << ", total_valid_num=" << total_valid_num_);
    total_valid_num_ += (data.id_2_search_cache_data_ptr_map_ptr_.get())->size();
  }  catch (...) {
    printf("MessageQueue Get Exception\n");
  }
}

void MessageQueue::Put(MessageUnit& data)  {
  try {
    sem_wait(&producer_sem_);
    pthread_mutex_lock(&mutex_);
    data_.push(data);
    pthread_mutex_unlock(&mutex_);
    sem_post(&consumer_sem_);
  } catch (...) {
    printf("MessageQueue::Put Exception!\n");
  }
}


