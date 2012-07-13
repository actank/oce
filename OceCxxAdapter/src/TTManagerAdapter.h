#ifndef __TT_MANAGER_ADAPTER_H
#define __TT_MANAGER_ADAPTER_H

#include "TTManager.h"
#include <string>
#include <IceUtil/IceUtil.h>
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "AdapterI.h"

//#define USE_TEST_CHANNEL

namespace xce {
  namespace searchcache {

    typedef int UserId;
/// @brief TT管理器的数目
    const int kCluster = 1;
/// @brief 用户id到用户信息的映射
    typedef std::map<UserId, SearchCacheDataPtr> UserId2SearchCacheDataPtrMap;

    class TTManagerAdapter: public MyUtil::AdapterI, 
#ifdef USE_TEST_CHANNEL
    public MyUtil::AdapterISingleton<MyUtil::SearchChannel, TTManagerAdapter> {
#else
    public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, TTManagerAdapter> {
#endif
     public:
       TTManagerAdapter();

       /// @brief Query  取key为user_id所对应的用户信息到data_ptr所指向的内存中
       /// 
       /// @param (in) user_id  用户id
       /// @param (out) data_ptr data_ptr所指向的内存用于存储取到的用户信息
       /// 
       /// @return 正常取到指定用户的信息返回true，出现异常或不存在该用户的信息返回false
       bool Query(const UserId user_id, SearchCacheData *data_ptr);

       /// @brief Query 查询一批用户的信息
       /// 
       /// @param (in) ids 待查询的一批用户id
       /// @param (out) data_array_ptr 指向vector, vector里存的是智能指针，指针指向的是查询到的用户的信息
       /// 
       /// @return 无异常返回true，否则返回false
       bool Query(const vector<UserId>& ids, vector<SearchCacheDataPtr> *data_array_ptr);

       /// @brief Insert 将指定用户的信息存入TT中
       /// 
       /// @param user_id (in) 指定用户的id
       /// @param data (in) 要存入的完整用户信息
       /// 
       /// @return 无异常返回true，否则返回false
       bool Insert(const UserId user_id, SearchCacheData& data); 

       /// @brief Insert 将多个用户的信息存入到TT中, 注意由于tt的只提供了批量查询的接口而未提供批量插入的接口，故这里的批量插入，只是一个伪批量插入，仅仅是节省了客户端到TTManager逐个插入的网络开销，从TTManager到TT的插入还是逐个进行的。
       /// 
       /// @param data_arrry_ptr (in) 指向vector，vector里存的是智能指针，指针指向的是待存的用户信息
       /// 
       /// @return 无异常返回true， 否则返回false
       bool Insert(const vector<SearchCacheDataPtr>& data_arrry_ptr);

       /// @brief Update 表示明确知道TT中已经存在该用户的信息，只是进行修改更新而已
       /// 
       /// @param user_id (in) 要更新用户信息的用户id
       /// @param data (in) 要更新的完整用户信息
       /// 
       /// @return 无异常返回true，否则返回false
       bool Update(const UserId user_id, SearchCacheData& data); 

       /// @brief Remove 从TT中删除指定用户的所有信息
       /// 
       /// @param user_id (in) 要从TT中删除的用户
       /// 
       /// @return 无异常返回true，否则返回false
       bool Remove(const UserId user_id); 

       /// @brief Traversal 顺序遍历内存id列表，然后从TT中取出相应一批用户的信息，每次最多取200个用户的信息
       ///
       /// @param offset (in)  本次遍历所开始的位置(即用户id)
       /// @param travel_length (out) 本次遍历返回时共遍历的用户id数(即共扫描的位图位数)
       /// @param id_2_search_cache_data_ptr_map (out) 本次遍历所真正得到的用户信息，为用户id到用户序列化信息的映射
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool Traversal(const UserId& offset, int& travel_length, UserId2SearchCacheDataPtrMap& id_2_search_cache_data_ptr_map);

       /// @brief TraversalKey 从小到大遍历所有的有效用户(TT中有存储)的id
       /// 
       /// @param offset  本次遍历所开始的位置(即起始用户id)
       /// @param request_num 指定本次遍历请求返回多少有效用户id
       /// 
       /// @return 返回一批有效用户的id，注意：id已经按从小到大排好序, 若返回的有效用户id的数目不等于request_num ， 表示遍历已经结束，即已经到达最后
       IntSeq TraversalKey(const UserId& offset, const int request_num);

       /// @brief InsertMemId 在内存id列表的位图中将user_id 所对应的一个用户位图置为1, 异步方式实现，最终借助DoInsertMemIds实现远程调用
       /// 
       /// @param user_id 要在内存id列表置位的用户
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool InsertMemId(const UserId user_id); 

       /// @brief RemoveMemId 在内存id列表的位图中将user_id 所对应的一个用户位图置为0, 异步方式实现，最终借助DoRemoveMemIds实现远程调用
       /// 
       /// @param user_id 要在内存id列表复位的用户
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool RemoveMemId(const UserId user_id); 

       /// @brief GetMemIdState 查看TT中是否存在某用户的信息(不发生实际的TTQuery操作，而是去内存id列表中查看相应比特位的状态)
       /// 
       /// @param user_id 用户id
       /// 
       /// @return 为true表示存在该用户信息(内存id列表相应比特位为1)，false表示不存在该用户信息(内存id列表相应比特位为0)
       bool GetMemIdState(const UserId user_id);

       /// @brief GetMemIdsCount 远程获取TTManager(TT管理器)所维护的内存id列表实际装载的元素的数目
       /// 
       /// @return TTManager内存id列表实际装载的元素的数目
       int GetMemIdsCount();

       /// @brief GetMemIdsListSize 远程获取TTManager(TT管理器)所维护的内存id列表可装载的元素的最大数目
       /// 
       /// @return  TTManager内存id列表可装载的元素的最大数目
       int GetMemIdsListSize();


     private:
       /// @brief DoInsertMemIds  在内存id列表的位图中将user_ids所对应的一批用户id置位(设置为1)
       /// 
       /// @param user_ids 要置位的一批用户id
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool DoInsertMemIds(const IntSeq& user_ids); 

       /// @brief DoRemoveMemIds 在内存id列表的位图中将user_ids所对应的一批用户id复位(设置为0), 表示TT中没有该用户id的信息
       /// 
       /// @param user_ids 要置位的一批用户id
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool DoRemoveMemIds(const IntSeq& user_ids); 
       virtual std::string name() {
         return "TTManager";
       }
       virtual std::string endpoints() {
         return "@TTManager";
       }
       virtual size_t cluster() {
         return kCluster;
       }
       TTManagerPrx GetTTManager();
       vector<TTManagerPrx> managers_;

       class InvokeInsertMemId: virtual public IceUtil::Thread {
        public:
          void invoke(UserId user_id) {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
            cache_.push_back(user_id);
            if (cache_.size() > 200) {
              mutex_.notify();
            }   
          }   
          void invoke(IntSeq& user_ids) {
            if (user_ids.empty()) {
              return;
            }   
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
            cache_.insert(cache_.end(), user_ids.begin(), user_ids.end());
            if (cache_.size() > 200) {
              mutex_.notify();
            }   
          }   

        protected:
          void run() {
            while(1) {
              IntSeq batch;
              {   
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
                if (cache_.size() < 200) {
                  mutex_.timedWait(IceUtil::Time::seconds(1));
                }   
                batch.swap(cache_);
              }   
              MyUtil::TaskManager::instance().execute(new TTManagerAdapterInsertMemIdTask(batch));
            }
          }   
        private:
          IceUtil::Monitor<IceUtil::Mutex> mutex_;
          IntSeq cache_;
       };  

       class InvokeRemoveMemId: virtual public IceUtil::Thread {
        public:
          void invoke(UserId user_id) {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
            cache_.push_back(user_id);
            if (cache_.size() > 200) {
              mutex_.notify();
            }
          }
          void invoke(IntSeq& user_ids) {
            if (user_ids.empty()) {
              return;
            }
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
            cache_.insert(cache_.end(), user_ids.begin(), user_ids.end());
            if (cache_.size() > 200) {
              mutex_.notify();
            }
          }

        protected:
          void run() {
            while(1) {
              IntSeq batch;
              {
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
                if (cache_.size() < 200) {
                  mutex_.timedWait(IceUtil::Time::seconds(1));
                }
                batch.swap(cache_);
              }
              MyUtil::TaskManager::instance().execute(new TTManagerAdapterRemoveMemIdTask(batch));
            }
          }
        private:
          IceUtil::Monitor<IceUtil::Mutex> mutex_;
          IntSeq cache_;
       };

       InvokeInsertMemId invoke_insert_mem_id_;
       InvokeRemoveMemId invoke_remove_mem_id_;

       class TTManagerAdapterInsertMemIdTask : public MyUtil::Task {
        public:
          TTManagerAdapterInsertMemIdTask(IntSeq& user_ids): Task(10),user_ids_(user_ids) {
          }
          virtual ~TTManagerAdapterInsertMemIdTask() {}
          virtual void handle() {
            TTManagerAdapter::instance().DoInsertMemIds(user_ids_);
          }
        private:
          IntSeq user_ids_;
       };

       class TTManagerAdapterRemoveMemIdTask : public MyUtil::Task {
        public:
          TTManagerAdapterRemoveMemIdTask(IntSeq& user_ids): Task(10),user_ids_(user_ids) {
          }
          virtual ~TTManagerAdapterRemoveMemIdTask() {}
          virtual void handle() {
            TTManagerAdapter::instance().DoRemoveMemIds(user_ids_);
          }
        private:
          IntSeq user_ids_;
       };

    };

  }
}

#endif
