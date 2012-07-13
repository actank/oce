/// 
/// @file tt_managerI.cc
/// @brief 
/// @author zhigang.wu
/// @date 2010-03-18
/// 
#include "tt_managerI.h"
#include <unistd.h> // access函数所需
#include <stdio.h> // remove函数所需
#include <sstream>
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/LogWrapper.h"
#include "bit_vector.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
//TODO:当具体的TT重启时，相应的handler作废，应更新。

//#define DEBUG
//#define TEST
//#define DEBUG_TIME

namespace MyUtil {
  void initialize() {
    ServiceI& service = ServiceI::instance();

    service.getAdapter()->add(&xce::searchcache::TTManagerI::instance(),service.createIdentity(xce::searchcache::kTTManagerObjectName, ""));

    TaskManager::instance().config(ThreadPoolConfig(32, 128));
    TaskManager::instance().config(xce::searchcache::kTaskLevelGetUserInfo, ThreadPoolConfig(32, 128));
    TaskManager::instance().config(xce::searchcache::kTaskLevelInsertUserInfo, ThreadPoolConfig(32, 128));
    //    TaskManager::instance().config(ThreadPoolConfig(32, 128));

    if(!xce::searchcache::TTManagerI::instance().bootstrap()) {
      MCE_FATAL("xce::searchcache::TTManagerI::instance().bootstrap() ERROR!");
    }
  }
}

//using namespace MyUtil;
namespace xce {
  namespace searchcache {
    using namespace MyUtil;

    /// @brief 任务执行级别，这里的5没有特别的含义，仅仅是一个大于0的整数
    const int kTaskLevelMonitorUpdateIdListFile = 5;

    //const int kMaxReturnNumEachTravel = 1048;
#ifdef DEBUG_TIME
    int TTManagerI::total_tt_query_time_ = 0;
    int TTManagerI::total_elapse_time_= 0;
    int TTManagerI::total_tt_query_count_ = 0;
    struct timeval TTManagerI::last_query_time_;
#endif 

    TTManagerI::TTManagerI() {
      pbit_vector_ = new CBitVector(~(1<<31)); //即最大32位正整数 2147483647
      invoke_insert_id_.start(128*1024).detach();
      invoke_remove_id_.start(128*1024).detach();
    }

    TTManagerI::~TTManagerI() {
      if (pbit_vector_) {
        delete pbit_vector_; 
        pbit_vector_ = NULL;
      }
    }
#ifdef TEST
    class TestTraversalTask: public MyUtil::Task {
     public:
       TestTraversalTask() : MyUtil::Task(kTaskLevelMonitorUpdateIdListFile){ }
       void handle() {
         MCE_DEBUG("********************");
         MCE_DEBUG("Being Traversal...");
         int start_pos = 0;
         int travel_length = 0;
         int total_num = TTManagerI::instance().get_bit_vector_().Size();
         int idx = 0;
         while (start_pos < total_num) {
           Int2ByteSeqMap user_id_2_user_info_map;
           Ice::Current *ic = new Ice::Current();
           TTManagerI::instance().Traversal(start_pos, travel_length, user_id_2_user_info_map, *ic);
           MCE_DEBUG( "idx=" << idx << ", start_pos=" << start_pos << ", travel_length=" << travel_length << ", result.size() is " << user_id_2_user_info_map.size());
           start_pos += travel_length;
           if (0 == ++idx % 100) {
             MCE_DEBUG("Travelsal idx=" << idx);
           }
         }
       }
    };
#endif

    /// @brief TTManager::bootstrap 
    /// 侧重于根据业务逻辑进行相关初始化
    /// @return 
    bool TTManagerI::bootstrap() {

      ServiceI& service = ServiceI::instance();
      cluster_ = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("TTManager.Cluster", 4);
      MCE_INFO("Reading Config: TTManager.Cluster=" << cluster_);
      max_return_num_each_traversal_ = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("TTManager.MaxReturnNumEachTraversal", 512);
      MCE_INFO("Reading Config: TTManager.MaxReturnNumEachTraversal=" << max_return_num_each_traversal_);

      if (!ConfigReaderTT()) {
        MCE_FATAL("ERROR in ConfigReaderTT");
      }
      if (!ConfigWriterTT()) {
        MCE_FATAL("ERROR in ConfigWriterTT");
      }
      if (!ConfigTraversalTT()) {
        MCE_FATAL("ERROR in ConfigTraversalTT");
      }

      id_list_file_ = service.getCommunicator()->getProperties()->getProperty("TTManager.IdListFile");
      MCE_INFO("Reading Config: TTManager.IdListFile is " << id_list_file_);
      bool use_id_list_file_flag = service.getCommunicator()->getProperties()->getPropertyAsInt("TTManager.UseIdListFileFlag");
      MCE_INFO("ReadingConfig: TTManager.UseIdListFileFlag is " << use_id_list_file_flag);
      //加载id列表到内存
      //TODO:考虑边界条件：当加载内存id列表尚未完成时，进行遍历即索引重建，则会缺失很多数据!!!
      TaskManager::instance().execute(new LoadIdListTask(use_id_list_file_flag, id_list_file_));

      string update_id_list_file_tag = service.getCommunicator()->getProperties()->getPropertyWithDefault("TTManager.UpdateIdListFileTag", "Config Error!");
      MCE_INFO("Reading Config: TTManager.UpdateIdListFileTag is " << update_id_list_file_tag);
      //监控是否更新id列表文件
      TaskManager::instance().execute(new MonitorUpdateIdListFileTask(id_list_file_, update_id_list_file_tag));
#ifdef TEST
      sleep(10);
      TaskManager::instance().execute(new TestTraversalTask());

#endif
      //////////////////////////////////////////////////
      //////////////////////////////////////////////////
      return true;
    }

    bool TTManagerI::ConfigReaderTT() {
      ServiceI& service = ServiceI::instance();
      for (int i=0; i<cluster_; ++i) {
        stringstream ss;
        ss << "TTManager.Reader.Mod"<<i<<".Num";
        int num = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(ss.str(), 0);
        MCE_INFO("TTPorxy: TTManager.Reader.Mod.Num=" << num);

        for (int j=0; j<num; ++j) {
          ss.str("");
          ss << "TTManager.Reader.Mod"<<i<<"."<<j;
          string tt_name = service.getCommunicator()->getProperties()->getPropertyWithDefault(ss.str(), "Config Error!");
          MCE_INFO("TTPorxy: TTManager.Reader.Mod" << i << "." << j <<" is " << tt_name);
          boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
          //TODO(zhigang.wu@opi-corp.com):对应应有Close
          if (db_cache->Initialize() == false) {
            MCE_WARN("Mod" << i << "." << j << " initialize failed");
            continue;
          }
          tt_reader_handler_map_[i].push_back(db_cache);
        }
      }

      return true;
    }

    bool TTManagerI::ConfigTraversalTT() {
      ServiceI& service = ServiceI::instance();
      for (int i=0; i<cluster_; ++i) {
        stringstream ss;
        ss << "TTManager.Traversal.Mod"<<i<<".Num";
        int num = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(ss.str(), 0);
        MCE_INFO("TTPorxy: TTManager.Traversal.Mod.Num=" << num);

        for (int j=0; j<num; ++j) {
          ss.str("");
          ss << "TTManager.Traversal.Mod"<<i<<"."<<j;
          string tt_name = service.getCommunicator()->getProperties()->getPropertyWithDefault(ss.str(), "Config Error!");
          MCE_INFO("TTPorxy: TTManager.Traversal.Mod" << i << "." << j <<" is " << tt_name);
          boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
          //TODO(zhigang.wu@opi-corp.com):对应应有Close
          if (db_cache->Initialize() == false) {
            MCE_WARN("Mod" << i << "." << j << " initialize failed");
            continue;
          }
          tt_traversal_handler_map_[i].push_back(db_cache);
        }
      }

      return true;
    }


    bool TTManagerI::ConfigWriterTT() {
      ServiceI& service = ServiceI::instance();
      for (int i=0; i<cluster_; ++i) {
        stringstream ss;
        ss << "TTManager.Writer.Mod"<<i<<".Num";
        int num = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(ss.str(), 0);
        MCE_INFO("TTPorxy: TTManager.Writer.Mod.Num=" << num);

        for (int j=0; j<num; ++j) {
          ss.str("");
          ss << "TTManager.Writer.Mod"<<i<<"."<<j;
          string tt_name = service.getCommunicator()->getProperties()->getPropertyWithDefault(ss.str(), "Config Error!");
          MCE_INFO("TTPorxy: TTManager.Writer.Mod" << i << "." << j <<" is " << tt_name);
          boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
          if (db_cache->Initialize() == false) {
            MCE_WARN("Mod" << i << "." << j << " initialize failed");
            continue;
          }
          tt_writer_handler_map_[i].push_back(db_cache);
        }
      }

      return true;
    }

    boost::shared_ptr<IDbCache> TTManagerI::GetTTReaderHandler(UserId user_id) {
      if (user_id < 0) {
        return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
      }
#ifdef DEBUG_TIME
      struct timeval begin;
      struct timeval end;
      int cost;
      gettimeofday(&begin, NULL);
#endif
      map<int , vector<boost::shared_ptr<IDbCache> > >::iterator tt_reader_handler_iterator = tt_reader_handler_map_.find(user_id % cluster_);
      if (tt_reader_handler_map_.end() == tt_reader_handler_iterator) {
        return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
      }

      vector< boost::shared_ptr<IDbCache> > tt_reader_handler_vector = tt_reader_handler_iterator->second;
      int size = tt_reader_handler_vector.size();
      if (0 == size) {
        return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
      } else {
        //用随机数实现负载均衡
        srand((unsigned int)time(NULL));
#ifdef DEBUG_TIME
        gettimeofday(&end, NULL);
        cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);
        MCE_INFO("GetTTReaderHandler(" << user_id << ") cost=" << cost) ;
#endif
        int tt_idx = rand() % size;
        MCE_INFO("GetTTReaderHandler select TT " << tt_idx);
        return tt_reader_handler_vector[rand() % size];
      }
    }

    boost::shared_ptr<IDbCache> TTManagerI::GetTTTraversalHandler(UserId user_id) {
      if (user_id < 0) {
        return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
      }
#ifdef DEBUG_TIME
      struct timeval begin;
      struct timeval end;
      int cost;
      gettimeofday(&begin, NULL);
#endif
      map<int , vector<boost::shared_ptr<IDbCache> > >::iterator tt_traversal_handler_iterator = tt_traversal_handler_map_.find(user_id % cluster_);
      if (tt_traversal_handler_map_.end() == tt_traversal_handler_iterator) {
        return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
      }

      vector< boost::shared_ptr<IDbCache> > tt_traversal_handler_vector = tt_traversal_handler_iterator->second;
      int size = tt_traversal_handler_vector.size();
      if (0 == size) {
        return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
      } else {
        //用随机数实现实现负载均衡
        srand((unsigned int)time(NULL));
#ifdef DEBUG_TIME
        gettimeofday(&end, NULL);
        cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);
        MCE_INFO("GetTTTraversalHandler(" << user_id << ") cost=" << cost) ;
#endif
        int tt_idx = rand() % size;
        MCE_INFO("GetTTTraversalHandler select TT " << tt_idx);
        return tt_traversal_handler_vector[rand() % size];
      }
    }


    vector<boost::shared_ptr<IDbCache> > TTManagerI::GetTTWriterHandler(UserId user_id) {
      vector<boost::shared_ptr<IDbCache> > temp_vector;
      if (user_id < 0) {
        return temp_vector;
      }
      map<int, vector<boost::shared_ptr<IDbCache> > >::iterator tt_writer_handler_iterator = tt_writer_handler_map_.find(user_id % cluster_);

      if (tt_writer_handler_map_.end() != tt_writer_handler_iterator) {
        return tt_writer_handler_iterator->second;
      }

      return temp_vector;
    }

    bool TTManagerI::Get(Ice::Int user_id, ByteSeq& user_info, const Ice::Current&) {
      if (user_id < 0) {
        return false;
      }
      boost::shared_ptr<IDbCache> tt_handler = GetTTReaderHandler(user_id);
      if (NULL == tt_handler) {
        return false;
      }

      if (!tt_handler->Query(user_id, user_info)) { 
        MCE_INFO("tt_handler->Query(" << user_id << ") Failed");
        return false;
      }

      return true;
    }

    bool TTManagerI::Gets(const IntSeq& user_ids, const int mod , Int2ByteSeqMap& user_id_2_user_info_map) {
      if (mod < 0 || mod >= cluster_) {
        return false;
      }

      if (user_ids.empty()) {
        return true;
      }

      try {
        boost::shared_ptr<IDbCache> tt_handler = GetTTTraversalHandler(mod);
        if (NULL == tt_handler) {
          return false;
        }

#ifdef DEBUG_TIME
        struct timeval begin;
        struct timeval end;
        int cost;
        gettimeofday(&begin, NULL);

        if (0 == mod%cluster_) {
          if (0 == total_tt_query_count_) {
            gettimeofday(&last_query_time_, NULL);
            ++total_tt_query_count_;
          } else {
            //总的逝去时间
            total_elapse_time_ += ( (begin.tv_sec*1000000 + begin.tv_usec) - (last_query_time_.tv_sec*1000000 + last_query_time_.tv_usec) );
            //模为0的tt总查询次数
            ++total_tt_query_count_;
            float mean_query_time_per_sec = total_tt_query_count_ * 1000000.0 / (total_elapse_time_);
            MCE_INFO("mod=" << mod << ", total_query_count_="<<total_tt_query_count_<<", total_elapse_time_="<<total_elapse_time_/1000000/60 <<" min, mean_query_count_per_second="<<mean_query_time_per_sec);
            gettimeofday(&last_query_time_, NULL);
          }
        }
#endif


        if (!tt_handler->Query(user_ids, user_id_2_user_info_map)) { 
          MCE_INFO("tt_handler->Query(user_ids) Failed");
          return false;
        }
        



#ifdef DEBUG_TIME
        gettimeofday(&end, NULL);
        cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);
        if (0 == mod % cluster_) {
          total_tt_query_time_ += cost;
          MCE_INFO("TTManagerI::Gets " << "Mod="<<mod<< ", user_ids.size=" << user_ids.size() << ", current cost=" << cost << ", total_tt_query_time=" << total_tt_query_time_ / 1000000 / 60 << " min, total_tt_query_count_=" << total_tt_query_count_<<", mean_query_time=" << total_tt_query_time_ / total_tt_query_count_ << " us");
        } else {
          MCE_INFO("TTManagerI::Gets " << "Mod="<<mod<< ", user_ids.size=" << user_ids.size() << ", Real TT Query cost=" << cost);
        }
#endif
      } catch (std::exception& e) {
        MCE_FATAL("TTManagerI::Gets Mod FATAL! " << e.what());
        return false;
      } catch (...) {
        MCE_FATAL("TTManagerI::Gets Mod FATAL! Unknown exception");
        return false;
      }

      return true;
    }

    bool TTManagerI::Gets(const IntSeq& user_ids, Int2ByteSeqMap& user_id_2_user_info_map, const Ice::Current&) {
      if (0 == user_ids.size()) {
        return false;
      }

      try {
        map<int, IntSeq> mod_2_user_ids;
        //先取模分类
        for (IntSeq::const_iterator iter = user_ids.begin(); iter != user_ids.end(); ++iter) {
          mod_2_user_ids[*iter % cluster_].push_back(*iter);
        }

        map<int, Int2ByteSeqMap> mod_2_user_id_2_info_map;
        for (int i=0; i<cluster_; ++i) {
          Int2ByteSeqMap temp_user_id_2_user_info_map;
          mod_2_user_id_2_info_map.insert(std::make_pair(i, temp_user_id_2_user_info_map));
        }

        TaskReferenceCount task_reference_count(cluster_);
        //多线程并行查询
        for (int i=0; i<cluster_; ++i) {
          TaskManager::instance().execute(new GetUserInfoTask(i, mod_2_user_ids[i], mod_2_user_id_2_info_map[i], task_reference_count, kTaskLevelGetUserInfo));
#ifdef DEBUG
          //MCE_DEBUG("before TaskTaskManager::instance().execute(new GetUserInfoTask, user_ids.size() is " << iter->second.size());
#endif 
          //TaskManager::instance().execute(new GetUserInfoTask(iter->first, iter->second, mod_2_user_id_2_info_map[iter->first], task_reference_count, kTaskLevelGetUserInfo));
#ifdef DEBUG
          MCE_DEBUG("after TaskTaskManager::instance().execute(new GetUserInfoTask, TaskManager::instance().size(kTaskLevelGetUserInfo) is " << TaskManager::instance().size(kTaskLevelGetUserInfo));
#endif 
        }

#ifdef DEBUG
        MCE_DEBUG("[DEBUG] before task_reference_count.Wait() !!!");
#endif 
        task_reference_count.Wait();
#ifdef DEBUG
        MCE_DEBUG("[DEBUG] task_reference_count.Wait() Return!!!");
#endif 

        MCE_DEBUG("[TEMPLE] Begin Merge ...");
        for (int i=0; i<cluster_; ++i) {
          MCE_DEBUG("mod_2_user_id_2_info_map[" << i << "].size() is " << mod_2_user_id_2_info_map[i].size())
            user_id_2_user_info_map.insert(mod_2_user_id_2_info_map[i].begin(), mod_2_user_id_2_info_map[i].end());
        }
      } catch (std::exception& e) {
        MCE_FATAL("TTManagerI::Gets FATAL! " << e.what());
        return false;
      } catch (...) {
        MCE_FATAL("TTManagerI::Gets FATAL! Unknown exception");
        return false;
      }

      return true;
    }

    /// @brief TTManagerI::Update
    /// Update与Insert的区别是：调用Update时表示明确知道TT里本来就存在这条记录，只是修改部分字段而已，
    //  因此也就不用再往内存id列表里插入, 若不知道TT里是否存在该记录，可以直接调用Insert，不过这样会增加维护内存id列表的成本
    /// @param user_id 
    /// @param user_info 
    /// @param Ice::Current& 
    /// 
    /// @return 
    bool TTManagerI::Update(Ice::Int user_id, const ByteSeq& user_info, const Ice::Current&) {
      if (user_id < 0) {
        return false;
      }

      vector<boost::shared_ptr<IDbCache> > writer_handler_vector = GetTTWriterHandler(user_id);
      for(vector<boost::shared_ptr<IDbCache> >::iterator iter = writer_handler_vector.begin(); iter != writer_handler_vector.end(); ++iter) {
        if(!(*iter)->Insert(user_id, user_info)) {
          MCE_WARN("tt_handler->Insert(" << user_id <<") Failed");
          return false;
        }
      }
      return true;
    }

    bool TTManagerI::Insert(Ice::Int user_id, const ByteSeq& user_info, const Ice::Current&) {
      if (user_id < 0) {
        return false;
      }
      MCE_DEBUG("[TEMPLE] Step in TTManagerI::Insert, userid is " << user_id);
      vector<boost::shared_ptr<IDbCache> > writer_handler_vector = GetTTWriterHandler(user_id);

      for(vector<boost::shared_ptr<IDbCache> >::iterator iter = writer_handler_vector.begin(); iter != writer_handler_vector.end(); ++iter) {
        if(!(*iter)->Insert(user_id, user_info)) {
          MCE_WARN("tt_handler->Insert(" << user_id <<") Failed");
          return false;
        } else {
          //异步调用, 维护内存id列表
          //MCE_DEBUG("[TEMPLE] in TTManagerI::Insert, begin invoke_insert_id_.invoke");
          invoke_insert_id_.invoke(user_id);
        }
      }
      return true;
    }

    bool TTManagerI::Inserts(const Int2ByteSeqMap& id_2_info_map, const Ice::Current&) {
      if (id_2_info_map.empty()) {
        return true;
      }
      //1. 取模分类
      map<int, Int2ByteSeqMap> mod_2_map;
      for (Int2ByteSeqMap::const_iterator iter=id_2_info_map.begin(); 
          iter!=id_2_info_map.end(); ++iter) {
        mod_2_map[iter->first % cluster_].insert(make_pair(iter->first, iter->second));
      }
      //2. 多线程并行插入
      TaskReferenceCount task_reference_count(cluster_);
      //多线程并行
      for (int i=0; i<cluster_; ++i) {
        TaskManager::instance().execute(new InsertUserInfoTask(i, mod_2_map[i], task_reference_count, kTaskLevelInsertUserInfo));
      }

      //3. 多线程查询汇合
      task_reference_count.Wait();
      return true;
    }

    bool TTManagerI::Inserts(int mod, Int2ByteSeqMap& id_2_info_map) {
      if (id_2_info_map.empty()) {
        return true;
      }
      if (mod<0 || mod >= cluster_) {
        return false;
      }

        vector<boost::shared_ptr<IDbCache> > writer_handler_vector = GetTTWriterHandler(mod);
        for(vector<boost::shared_ptr<IDbCache> >::iterator iter = writer_handler_vector.begin(); iter != writer_handler_vector.end(); ++iter) {
          if (NULL != (*iter).get()) {
            try {
              (*iter)->Insert(id_2_info_map);
            } catch (IceUtil::Exception& e) {
              MCE_WARN("Exception in TTManager::Insert(int mod, Int2ByteSeqMap& id_2_info_map) " << e.what());
            } catch (std::exception& e) {
              MCE_WARN("Exception in TTManager::Insert(int mod, Int2ByteSeqMap& id_2_info_map) " << e.what());
            } catch (...) {
              MCE_WARN("unknown exception in TTManager::Insert(int mod, Int2ByteSeqMap& id_2_info_map)");
            }
          }
        }

      return true;
    }


    bool TTManagerI::Remove(Ice::Int user_id, const Ice::Current&) {
      if (user_id < 0) {
        return false;
      }
      vector<boost::shared_ptr<IDbCache> > writer_handler_vector = GetTTWriterHandler(user_id);

      for(vector<boost::shared_ptr<IDbCache> >::iterator iter = writer_handler_vector.begin(); iter != writer_handler_vector.end(); ++iter) {
        if(!(*iter)->Remove(user_id)) {
          MCE_WARN("tt_handler->Remove(" << user_id <<") Failed");
          return false;
        } else {
          invoke_remove_id_.invoke(user_id);
        }
      }
      return true;
    }

    bool TTManagerI::DoInsertMemIds(const IntSeq& user_ids) {
#ifdef DEBUG
      MCE_DEBUG("Step in DoInsertMemIds, user_ids.size() is " << user_ids.size());
#endif
      if (0 == user_ids.size()) {
        return false;
      }
      for (IntSeq::const_iterator iter=user_ids.begin(); iter!=user_ids.end(); ++iter) {
        MCE_DEBUG("Set MemId for uid " << *iter);
        pbit_vector_->Set(*iter);
      }
      return true;
    }

    bool TTManagerI::InsertMemIds(const IntSeq& user_ids, const Ice::Current&) {
      if (0 == user_ids.size()) {
        return false;
      }
      return DoInsertMemIds(user_ids);
    }

    bool TTManagerI::DoRemoveMemIds(const IntSeq& user_ids) {
      if (0 == user_ids.size()) {
        return false;
      }
      for (IntSeq::const_iterator iter=user_ids.begin(); iter!=user_ids.end(); ++iter) {
        MCE_DEBUG("Clear MemId for uid " << *iter);
        pbit_vector_->Clear(*iter);
      }
      return true;
    }
    bool TTManagerI::RemoveMemIds(const IntSeq& user_ids, const Ice::Current&) {
      if (0 == user_ids.size()) {
        return false;
      }
      return DoRemoveMemIds(user_ids);
    }

    int TTManagerI::GetMemIdsCount(const Ice::Current&) {
      MCE_DEBUG("Step in TTManagerI::GetMemIdsCount");
      int result = get_bit_vector_().Count();
      MCE_DEBUG("Step out TTManagerI::GetMemIdsCount, result is " << result);
      return result;
    }

    int TTManagerI::GetMemIdsListSize(const Ice::Current&) {
      MCE_DEBUG("Step in TTManagerI::GetMemIdsListSize");
      int result = get_bit_vector_().Size();
      MCE_DEBUG("Step out TTManagerI::GetMemIdsListSize, result is " << result);
      return result;
    }

    bool TTManagerI::GetMemIdState(const UserId user_id, const Ice::Current&) {
      MCE_DEBUG("Step in TTManagerI::GetMemIdState, uid is " << user_id);
      bool result = get_bit_vector_().Get(user_id);
      MCE_DEBUG("Step out TTManagerI::GetMemIdState, result is " << result);
      return result;
    }
    /// @brief TTManagerI::Traversal 全遍历
    /// 
    /// @param start_pos (in) 本次读取的开始位置
    /// @param travel_length (out) 本次读取实际共遍历了多少个位置
    /// @param user_id_2_user_info_map (out) 实际读取到的内容，用户id到用户信息的映射
    /// @param Ice::Current& 
    /// 
    /// @return 有异常返回false， 正常返回true
    bool TTManagerI::Traversal(Ice::Int start_pos, Ice::Int& travel_length, Int2ByteSeqMap& user_id_2_user_info_map, const Ice::Current&ic) {
      MCE_DEBUG("[TEMPLE] Step in Travelsal, start_pos is " << start_pos);
      if (0 > start_pos) {
        MCE_ERROR("TTManagerI::Travelsal, start_pos=" << start_pos << ", less than zero");
        return false;
      }

      travel_length = 0;
      IntSeq user_ids;
#ifdef DEBUG_TIME
      struct timeval begin;
      struct timeval end;
      int bits_cost, gets_cost;
      gettimeofday(&begin, NULL);
#endif
      //TODO:注意退出时的边界条件
      //MCE_DEBUG("[TEMPLE] before While cycle, travel_length=" << travel_length << ", start_pos=" << start_pos <<",user_ids.size()=" << user_ids.size() << ", pbit_vector_->Size()=" << pbit_vector_->Size() << ", max_return_num_each_traversal_=" << max_return_num_each_traversal_);

      //注意：此处千万小心，一定不要出现死循环
      while (user_ids.size() < (unsigned int)max_return_num_each_traversal_ && start_pos+(travel_length-1) < pbit_vector_->Size()) {
        int user_id = start_pos + travel_length;
        std::vector<int> valid_user_ids;
        travel_length += pbit_vector_->GetsValidInNext64Bit(user_id, valid_user_ids);
        //MCE_DEBUG("[TEMPLE] in While cycle, travel_length=" << travel_length << ", start_pos=" << start_pos << ", valid_user_ids.size=" << valid_user_ids.size() << ", user_ids.size=" << user_ids.size() << ", max_return_num_each_traversal_=" << max_return_num_each_traversal_);
        user_ids.insert(user_ids.end(), valid_user_ids.begin(), valid_user_ids.end());
      }
#ifdef DEBUG_TIME
      gettimeofday(&end, NULL);
      bits_cost = (end.tv_sec*1000000+end.tv_usec) - (begin.tv_sec*1000000+begin.tv_usec);
#endif
      if (!Gets(user_ids, user_id_2_user_info_map, ic)) {
        MCE_DEBUG("[TEMPLE] Step out of Travelsal, Gets Failed");
        return false;
      }
#ifdef DEBUG_TIME
      gettimeofday(&begin, NULL);
      gets_cost = (begin.tv_sec*1000000+begin.tv_usec) - (end.tv_sec*1000000+end.tv_usec) ;
#endif

      //int message_size_max = ic.getProperties()->getPropertyAsInt("Ice.MessageSizeMax");
      //用于调试Ice.MessageSizeMax属性的设置 TODO: 正式版应去掉此调试信息
      int total_length = 0;
      for (Int2ByteSeqMap::const_iterator iter=user_id_2_user_info_map.begin(); iter!=user_id_2_user_info_map.end(); ++iter) {
        total_length +=  (iter->second).size();
      }
#ifdef DEBUG_TIME
      MCE_DEBUG("out Travelsal, start_pos is " << start_pos << ", traversal_length=" << travel_length <<", user_id_2_user_info_map.size() is " << user_id_2_user_info_map.size() << ", total_length is " << total_length << ", bits_cost="<< bits_cost << " us, gets_cost=" << gets_cost << " us");
#else 
      MCE_DEBUG("[TEMPLE] Step out of Travelsal, start_pos is " << start_pos << ", traversal_length=" << travel_length <<", user_id_2_user_info_map.size() is " << user_id_2_user_info_map.size() << ", total_length is " << total_length);
#endif
      return true;
    }


    /// @brief TraversalKey  顺序返回TT中存储的大于start_pos的一批用户id，用户id数目的多少由配置文件中"MaxReturnNumEachTraversal"设置。
    /// 
    /// @param start_pos (in) 本次读取的开始位置
    /// @param travel_length (out) 本次读取实际共遍历了多少个位置
    /// @param Ice::Current&ic (in)
    /// 
    /// @return 本次调用得到的有效用户id列表
    IntSeq TTManagerI::TraversalKey(Ice::Int start_pos, Ice::Int request_num, const Ice::Current&ic) {
      IntSeq user_ids;

      MCE_DEBUG("[TEMPLE] Step in TravelsalKey, start_pos is " << start_pos);
      if (0 > start_pos || request_num <= 0) {
        MCE_ERROR("TTManagerI::Travelsal, start_pos=" << start_pos << ", request_num="  << request_num);
        return user_ids;
      }

      int travel_length = 0;
      //TODO:注意退出时的边界条件
      //MCE_DEBUG("[TEMPLE] before While cycle, travel_length=" << travel_length << ", start_pos=" << start_pos <<",user_ids.size()=" << user_ids.size() << ", pbit_vector_->Size()=" << pbit_vector_->Size() << ", max_return_num_each_traversal_=" << max_return_num_each_traversal_);

      //注意：此处千万小心，一定不要出现死循环
      while (user_ids.size() < (unsigned int)request_num && start_pos+(travel_length-1) < pbit_vector_->Size()) {
        int user_id = start_pos + travel_length;
        std::vector<int> valid_user_ids;
        travel_length += pbit_vector_->GetsValidInNext64Bit(user_id, valid_user_ids);
        //MCE_INFO("[TEMPLE] in While cycle, travel_length=" << travel_length << ", start_pos=" << start_pos << ", valid_user_ids.size=" << valid_user_ids.size() << ", user_ids.size=" << user_ids.size() << ", max_return_num_each_traversal_=" << max_return_num_each_traversal_);
        user_ids.insert(user_ids.end(), valid_user_ids.begin(), valid_user_ids.end());
      }

      if (user_ids.size() > (unsigned int)request_num)  {
        user_ids.resize((unsigned int)request_num);
      }

      return user_ids;
    }



    /// @brief LoadIdList 将所有TT中的用户Id加载到内存中，用于遍历
    /// 
    /// @param load_id_list_from_file 0表示从文件加载id列表，1表示从TT中加载id列表
    /// @param file_name 存放id列表的文件

    LoadIdListTask::LoadIdListTask(const bool use_id_list_file_flag, const string& file_name) : MyUtil::Task(kTaskLevelMonitorUpdateIdListFile), use_id_list_file_flag_(use_id_list_file_flag), id_list_file_(file_name) {
    }
    void LoadIdListTask::handle() {
      MCE_DEBUG("[TEMPLE] step in LoadIdList...");
      //读二进制的user id 列表文件，加载到内存列表中
      try {
        if (use_id_list_file_flag_) {
          //从二进制文件读很快，1亿个int几秒钟即可
          TTManagerI::instance().get_bit_vector_().Load(id_list_file_.c_str());
          MCE_INFO("Load Mem Id List From File " << id_list_file_ << ", total valid user id num is " << TTManagerI::instance().get_bit_vector_().Count());
          //直接从所有TT中遍历得到所有的user id，加载到内存列表中
        } else {
          //*******************************以下为多线程加载tt中的id到内存id列表
          vector<IceUtil::ThreadControl> vector_load_thread;
          for (int i=0; i<TTManagerI::instance().get_cluster_(); ++i) {
            IceUtil::ThreadPtr t = new LoadThread(i, id_list_file_);
            vector_load_thread.push_back(t->start(128*1024));
          }

          //线程汇合
          for (vector<IceUtil::ThreadControl>::iterator iter=vector_load_thread.begin();
              iter!=vector_load_thread.end(); ++iter) {
            iter->join();
          }
          /****************************** 
            for (int i=0; i<TTManagerI::instance().get_cluster_(); ++i) {
            boost::shared_ptr<IDbCache> tt_handler = TTManagerI::instance().GetTTReaderHandler(i);
            if (tt_handler) {
            IDbResult* pDbResult = tt_handler->Query();
            UserId user_id;
            int idx = 0;
            while (pDbResult->FetchObject(user_id)) {
            TTManagerI::instance().get_bit_vector_().Set(user_id);
            if (0 == ++idx % 1000) {
            MCE_DEBUG("[TEMPLE]:" << idx << " user ids has been Load");
            }
            }
            MCE_INFO("LoadThread for mod " << i << " done");
            }
            }
           ******************************/

          TTManagerI::instance().get_bit_vector_().Save(id_list_file_.c_str());
        }
      } catch (IceUtil::Exception& e) {
        MCE_FATAL("IceUtil " << e.what());
      } catch (std::exception e) {
        MCE_FATAL("std::exception " << e.what());
      }


      //将遍历TT得到的id列表保持到硬盘文件中，以便于万一TTManager挂掉后能及时重启。
      MCE_DEBUG("[TEMPLE] step out LoadIdList...");
    }

    MonitorUpdateIdListFileTask::MonitorUpdateIdListFileTask(string id_list_file, string update_id_list_file_tag) : MyUtil::Task(kTaskLevelMonitorUpdateIdListFile), id_list_file_(id_list_file), update_id_list_file_tag_(update_id_list_file_tag) {
    }

    void MonitorUpdateIdListFileTask::handle() {
      while (1) {
        //若标记文件存在，更新id列表文件，并删除标记文件
        if (0 == access(update_id_list_file_tag_.c_str(), F_OK)) {

          if (0 > TTManagerI::instance().get_bit_vector_().Save(id_list_file_.c_str())) {
            MCE_WARN("Update id list file Failed!");
          } else {
            Ice::Current ic;
            MCE_INFO("Save id list file to " << id_list_file_ << ", mem id list size is " << TTManagerI::instance().GetMemIdsListSize(ic));
          }

          if (0 != remove(update_id_list_file_tag_.c_str())) { 
            MCE_WARN("ERROR! Remove " << update_id_list_file_tag_ << " Failded!");
          } else {
            MCE_INFO("remove tag file Succeed!");
          }
        }

        sleep(60);
      }
    }

    GetUserInfoTask::GetUserInfoTask(const int mod, const IntSeq& user_ids, Int2ByteSeqMap& user_id_2_user_info_map, TaskReferenceCount& task_reference_count, int level) : 
      Task(level), mod_(mod), user_ids_(user_ids), user_id_2_user_info_map_(user_id_2_user_info_map), task_reference_count_(task_reference_count) {
      }

    void GetUserInfoTask::handle() {
#ifdef DEBUG
      MCE_DEBUG("[TEMPLE] Step in GetUserInfoTask::handle, mod_ is " << mod_ <<", user_ids_.size() is " << user_ids_.size() << ", user_id_2_user_info_map_.size() is " << user_id_2_user_info_map_.size() << "\tsize=" << TaskManager::instance().size(kTaskLevelGetUserInfo)) ;
      //MCE_DEBUG("[TEMPLE] sleep 3 sec");
      //sleep(3);
#endif
      try {
        TTManagerI::instance().Gets(user_ids_, mod_, user_id_2_user_info_map_);
      } catch (...) {
        MCE_FATAL("Fatal in GetUserInfoTask");
      }

      MCE_DEBUG("[TEMPLE] Step out GetUserInfoTask::handle, user_ids_.size()= " << user_ids_.size()<< ", user_id_2_user_info_map_.size() is " << user_id_2_user_info_map_.size());
      task_reference_count_.Post();
    }


    InsertUserInfoTask::InsertUserInfoTask(const int mod, Int2ByteSeqMap& user_id_2_user_info_map, TaskReferenceCount& task_reference_count, int level) :
      Task(level), mod_(mod), user_id_2_user_info_map_(user_id_2_user_info_map), task_reference_count_(task_reference_count) {

      }
    void InsertUserInfoTask::handle() {
      try {
        TTManagerI::instance().Inserts(mod_, user_id_2_user_info_map_);
      } catch (IceUtil::Exception& e) {
        MCE_WARN("Exception while InsertUserInfoTask " << e.what());
      } catch (std::exception& e) {
        MCE_WARN("Exception while InsertUserInfoTask " << e.what());
      } catch (...) {
        MCE_WARN("unknown Exception while InsertUserInfoTask");
      }

      task_reference_count_.Post();
    }

  }
}

