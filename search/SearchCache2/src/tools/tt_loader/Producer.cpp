#include "Producer.h"

BatchBasicStatusResultHandlerI::BatchBasicStatusResultHandlerI(int& max_id, Id2SearchCacheDataPtrMapPtr& id_2_search_cache_data_ptr_map_ptr) :
  _max_id(max_id), id_2_search_cache_data_ptr_map_ptr_(id_2_search_cache_data_ptr_map_ptr) {
  }

bool BatchBasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  _max_id = id > _max_id ? id : _max_id;
  int status = (int) row["status"];
  cout<<id<<endl;
  if (status<3) {
    SearchCacheDataPtr obj = new(nothrow) SearchCacheData;
    if(0 != obj)  {
      obj->_id =id;
      obj->setUserStatusProperties(row);
      id_2_search_cache_data_ptr_map_ptr_->insert(make_pair(id,obj));
    } 
  }

  return true;
}


Producer::Producer(MessageQueue& message_queue, int mysql_select_count, int begin_id, int processed_valid_num):message_queue_(message_queue), mysql_select_count_(mysql_select_count) , begin_id_(begin_id), processed_valid_num_(processed_valid_num) {}

void Producer::run() {

  int max_id = begin_id_;

  do {
    try{
      Id2SearchCacheDataPtrMapPtr id_2_search_cache_data_ptr_map_ptr(new Id2SearchCacheDataPtrMap);

      begin_id_ = max_id;
      //MCE_INFO("Producer: cycles=" << mysql_select_count_ << ", begin_id_=" << begin_id_ << ", processed_valid_num=" << processed_valid_num_);

      Statement status_sql;
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << begin_id_ << " LIMIT " << FILL_BATCH_SIZE;

      BatchBasicStatusResultHandlerI status_handler(max_id, id_2_search_cache_data_ptr_map_ptr);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);

      MessageUnit message(begin_id_, max_id, id_2_search_cache_data_ptr_map_ptr);

      message_queue_.Put(message);
      
      ++mysql_select_count_;
    } catch (IceUtil::Exception& e) {
      MCE_WARN("FillTask done with exception " <<  e.what());
    } catch (mysqlpp::Exception& e) {
      MCE_WARN("FillTask done with exception -> " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("FillTask done with exception " << e.what());
    }
  } while (begin_id_ < max_id);

  MCE_INFO("Producer Task done.");
}

