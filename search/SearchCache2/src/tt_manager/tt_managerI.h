/// 
/// @file tt_managerI.h
/// @brief Tokyo Tyrant 管理器，将多个TT封装为一个整体，对外透明，提供服务，
/// @author zhigang.wu@opi-corp.com
/// @date 2010-04-09
/// 
#ifndef _TT_PROXY_SERVERI_H
#define _TT_PROXY_SERVERI_H

#include "TTManager.h"
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <IceUtil/IceUtil.h>
#include "bit_vector.h"
#include "UtilCxx/src/Singleton.h"
#include "search/SearchCache2/src/Util/IDbCache.h"

//#define DEBUG_TIME

namespace xce{
  namespace searchcache{
    typedef int UserId;
    using std::string;
    /// @brief ice通讯进行定位时所用到的object name, 即 T@TTManager, 其中TTManger为adapter name，在ice配置文件中设置
    const string kTTManagerObjectName = "T";

    const int kTaskLevelGetUserInfo = 8;
    const int kTaskLevelInsertUserInfo = 9;


    /// @brief 分布式tokyo tyrant 服务器(多个)的集中管理器, 用于将多个TT封装成一个整体，对外透明, 提供以下服务
    ///        1.读写key-vlue数据，
    ///        2.遍历所有TT中的数据
    class TTManagerI: virtual public TTManager, 
    public MyUtil::Singleton<TTManagerI> {
     public:
       TTManagerI();
       ~TTManagerI();
       /// @brief Get 取key为user_id所对应的信息到user_info中, user_info为SearchCacheData序列化后的串
       /// @param user_id (in) 用户id
       /// @param user_info (out) 用户id对应的序列化信息
       /// @param Ice::Current& 
       /// 
       /// @return 若TT中存在该用户数据，返回true，其它情况返回false
       bool Get(Ice::Int user_id, ByteSeq& user_info, const Ice::Current&); 

       /// @brief Gets 读取多个用户id所对应的序列化信息，返回值保存在user_id_2_user_info_map中   
       /// @param user_ids (in) 一批用户id
       /// @param user_id_2_user_info_map (out) 用户id到用户序列化信息的一个map
       /// @param Ice::Current& 
       /// 
       /// @return 无异常返回true，否则返回false
       bool Gets(const IntSeq& user_ids, Int2ByteSeqMap& user_id_2_user_info_map, const Ice::Current&); 

       /// @brief Gets 为减小网络开销，特增加该接口，可一次从模为mod的TT中查询一批用户的信息
       /// 
       /// @param user_ids (in) 一批用户id，注意，所有的这些id模均为mod
       /// @param mod (in) 据此选择相应的TT, 取值范围 0 ~ cluster_
       /// @param user_id_2_user_info_map (out) 存储查询到的用户信息，用户id到用户序列化信息的一个map
       /// 
       /// @return 无异常返回true，否则返回false
       bool Gets(const IntSeq& user_ids, const int mod, Int2ByteSeqMap& user_id_2_user_info_map); 

       /// @brief Update 表示明确知道TT中已经存在该用户的信息，只是进行修改更新而已，因此不会更改相应的内存id列表，
       ///               当不知道TT中是否存在该用户的信息时，请使用下面的Insert接口
       /// 
       /// @param user_id (in) 用户id
       /// @param user_info (in) 用户的序列化信息
       /// @param Ice::Current& 
       /// 
       /// @return 无异常返回true，否则返回false
       bool Update(Ice::Int user_id, const ByteSeq& user_info, const Ice::Current&); 

       /// @brief Insert 向TT中插入指定用户的信息
       /// 
       /// @param user_id (in) 待插入的用户id
       /// @param user_info (in) 待插入的用户信息
       /// @param Ice::Current& 
       /// 
       /// @return 无异常返回true，否则返回false
       bool Insert(Ice::Int user_id, const ByteSeq& user_info, const Ice::Current&); 

       /// @brief Inserts 向TT中插入一批用户的信息
       /// 
       /// @param id_2_info_map (in) 待插入的用户信息
       /// @param Ice::Current& 
       /// 
       /// @return 无异常返回true，否则返回false
       bool Inserts(const Int2ByteSeqMap& id_2_info_map, const Ice::Current&);

       /// @brief Inserts 向特定TT中插入一批用户的信息
       /// 
       /// @param mod (in) 向模为mod值的TT中插入用户信息
       /// @param id_2_info_map (in) 一批用户的信息
       /// 
       /// @return 无异常返回true，否则返回false
       bool Inserts(int mod, Int2ByteSeqMap& id_2_info_map);

       /// @brief Remove 从TT中删除指定用户的信息
       /// 
       /// @param user_id (in) 用户id
       /// @param Ice::Current& 
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool Remove(Ice::Int user_id, const Ice::Current&); 

       /// @brief InsertMemIds 在内存id列表的位图中将user_ids所对应的一批用户id置位(设置为1)
       /// 
       /// @param user_ids (in) 要置位的一批用户id
       /// @param Ice::Current& 
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false, 目前的代码肯定返回true
       bool InsertMemIds(const IntSeq& user_ids, const Ice::Current&);

       /// @brief DoInsertMemIds 在内存id列表的位图中将user_ids所对应的一批用户id置位(设置为1)
       /// 
       /// @param user_ids (in) 要置位的一批用户id
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false, 目前的代码肯定返回true
       bool DoInsertMemIds(const IntSeq& user_ids);

       /// @brief RemoveMemIds 在内存id列表的位图中将user_ids所对应的一批用户id复位(设置为0), 表示TT中没有该用户id的信息
       /// 
       /// @param user_ids (in) 要复位的一批用户id
       /// @param Ice::Current& 
       /// 
       /// @return 目前的代码肯定返回true TODO:应该进一步考虑异常情况下的返回值
       bool RemoveMemIds(const IntSeq& user_ids, const Ice::Current&);

       /// @brief DoRemoveMemIds在内存id列表的位图中将user_ids所对应的一批用户id复位(设置为0), 表示TT中没有该用户id的信息
       /// 
       /// @param user_ids (in) 要复位的一批用户id
       /// 
       /// @return 目前的代码肯定返回true TODO:应该进一步考虑异常情况下的返回值
       bool DoRemoveMemIds(const IntSeq& user_ids);

       /// @brief GetMemIdState 查看TT中是否存在某用户的信息(不发生实际的TTQuery操作，而是去内存id列表中查看相应比特位的状态)
       /// 
       /// @param user_id (in) 用户id
       /// @param Ice::Current& 
       /// 
       /// @return 为true表示存在该用户信息(内存id列表相应比特位为1)，false表示不存在该用户信息(内存id列表相应比特位为0)
       bool GetMemIdState(const UserId user_id, const Ice::Current&);

       /// @brief GetMemIdsCount 得到内存id列表中有效的用户id数(即TT中真正存储的用户信息数)
       /// 
       /// @param Ice::Current& 
       /// 
       /// @return 内存id列表中的实际有效用户id数
       int GetMemIdsCount(const Ice::Current&);

       /// @brief GetMemIdsCount 得到内存id列表中可装载的最大用户数
       /// 
       /// @param Ice::Current& 
       /// 
       /// @return 内存id列表中可装载的最大用户id数
       int GetMemIdsListSize(const Ice::Current&);


       /// @brief Traversal 顺序遍历内存id列表， 每次从TT中读取最多200个用户信息
       /// 
       /// @param start_pos (in) 本次遍历所开始的位置(即用户id)
       /// @param travel_length (out) 本次遍历返回时共遍历的用户id数(即共扫描的位图位数)
       /// @param user_id_2_user_info_map (out) 本次遍历所真正得到的用户信息，为用户id到用户序列化信息的映射
       /// @param Ice::Current& 
       /// 
       /// @return 成功执行无异常发生返回true，否则返回false
       bool Traversal(Ice::Int start_pos, Ice::Int& travel_length, Int2ByteSeqMap& user_id_2_user_info_map, const Ice::Current&);

       /// @brief TraversalKey  顺序返回TT中存储的大于start_pos的一批用户id，用户id数目的多少由配置文件中"MaxReturnNumEachTraversal"设置。
       /// 
       /// @param start_pos (in) 本次读取的开始位置
       /// @param travel_length (out) 本次读取实际共遍历了多少个位置
       /// @param Ice::Current&ic (in)
       /// 
       /// @return 本次调用得到的有效用户id列表
       IntSeq TraversalKey(Ice::Int start_pos, Ice::Int request_num, const Ice::Current&ic);

       /// @brief get_bit_vector_ 返回内存id列表所对应的位图
       /// 
       /// @return 内存id列表所对应的位图
       inline CBitVector& get_bit_vector_() {
         return * pbit_vector_;
       }
       bool bootstrap();
       //TODO: 此处封装不好
       //根据user_id 获取所有的tt_server的写操作句柄
       vector<boost::shared_ptr<IDbCache> > GetTTWriterHandler(UserId user_id) ;

       //根据user_id随机获取一个tt_server的读操作句柄
       boost::shared_ptr<IDbCache> GetTTReaderHandler(UserId user_id);

       //根据user_id随机获取一个tt_server的遍历读操作句柄
       boost::shared_ptr<IDbCache> GetTTTraversalHandler(UserId user_id);
       inline int get_cluster_() {return cluster_; };
     private: 
       /// @brief ConfigWriterTT 根据配置文件中的信息初始化所有用于写操作的TT
       /// 
       /// @return 
       bool ConfigWriterTT();

       /// @brief ConfigReaderTT 根据配置文件中的信息初始化所有用于读操作的TT
       /// 
       /// @return 
       bool ConfigReaderTT();

       /// @brief ConfigTraversalTT 根据配置文件中的信息初始化所有用于遍历时所用到的TT
       /// 
       /// @return 
       bool ConfigTraversalTT();

       /// @brief 存储用于查询数据的TT的句柄
       std::map<int, std::vector<boost::shared_ptr<IDbCache> > > tt_reader_handler_map_;

       /// @brief 存储用于遍历的TT的句柄, 遍历时对TT性能要求较高，故专门指定，与平时的零星查询相区分
       std::map<int, std::vector<boost::shared_ptr<IDbCache> > > tt_traversal_handler_map_;

       /// @brief 存储用于写数据的TT的句柄
       std::map<int, std::vector<boost::shared_ptr<IDbCache> > > tt_writer_handler_map_;

       /// @brief 目前TT服务器是按模进行散列的，cluster_即为要取的模
       int cluster_;

       /// @brief 内存id列表所对应的位图
       CBitVector *pbit_vector_;

       /// @brief 内存id列表所对应的文件，在TTManager挂掉重启时能够迅速从该文件加载到内存id列表中
       std::string id_list_file_;

       //遍历时每次实际应取到的有效用户信息数, 设为2048的想法是： 在每次查询尽可能多的用户的前提下， 2048个用户id,
       //假设每个用户的信息平均长度为200B，则 200B*2048 = 400KB < 1MB(Ice通信默认的最大消息长度)
       //假设2048个用户ID平均分布，则模为0，1，2，3的实际TT查询为每次查询512个用户信息, 也是比较合理的
       int max_return_num_each_traversal_;

#ifdef DEBUG_TIME
       //上次tt查询发生时的绝对时刻
       static struct timeval last_query_time_;
       //总的tt查询次数
       static int total_tt_query_count_;
       //总的查询时间
       static int total_tt_query_time_;
       //总的逝去时间
       static int total_elapse_time_;
#endif 
       /// @brief 实现异步维护内存id列表，尽快返回远程调用
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
                //MCE_DEBUG("in InvokeInsertMemId, cache_.size() is " << cache_.size());
                if (cache_.size() < 200) {
                  mutex_.timedWait(IceUtil::Time::seconds(1));
                }
                batch.swap(cache_);
              }
              if (!batch.empty()) {
                MyUtil::TaskManager::instance().execute(new TTManagerInsertMemIdTask(batch));
              }
            }
          }
        private:
          IceUtil::Monitor<IceUtil::Mutex> mutex_;
          IntSeq cache_;

       };

       /// @brief 实现异步维护内存id列表，尽快返回远程调用
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
                //MCE_DEBUG("in InvokeRemoveMemId, cache_.size() is " << cache_.size());
                if (cache_.size() < 200) {
                  mutex_.timedWait(IceUtil::Time::seconds(1));
                }
                batch.swap(cache_);
              }
              if (!batch.empty()) {
                MyUtil::TaskManager::instance().execute(new TTManagerRemoveMemIdTask(batch));
              }
            }
          }
        private:
          IceUtil::Monitor<IceUtil::Mutex> mutex_;
          IntSeq cache_;
       };

       /// @brief 实现异步维护内存id列表，尽快返回远程调用
       InvokeInsertMemId invoke_insert_id_;
       /// @brief 实现异步维护内存id列表，尽快返回远程调用
       InvokeRemoveMemId invoke_remove_id_;

       class TTManagerInsertMemIdTask : public MyUtil::Task {
        public:
          TTManagerInsertMemIdTask(IntSeq& user_ids): Task(10),user_ids_(user_ids) {
          }
          //virtual ~TTManagerInsertMemIdTask() { }
          virtual void handle() {
            //MCE_DEBUG("[TEMPLE] in TTManagerInsertMemIdTask::handler(), user_ids_.size() is " << user_ids_.size());
            TTManagerI::instance().DoInsertMemIds(user_ids_);
          }
        private:
          IntSeq user_ids_;
       };

       class TTManagerRemoveMemIdTask : public MyUtil::Task {
        public:
          TTManagerRemoveMemIdTask(IntSeq& user_ids): Task(10),user_ids_(user_ids) {
          }
          virtual void handle() {
            //MCE_DEBUG("[TEMPLE] in TTManagerRemoveMemIdTask::handler(), user_ids_.size() is " << user_ids_.size());
            TTManagerI::instance().DoRemoveMemIds(user_ids_);
          }
        private:
          IntSeq user_ids_;
       };
    };

    class MonitorUpdateIdListFileTask : virtual public MyUtil::Task {
     public:
       /// @brief MonitorUpdateIdListFileTask
       /// 
       /// @param id_list_file 
       /// @param update_id_list_file_tag 
       MonitorUpdateIdListFileTask(string id_list_file, string update_id_list_file_tag);
       virtual void handle();
     private:
       /// @brief 
       string id_list_file_;
       /// @brief 
       string update_id_list_file_tag_;
    };

    /// @brief 
    class LoadIdListTask : public MyUtil::Task {
     public:
       LoadIdListTask(const bool use_id_list_file_flag, const std::string& id_list_file);
       void handle();
     private:
       /// @brief 
       bool use_id_list_file_flag_;
       /// @brief 
       string id_list_file_;
       class LoadThread : virtual public IceUtil::Thread {
        public:
          LoadThread(int mod, string id_list_file) :mod_(mod), id_list_file_(id_list_file) {}
          ~LoadThread() {};
        protected:
          void run() {
            int idx = 0;
            try {
              boost::shared_ptr<IDbCache> tt_handler = TTManagerI::instance().GetTTTraversalHandler(mod_);
              if (tt_handler) {
                IDbResult* pDbResult = tt_handler->Query();
                UserId user_id;
                while (BinStringPtr binObj = pDbResult->FetchObject(user_id)) {
                  TTManagerI::instance().get_bit_vector_().Set(user_id);
                  if (0 == ++idx % 1000) {
                    MCE_INFO("[TEMPLE]:" << idx << " user ids has been load for mod " << mod_);
                  }
                }
              }
            } catch (IceUtil::NullHandleException e) {
              MCE_FATAL("IceUtil " << e.what());
            } catch (std::exception e) {
              MCE_FATAL("std::exception " << e.what());
            }

            TTManagerI::instance().get_bit_vector_().Save(id_list_file_.c_str());
            MCE_INFO("LoadThread for mod " << mod_ << " done, total load "<< idx << " user id");
          }

        private:
          /// @brief 
          int mod_;
          string id_list_file_;

       };
    };

    /// @brief 用于对线程池的汇合
    class TaskReferenceCount {
     public:
       TaskReferenceCount(int cluster) : task_count_(cluster){
       }
       void Post() {
         try {
           MCE_DEBUG("[TEMPLE] before Post, task_count_ is " << task_count_);
           IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
           if (0 == --task_count_) {
             mutex_.notify();
           }
         } catch (IceUtil::Exception& e) {
           MCE_FATAL("TaskReferenceCount::Post() IceUtil::Exception " << e.what());
         } catch (std::exception& e) {
           MCE_FATAL("TaskReferenceCount::Post() std::exception " << e.what());
         } catch (...) {
           MCE_FATAL("TaskReferenceCount::Post() unknown exception ");
         }

       }
       void Wait() {
         MCE_DEBUG("[TEMPLE] in TaskReferenceCount::Wait(), task_count_ is " << task_count_);
         try {
           IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
           if (0 != task_count_) {
             //mutex_.timedWait(IceUtil::Time::seconds(1));
             mutex_.wait();
           }
         } catch (IceUtil::Exception& e) {
           MCE_FATAL("TaskReferenceCount::Wait() IceUtil::Exception " << e.what());
         } catch (std::exception& e) {
           MCE_FATAL("TaskReferenceCount::Wait() std::exception " << e.what());
         } catch (...) {
           MCE_FATAL("TaskReferenceCount::Wait() unknown exception ");
         }
       }
     private:
       int task_count_;
       IceUtil::Monitor<IceUtil::Mutex> mutex_;
    };


    class GetUserInfoTask: public MyUtil::Task {
     public:
       GetUserInfoTask(const int mod, const IntSeq& user_ids, Int2ByteSeqMap& user_id_2_user_info_map, TaskReferenceCount& task_reference_count, int level);
       void handle();
     private:
       const int mod_;
       const IntSeq& user_ids_;
       Int2ByteSeqMap& user_id_2_user_info_map_;
       TaskReferenceCount& task_reference_count_;
    };

    class InsertUserInfoTask: public MyUtil::Task {
     public:
       InsertUserInfoTask(const int mod, Int2ByteSeqMap& user_id_2_user_info_map, TaskReferenceCount& task_reference_count, int level);
       void handle();
     private:
       const int mod_;
       Int2ByteSeqMap& user_id_2_user_info_map_;
       TaskReferenceCount& task_reference_count_;
    };
  }
}

#endif 
