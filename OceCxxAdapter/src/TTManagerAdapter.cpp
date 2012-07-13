/// 
/// @file TTManagerAdapter.cpp
/// @brief 
/// @author zhigang.wu
/// @date 2010-03-30
/// 

#include "TTManagerAdapter.h"
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src/Util/BinString.h"

using namespace xce::searchcache;
using namespace MyUtil;
using std::string;
using std::cout;
using std::endl;

TTManagerAdapter::TTManagerAdapter(){
  invoke_insert_mem_id_.start(128 * 1024).detach();
  invoke_remove_mem_id_.start(128 * 1024).detach();
  managers_.resize(cluster() > 0 ? cluster() : 1);
}

TTManagerPrx TTManagerAdapter::GetTTManager() {
  //-1表示不限制超时时间，否则默认为260ms
  return locate<TTManagerPrx>(managers_, "T", 0, TWO_WAY, -1);
}

bool TTManagerAdapter::Query(const UserId key, SearchCacheData *data_ptr) {
  if (NULL == data_ptr || key <= 0) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      ByteSeq user_info;
      if (proxy->Get(key, user_info)) {
        if (data_ptr->Unserialize((const char*)user_info.data(), user_info.size())) {
          return true;
        }
      }
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Query Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Query std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Query Unknown Exception ");
  }
  return false;
}

bool TTManagerAdapter::Query(const vector<UserId>& ids, vector<SearchCacheDataPtr> *data_array_ptr) {
  if (NULL == data_array_ptr) {
    return false;
  }

  if (ids.empty()) {
    return true;
  }

  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      Int2ByteSeqMap user_id_2_user_info_map;

      //查询
      if (proxy->Gets(ids, user_id_2_user_info_map)) {

        //反序列化
        for (Int2ByteSeqMap::const_iterator iter = user_id_2_user_info_map.begin();
            iter!=user_id_2_user_info_map.end(); ++iter) {
          SearchCacheDataPtr data_ptr(new SearchCacheData);
          if (data_ptr->Unserialize((const char*)iter->second.data(), iter->second.size())) {
            data_array_ptr->push_back(data_ptr);
          }
        }
        
        return true;
      }
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Query Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Query std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Query Unknown Exception ");
  }

  return false;
}

bool TTManagerAdapter::Insert(const UserId key, SearchCacheData& data) {
  MCE_DEBUG("[TEMPLE] Step in TTManagerAdapter::Insert, uid is " << key);
  if (key <= 0) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      BinStringPtr bin_string_ptr = data.Serialize();
      char * pBuf = NULL;
      int length = 0;
      if (bin_string_ptr.get()) {
        if (0 < bin_string_ptr->getBuf(&pBuf, &length)) {
          ByteSeq user_info(pBuf, pBuf + length);
          return proxy->Insert(key, user_info);
        }
      }
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Insert Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Insert std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Insert Unknown Exception ");
  }

  return false;
}

bool TTManagerAdapter::Insert(const vector<SearchCacheDataPtr>& data_array_ptr) {
  if (data_array_ptr.empty()) {
    return true;
  }

  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      //先序列化
      Int2ByteSeqMap id_2_info_map;
      for (vector<SearchCacheDataPtr>::const_iterator iter=data_array_ptr.begin();
          iter!=data_array_ptr.end(); ++iter) {
        BinStringPtr bin_string_ptr = (*iter)->Serialize();
        char * pBuf = NULL;
        int length = 0;

        if (bin_string_ptr.get()) {
          if (0 < bin_string_ptr->getBuf(&pBuf, &length)) {
            ByteSeq user_info(pBuf, pBuf + length);
            id_2_info_map.insert(make_pair((*iter)->_id, user_info));
          }
        }
      }

      return proxy->Inserts(id_2_info_map);
    }

  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Insert Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Insert std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Insert Unknown Exception ");
  }

  return false;
}

bool TTManagerAdapter::Update(const UserId key, SearchCacheData& data) {
  if (key <= 0) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      BinStringPtr bin_string_ptr = data.Serialize();
      char * pBuf = NULL;
      int length = 0;
      if (bin_string_ptr.get()) {
        if (0 < bin_string_ptr->getBuf(&pBuf, &length)) {
          ByteSeq user_info(pBuf, pBuf + length);
          return proxy->Update(key, user_info);
        }
      }
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Update Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Update std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Update Unknown Exception ");
  }
  return false;
}

bool TTManagerAdapter::InsertMemId(UserId user_id) {
  if (user_id <= 0) {
    return false;
  }
  invoke_insert_mem_id_.invoke(user_id);
  return true;
}

bool TTManagerAdapter::DoInsertMemIds(const IntSeq& user_ids) {
  if (0 == user_ids.size()) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      return proxy->InsertMemIds(user_ids);
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::DoInsertMemIds Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::DoInsertMemIds std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::DoInsertMemIds Unknown Exception ");
  }
  return false;
}

bool TTManagerAdapter::GetMemIdState(UserId user_id) {
  if (user_id <= 0) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      return proxy->GetMemIdState(user_id);
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::GetMemIdState Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::GetMemIdState std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::GetMemIdState Unknown Exception ");
  }
  return false;
}

int TTManagerAdapter::GetMemIdsListSize() {
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      return proxy->GetMemIdsListSize();
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::GetMemIdsCount Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::GetMemIdsCount std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::GetMemIdsCount Unknown Exception ");
  }
  return -1;
}

int TTManagerAdapter::GetMemIdsCount() {
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      return proxy->GetMemIdsCount();
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::GetMemIdsCount Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::GetMemIdsCount std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::GetMemIdsCount Unknown Exception ");
  }
  return -1;
}

bool TTManagerAdapter::RemoveMemId(const UserId user_id) {
  if (user_id <= 0) {
    return false;
  }
  invoke_remove_mem_id_.invoke(user_id);
  return true;
}

bool TTManagerAdapter::DoRemoveMemIds(const IntSeq& user_ids) {
  if (0 == user_ids.size()) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      return proxy->RemoveMemIds(user_ids);
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::DoRemoveMemIds Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::DoRemoveMemIds std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::DoRemoveMemIds Unknown Exception ");
  }
  return false;
}

//TODO:此处的业务逻辑，对于TT的读写，可以异步写，同步读, 减少ice调用次数
bool TTManagerAdapter::Remove(const UserId key) {
  if (key <= 0) {
    return false;
  }
  try {
    TTManagerPrx proxy = GetTTManager();
    if (proxy) {
      return proxy->Remove(key);
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Remove Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Remove std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Remove Unknown Exception ");
  }
  return false;
}

xce::searchcache::IntSeq TTManagerAdapter::TraversalKey(const UserId& offset, const int request_num) {
  xce::searchcache::IntSeq valid_user_ids;
  if (offset < 0 || request_num <= 0) {
    return valid_user_ids;
  }

  try {
    TTManagerPrx proxy = GetTTManager();

    if (proxy) {
      return proxy->TraversalKey(offset, request_num);
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::TraversalKey Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::TraversalKey std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::TraversalKey Unknown Exception ");
  }

  return valid_user_ids;
}

bool TTManagerAdapter::Traversal(const UserId& offset, int& travel_length, UserId2SearchCacheDataPtrMap& id_2_search_cache_data_ptr_map) {
  if (offset < 0) {
    return false;
  }
  MCE_DEBUG("[TEMPLE] Step in TTManagerAdapter::Traversal");
  cout <<"[TEMPLE] Step in TTManagerAdapter::Traversal" << endl;
  try {
    TTManagerPrx proxy = GetTTManager();
    Int2ByteSeqMap user_id_2_user_info_map;
    if (proxy) {
      bool return_value = proxy->Traversal(offset, travel_length, user_id_2_user_info_map);
      if (return_value) {
        for (Int2ByteSeqMap::const_iterator iter=user_id_2_user_info_map.begin(); 
            iter!=user_id_2_user_info_map.end(); ++iter) {
          SearchCacheDataPtr ptr(new SearchCacheData);
          //防错性设计
          ptr->Unserialize((const char*)(iter->second).data(), (iter->second).size());
          id_2_search_cache_data_ptr_map.insert(std::make_pair(iter->first, ptr));
        }
      } 
      MCE_DEBUG("[TEMPLE] Step out TTManagerAdapter::Traversal");
      cout <<"[TEMPLE] Step out TTManagerAdapter::Traversal" << endl;
      return return_value;
    }
  } catch (Ice::Exception &e) {
    MCE_FATAL("TTManagerAdapter::Traversal Ice::Exception " <<e);
  } catch (std::exception& e) {
    MCE_FATAL("TTManagerAdapter::Traversal std::exception " <<e.what());
  } catch (...) {
    MCE_FATAL("TTManagerAdapter::Traversal Unknown Exception ");
  }

  MCE_DEBUG("[TEMPLE] Step out TTManagerAdapter::Traversal");
  cout <<"[TEMPLE] Step out TTManagerAdapter::Traversal" << endl;
  return false;
}
