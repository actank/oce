/// 
/// @file FriendOfFriendIndexerI.cpp
/// @brief 二度好友搜索索引服务
/// @author zhigang.wu@opi-corp.com
/// @date 2010-06-11
/// 
#include "FriendOfFriendIndexerI.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "OceCxxAdapter/src/FriendOfFriendCacheAdapter.h"
#define DEBUG_TIME

#ifdef DEBUG_TIME
#include <sys/time.h>
#endif

//#include <glog/logging.h>


using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace com::renren::search2::friendoffriend;
//***************************************************************************

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.getAdapter()->add(&FriendOfFriendIndexerManagerI::instance(),
      service.createIdentity(kFOFIndexerObjectName, ""));
  TaskManager::instance().config(kTaskLevelForFetchName, ThreadPoolConfig(8, 16));
  FriendOfFriendIndexerManagerI::instance().bootstrap();

}

namespace com {
  namespace renren {
    namespace search2 {
      namespace friendoffriend {

        const string DB_SEARCH2_USERNAME = "user_names";
        const string kUserNameField = "id, name";
        const string kUserNameTable = "user_names";
        const int kMaxSqlQueryNum = 3000;

        /// @brief 一级消息队列的线程数
        const unsigned int kFirstLevelMessageQueueThreadNum = 10;
        /// @brief 二级消息队列的线程数
        const unsigned int kSecondLevelMessageQueueThreadNum = 10;
        /// @brief 一级线程中每个线程能存储的最大消息个数
        const unsigned int kFirstLevelThreadMaxMessageNum = 10000; 
        /// @brief 二级线程中每个线程能存储的最大消息个数
        const unsigned int kSecondLevelThreadMaxMessageNum = 10000; 

        FriendOfFriendIndexerManagerI::FriendOfFriendIndexerManagerI() : all_id_2_name_(200000000) {
          //google::InstallFailureSignalHandler();
          for (unsigned int i=0; i<kFirstLevelMessageQueueThreadNum; ++i) {
            first_level_message_queue_thread_vec_.push_back(new FirstLevelMessageQueueThread(i));
            (first_level_message_queue_thread_vec_.at(i))->start(128*1024).detach();
          }

          //for (unsigned int i=0; i<kSecondLevelMessageQueueThreadNum; ++i) {
          //  second_level_message_queue_thread_vec_.push_back(new SecondLevelMessageQueueThread(i));
          //  (second_level_message_queue_thread_vec_.at(i))->start(128*1024).detach();
          //}
        }

        /// @brief FriendOfFriendIndexerManagerI::bootstrap
        /// 与业务逻辑相关的初始化
        void FriendOfFriendIndexerManagerI::bootstrap() {
          ServiceI& service = ServiceI::instance();
          string dsn  = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendIndexer.MemCacheURL");
          MCE_INFO("FriendOfFriendIndexer.MemCacheURL=" << dsn);
          if (!cache_handler_.SetServers(dsn.c_str())) {
            LOGP("SetServers() failed.");
          } else {
            MCE_DEBUG("succeed to SetServers() for " << dsn);
          }

          bool read_from_file_flag = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendIndexer.ReadFromFileFlag");
          MCE_INFO("FriendOfFriendIndexer.ReadFromFileFlag=" << read_from_file_flag);
          string file = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendIndexer.Id2NameMapFile");
          MCE_INFO("FriendOfFriendIndexer.Id2NameMapFile=" << file);

          //加载并更新id到姓名的映射
          MCE_INFO("[TEMPLE] TaskManager::instance().execute(new LoadAndUpdateId2NameMapTask)...");
          TaskManager::instance().execute(new LoadAndUpdateId2NameMapTask(all_id_2_name_, read_from_file_flag, file));
        }


        AllId2Name::AllId2Name(int hash_size) : all_id_2_name_map_(hash_size), all_id_2_name_map_valid_(false) {
        }

        bool AllId2Name::Set(int id, string name) {
          IceUtil::RWRecMutex::WLock lock(all_id_2_name_map_rw_lock_);
          all_id_2_name_map_.insert(make_pair(id, name));
          return true;
        }

        int AllId2Name::Size() {
          IceUtil::RWRecMutex::RLock lock(all_id_2_name_map_rw_lock_);
          return all_id_2_name_map_.size();

        }

        string AllId2Name::Get(int id) {
          string result = "NULL";
          IceUtil::RWRecMutex::RLock lock(all_id_2_name_map_rw_lock_);
          hash_map<int, string>::iterator iter = all_id_2_name_map_.find(id);
          if (all_id_2_name_map_.end() != iter) {
            result = iter->second;
          }
          return  result;
        }

        bool AllId2Name::Update(int id, string new_name) {
          string old_name = Get(id);
          if (0 != old_name.compare(new_name)) {
            Set(id, new_name);
          }

          return true;
        }

        bool AllId2Name::IsValid() {
          IceUtil::RWRecMutex::RLock lock(all_id_2_name_map_rw_lock_);
          return all_id_2_name_map_valid_;
        }

        bool AllId2Name::SetValid() {
          IceUtil::RWRecMutex::WLock lock(all_id_2_name_map_rw_lock_);
          all_id_2_name_map_valid_ = true;
          return true;
        }

        bool AllId2Name::ReadFromFile(const string& file_name) {
          ifstream input_file(file_name.c_str());
          string line;
          int id;
          string name;
          int idx = 0;
          int pos = -1;

          while(getline(input_file, line, '\n')) {
            pos = line.find('\t');
            if(pos == -1) continue;
            id = atoi((line.substr(0, pos)).c_str());
            name = line.substr(pos+1,line.length() - pos - 1);

            Set(id, name);
            if (0 == ++idx % 1000000) {
              //TODO: 去掉临时调试信息
              MCE_INFO("[TEMPLE] ReadFromFile have read " << idx << " lines.");
            }
          }


          return true;
        }


        bool AllId2Name::Save2File(const string& file_name) {
          IceUtil::RWRecMutex::RLock lock(all_id_2_name_map_rw_lock_);
          ofstream output_file(file_name.c_str());
          int idx = 0;
          for ( hash_map<int, string>::iterator iter=all_id_2_name_map_.begin(); iter!=all_id_2_name_map_.end(); ++iter) {
            output_file << iter->first << "\t" << iter->second << endl;
            if (0 == ++idx % 1000000) {
              //TODO: 去掉临时调试信息
              MCE_INFO("[TEMPLE] Save2FIle have write " << idx << " lines.");
            }
          }

          MCE_INFO("Save2File " << file_name << " succeed!");
          return true;
        }

        LoadAndUpdateId2NameMapTask::LoadAndUpdateId2NameMapTask(AllId2Name& all_id_2_name, const bool read_from_file_flag, const string& file) : MyUtil::Task(kTaskLevelForFetchName), all_id_2_name_(all_id_2_name), read_from_file_flag_(read_from_file_flag), file_(file) {
          MCE_INFO("[TEMPLE] in LoadAndUpdateId2NameMapTask::LoadAndUpdateId2NameMapTask");
        }

        void LoadAndUpdateId2NameMapTask::handle() {
          MCE_INFO("[TEMPLE] Step in LoadAndUpdateId2NameMapTask::handle, read_from_file_flag_=" << read_from_file_flag_ << ", file=" << file_ );

          //1.初始的加载
          if (read_from_file_flag_) {
            //从文件加载
            all_id_2_name_.ReadFromFile(file_);
            all_id_2_name_.SetValid();
            MCE_INFO("[TEMPLE] ReadFromFile(" << file_ << " finished, Map size is " << all_id_2_name_.Size());
          } 

          while (true) {
            //1.从数据库中获取所有用户的id和名字，建立id->name的映射
            int begin_id = 0;
            int max_id = 0;
            int idx = 0;
            do {
              begin_id = max_id;
              Statement sql ;
              sql << "SELECT " << kUserNameField << " FROM " << kUserNameTable << " WHERE id>" << begin_id << " LIMIT " << kMaxSqlQueryNum;

              BatchResultHandlerI handler(all_id_2_name_, max_id);
              QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
              if (0 == ++idx % 100) {
                MCE_INFO("[TEMPLE]max_id=" << max_id);
              }
            } while (begin_id < max_id);

            all_id_2_name_.SetValid();
            MCE_INFO("[TEMPLE] GetNameFromDB " << " finished once, all_id_2_name_.Size()=" << all_id_2_name_.Size() << ", sleep 1 day...");
            all_id_2_name_.Save2File(file_);
            sleep(60*60*24);
          }
        }



        void FirstLevelMessageQueueThread::invoke(const Ice::Int user_id) {
          MCE_DEBUG("FirstLevelMessageQueueThread::invoke " << user_id);
          {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
            cache_.push(user_id);
            //线程中队列过长，丢掉
            if (cache_.size() > kFirstLevelThreadMaxMessageNum) {
              int temp_id = cache_.front();
              cache_.pop();
              MCE_INFO("RebuildIndex Thread, mod=" << mod_ << ", drop id " << temp_id);
            }
            mutex_.notify();
          }
        }

        void FirstLevelMessageQueueThread::run() {
          while(true) {
            Ice::Int user_id = -1;
            {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
              if (cache_.empty()) {
                mutex_.timedWait(IceUtil::Time::seconds(1));
              }
              if (!cache_.empty()) {
                user_id = cache_.front();
                cache_.pop();
              }
            }

            if (user_id > 0)  {
              //重建该用户的索引, 同步调用
              FriendOfFriendIndexerManagerI::instance().RebuildIndex(user_id);
              //删除与该用户相关的人的索引(不包括自己), 异步调用
              //(FriendOfFriendIndexerManagerI::instance().second_level_message_queue_thread_vec_.at(user_id % kSecondLevelMessageQueueThreadNum))->invoke(user_id);  
              MCE_INFO("BuildIndex Thread, mod=" << mod_ << ", cache_.size()=" << cache_.size());
            }
          }
        }

        void SecondLevelMessageQueueThread::invoke(const Ice::Int user_id) {
          MCE_DEBUG("SecondLevelMessageQueueThread::invoke uid=" << user_id);
          {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
            cache_.push(user_id);
            //线程中队列过长，丢掉
            if (cache_.size() > kSecondLevelThreadMaxMessageNum) {
              int temp_id = cache_.front();
              cache_.pop();
              MCE_INFO("EraseMap Thread, mod=" << mod_ << ", drop " << temp_id);
            }
            mutex_.notify();
          }
        }

        void SecondLevelMessageQueueThread::run() {
          while(true) {
            Ice::Int user_id = -1;
            {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
              if (cache_.empty()) {
                mutex_.timedWait(IceUtil::Time::seconds(1));
              }
              if (!cache_.empty()) {
                user_id = cache_.front();
                cache_.pop();
              }
            }

            if (user_id > 0)  {
              //删除与该用户相关的人的索引(不包括自己)
              FriendOfFriendIndexerManagerI::instance().RemoveAllRelatedIndex(user_id);
              MCE_INFO("EraseMap Thread, mod=" << mod_ << ", cache_.size()=" << cache_.size());
            }
          }
        }

        bool FriendOfFriendIndexerManagerI::BuildIndex(::Ice::Int user_id, ::Ice::Short level, const ::Ice::Current& ic) {
          const Ice::ConnectionPtr con = ic.con;
          if (con.get() != NULL) {
            MCE_INFO("FriendOfFriendIndexerManagerI::BuildIndex, uid=" << user_id  <<", level=" << level << ", invoker is :" << con->toString() );
          }
          if (2 == user_id % 10 || 3 == user_id % 10 || 4 == user_id % 10) {
            return BuildIndex(user_id);
          }

          return true;
        }

        bool FriendOfFriendIndexerManagerI::BuildIndex(::Ice::Int user_id) {
          //1. 从FOFCache服务中取二度好友id列表
          //2. 从DB中取相应ID的name
          //3. 调用库来建立索引(一个map)
          //4. 将建好的索引写入到MemCache中
          MCE_INFO("FriendOfFriendIndexerManagerI::BuildIndex " << user_id);
          if (0 > user_id) {
            return false;
          }

          try {
            FriendOfFriendMap map;
            //如果memcache已经有了，就不用重新建索引了
            if (cache_handler_.GetMap(user_id, map)) {
              MCE_INFO("memcache has store " << user_id << ", so do not need to build index, return.");
              return true;
            }

            //下列变量为统计程序性能用
            struct timeval begin;
            struct timeval fetch_indirect_id_time, fetch_name_time, set_map_time, before_generate_from_info_time, before_set_map_time;
            int fetch_indirect_id_cost, fetch_name_cost, generate_from_info_cost, set_map_cost, total_cost;
            gettimeofday(&begin, NULL);
            //1.从FOFCache服务中取二度好友id列表
            //MCE_INFO("[TEMPLE]FriendOfFriendIndexerManagerI::BuildIndex 取二度好友列表..." << user_id);
            FriendInfoSeq friend_info_seq = FriendOfFriendCacheAdapter::instance().get(user_id);

            gettimeofday(&fetch_indirect_id_time, NULL);
            fetch_indirect_id_cost = ((fetch_indirect_id_time.tv_sec*1000000 + fetch_indirect_id_time.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec) ) / 1000;

            if (!friend_info_seq.empty()) {
              vector<string> name_seq;
              name_seq.reserve(100000);

              if (!all_id_2_name_.IsValid()) {
                //MCE_INFO("[TEMPLE] all_id_2_name is invalid, so this time fetch name from db");
                AllId2Name local_id_2_name(100000);
                //hash_map<int, string> id_2_name;
                //2. 从DB中取相应ID的name
                FriendInfoSeq::iterator iter=friend_info_seq.begin();
                //MCE_INFO("[TEMPLE]FriendOfFriendIndexerManagerI::BuildIndex 从DB中取相应ID的name..." << user_id);
                while (iter!=friend_info_seq.end()) {
                  stringstream ss;
                  int real_size = 0;
                  ss << "(";
                  ss << iter->id;
                  real_size = 1;
                  ++iter;

                  while (iter!=friend_info_seq.end() && real_size<kMaxSqlQueryNum) {
                    ss << "," << iter->id;
                    ++real_size;
                    ++iter;
                  }
                  ss << ")";

                  //查询数据库DB
                  {
                    int max_id = 0; //此处max_id无用，纯粹是为形式上的统一

                    Statement sql;
                    sql << "SELECT " << " id, name " << " FROM user_names WHERE id IN "
                      << ss.str();
                    BatchResultHandlerI handler(local_id_2_name, max_id);
                    QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
                  }
                }
                for (size_t i=0; i<friend_info_seq.size(); ++i) {
                  int user_id = friend_info_seq[i].id;
                  name_seq.push_back(local_id_2_name.Get(user_id));
                }
              } else { //不从db取名字，从本地的内存map中取
                //MCE_INFO("[TEMPLE] all_id_2_name is valid, so this time fetch name from local map");
                for (size_t i=0; i<friend_info_seq.size(); ++i) {
                  int user_id = friend_info_seq[i].id;
                  name_seq.push_back(all_id_2_name_.Get(user_id));
                }
              }

              gettimeofday(&fetch_name_time, NULL);
              fetch_name_cost = ((fetch_name_time.tv_sec*1000000 + fetch_name_time.tv_usec) - (fetch_indirect_id_time.tv_sec*1000000 + fetch_indirect_id_time.tv_usec) ) / 1000;

              //MCE_INFO("[TEMPLE]FriendOfFriendIndexerManagerI::BuildIndex 调用库来建立索引..." << user_id << ", friend_info_seq.size=" << friend_info_seq.size() << ", name_seq.size=" << name_seq.size());
              //3. 调用库来建立索引(一个map)

              gettimeofday(&before_generate_from_info_time, NULL);

              map.GenerateFromInfo(friend_info_seq, name_seq);

              const char* data_ptr;
              size_t data_size;
              map.GetData(data_ptr, data_size);
              //4. 写到memcache中
              //MCE_INFO("[TEMPLE]FriendOfFriendIndexerManagerI::BuildIndex 写到memcache中..." << user_id);

              gettimeofday(&before_set_map_time, NULL);

              if (!cache_handler_.SetMap(user_id, map)) {
                MCE_WARN("SetMap() for " << user_id << " failed!");
              }

              gettimeofday(&set_map_time, NULL);

              generate_from_info_cost = ((before_set_map_time.tv_sec*1000000 + before_set_map_time.tv_usec) - (before_generate_from_info_time.tv_sec*1000000 + before_generate_from_info_time.tv_usec) ) / 1000;
              set_map_cost = ((set_map_time.tv_sec*1000000 + set_map_time.tv_usec) - (before_set_map_time.tv_sec*1000000 + before_set_map_time.tv_usec) ) / 1000;

              total_cost = ((set_map_time.tv_sec*1000000 + set_map_time.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec) ) / 1000;
              MCE_INFO("uid=" << user_id << ", 二度好友数=" << friend_info_seq.size() << ", 建map索引总耗时=" << total_cost << " ms: 取二度好友ID=" << fetch_indirect_id_cost << " ms, 取ID对应的姓名=" << fetch_name_cost << " ms, 产生map=" << generate_from_info_cost<< " ms, 建立map并写入远程的cache中=" << set_map_cost << " ms, 索引的size=" << data_size);

              return true;
            }
          } catch (IceUtil::Exception& e) {
            MCE_WARN("FriendOfFriendIndexerManagerI::BuildIndex "<<user_id << " occurs IceUtil::Exception! " << e.what()); 
          } catch (std::exception& e) {
            MCE_WARN("FriendOfFriendIndexerManagerI::BuildIndex "<< user_id << " occurs std::exception! " << e.what()); 
          } catch (...) {
            MCE_WARN("FriendOfFriendIndexerManagerI::BuildIndex " << user_id << " occurs unknown exception! "); 
          }
          return false;
        }

        bool FriendOfFriendIndexerManagerI::RebuildAllRelatedIndex(::Ice::Int user_id, ::Ice::Short level, const ::Ice::Current& ic) {
          const Ice::ConnectionPtr con = ic.con;
          if (con.get() != NULL) {
            MCE_INFO("FriendOfFriendIndexerManagerI::RebuildAllRelatedIndex, uid=" << user_id  <<", level=" << level << ", invoker is :" << con->toString() );
          }

          //FriendOfFriendCacheAdapter::instance().UpdateAllRelatedCache(user_id);
          if (user_id >= 0) { 
            //TODO:暂时只对尾号为2或3的用户提供Rebuild服务
            if (2 == user_id % 10 || 3 == user_id % 10 || 4 == user_id % 10) {
              (first_level_message_queue_thread_vec_.at(user_id % kFirstLevelMessageQueueThreadNum))->invoke(user_id);
            }
          }
          return true;
        }

        /// @brief FriendOfFriendIndexerManagerI::RebuildIndex 只重建该用户的索引
        /// 
        /// @param user_id 
        /// 
        /// @return 
        bool FriendOfFriendIndexerManagerI::RebuildIndex(int user_id) {
          MCE_INFO("FriendOfFriendIndexerManagerI::RebuildIndex: user_id=" << user_id);
          FriendOfFriendMap map;
          //相当于替换，只有memcache中原来就有，这时候才重建
          if (cache_handler_.GetMap(user_id, map)) {
            cache_handler_.EraseMap(user_id);
            BuildIndex(user_id);
          } else {
            MCE_INFO("RebuildIndex: GetMap Failed, so do not need to BuildIndex for " << user_id);
          }
          return true;
        }

        /// @brief FriendOfFriendIndexerManagerI::RemoveAllRelatedIndex 删除指定用户的所有相关用户的索引(不包括自己)
        /// 
        /// @param user_id 
        /// 
        /// @return 
        bool FriendOfFriendIndexerManagerI::RemoveAllRelatedIndex(int user_id) {

          FriendInfoSeq friend_info_seq = FriendOfFriendCacheAdapter::instance().get(user_id);
          if (!friend_info_seq.empty()) {
            //TODO(zhigang.wu@opi-corp.com):将FOF取二度好友服务拆分成带共同好友数的和不带共同好友数两个接口，后者可以节省时间
            for(FriendInfoSeq::iterator iter=friend_info_seq.begin(); iter!=friend_info_seq.end(); ++iter) {
              if (user_id != iter->id) {
                if (!cache_handler_.EraseMap(iter->id)) {
                  MCE_DEBUG("RebuildAllIndex Erase " << iter->id << " Failed");
                }
              }
            }
          }

          return true;
        }

        bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
          int user_id = (int)row["id"];
#ifndef NEWARCH                                                                         
          string name = row["name"].get_string();                            
#else                                                                                   
          string name = row["name"].c_str();                                 
#endif     

          //MCE_DEBUG("BatchResultHandlerI::handle id=" << user_id << ", name=" << name);
          id_2_name_.Update(user_id, name);

          max_id_ = user_id > max_id_ ? user_id : max_id_;

          return true;
        }



      }
    }
  }
}
