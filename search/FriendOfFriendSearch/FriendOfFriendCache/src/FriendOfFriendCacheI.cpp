#include "FriendOfFriendCacheI.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#define DEBUG_TIME

#ifdef DEBUG_TIME
#include <sys/time.h>
#endif

using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace com::renren::search2::friendoffriend;
//***************************************************************************

const string kFriendOfFriendCacheManager = "FOF";

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.getAdapter()->add(&FriendOfFriendCacheManagerI::instance(),
      service.createIdentity(kFriendOfFriendCacheManager, ""));

  FriendOfFriendCacheManagerI::instance().bootstrap();
}

namespace com {
  namespace renren {
    namespace search2 {
      namespace friendoffriend {
        typedef __gnu_cxx::hash_map<int, FriendInfo> Int2FriendInfoMap;

        using xce::buddy::adapter::BuddyByIdCacheAdapter;
        const int kHashSetDefaultSize = 1000;
        const string kDBSearchFriendCount = "search_user_friendcount";
        const int kTaskLevelOfUpdateSpecialUserIdSet = 1;

        /// @brief 一级消息队列的线程数
        const unsigned int kFirstLevelMessageQueueThreadNum = 10;
        /// @brief 一级线程中每个线程能存储的最大消息个数
        const unsigned int kFirstLevelThreadMaxMessageNum = 10000;

        FriendOfFriendCacheManagerI::FriendOfFriendCacheManagerI() : special_user_id_set_(kHashSetDefaultSize) {
          for (unsigned int i=0; i<kFirstLevelMessageQueueThreadNum; ++i) {
            first_level_message_queue_thread_vec_.push_back(new FirstLevelMessageQueueThread(i));
            (first_level_message_queue_thread_vec_.at(i))->start(128*1024).detach();
          }
        }

        /// @brief FriendOfFriendCacheManagerI::bootstrap
        /// 与业务逻辑相关的初始化
        void FriendOfFriendCacheManagerI::bootstrap() {
          ServiceI& service = ServiceI::instance();
          cluster_ = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendCache.Cluster");
          MCE_INFO("Cluster=" << cluster_);
          string mem_cache_url = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendCache.MemCacheURL");
          MCE_INFO("FriendOfFriendCache.MemCacheURL=" << mem_cache_url);
          mod_ = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendCache.Mod");
          MCE_INFO("Mod=" << mod_);
          mem_cache_handler_.SetServers(mem_cache_url.c_str());

          TaskManager::instance().execute(new UpdateSpecialUserIdSetTask());
        }


        //时间复杂度 N*N
        bool FriendOfFriendCacheManagerI::load(const int user_id, const Ice::Current& ic) {
          MCE_INFO("FriendOfFriendCacheManagerI::load " << user_id);
          if (0 > user_id) 
            return false;
          {
            IceUtil::RWRecMutex::RLock lock(set_rw_rec_lock_);
            if (!special_user_id_set_.empty() && special_user_id_set_.find(user_id) != special_user_id_set_.end()) {
              MCE_INFO("find " << user_id << " in special_user_id_set_, his friends num is so huge, so dump it");
              return false;
            }
          }
#ifdef DEBUG_TIME
          try {
            struct timeval begin;
            struct timeval end;
            long getFriendListAsc_cost = 0; 
            long getFriendLists_cost = 0; 
            long traversal_cost = 0;
            gettimeofday(&begin, NULL);
#endif
            //取一度好友 , 注意： -1表示不限制结果数
            IntSeq direct_friends_list = BuddyByIdCacheAdapter::instance().getFriendListAsc(user_id, -1);
            //朋友数为0，不处理，返回
            if (direct_friends_list.empty()) {
              return false;
            }
#ifdef DEBUG_TIME
            gettimeofday(&end, NULL);
            getFriendListAsc_cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);
            //MCE_INFO("getFriendListAsc cost " << cost / 1000 << " ms");
#endif
            //hash_map预分配，减少resize次数，提高性能，用空间换时间
            Int2FriendInfoMap id_2_friend_info_map(500000); //好友id->好友类型相关信息

            //1.先把自己的信息存进去，这是边界值，自己与自己是0度好友, 共同好友数无意义
            {
              FriendInfo info;
              info.id = user_id;
              info.type = 0;
              info.commonFriendsNum = 0;
              id_2_friend_info_map.insert(make_pair(user_id, info));
            }

            //2.存储一度好友的相关信息
            for(IntSeq::const_iterator iter=direct_friends_list.begin(); iter!=direct_friends_list.end(); ++iter) {
              
              {
                IceUtil::RWRecMutex::RLock lock(set_rw_rec_lock_);
                if (!special_user_id_set_.empty() && special_user_id_set_.find(*iter) != special_user_id_set_.end()) {
                  MCE_INFO("find " << *iter << " in special_user_id_set_, his friends num is so huge, so dump it");
                  continue;
                }
              }
              FriendInfo info;
              info.id = *iter;
              info.type = 1;
              info.commonFriendsNum = 0;
              id_2_friend_info_map.insert(make_pair(*iter, info));
            }

            //取二度好友
            if (0 != direct_friends_list.size()) {
              //极端情况下，用户的一度好友数会比较多，一次全部批量进行查询，会导致超时，故分批查询，每次最多100个 
              size_t offset = 0;
              while(offset*100 < direct_friends_list.size()) {
                //本批要查询的一批好友的id(都是一度好友)
                IntSeq current_batch_direct_friends ; 

                current_batch_direct_friends.assign(direct_friends_list.begin() + offset*100, ((unsigned int)offset+1)*100 > direct_friends_list.size() ? direct_friends_list.end() : direct_friends_list.begin() + (offset+1)*100);
                ++offset;

                //一度好友id -> 该id的所有好友
                Int2IntSeqMap id_2_indirect_friends_map = BuddyByIdCacheAdapter::instance().getFriendLists(current_batch_direct_friends);

#ifdef DEBUG_TIME
                gettimeofday(&begin, NULL);
                getFriendLists_cost = (begin.tv_sec*1000000 + begin.tv_usec) - (end.tv_sec*1000000 + end.tv_usec) ;
#endif
                for (Int2IntSeqMap::iterator id_2_indirect_friends_map_iter = id_2_indirect_friends_map.begin(); 
                    id_2_indirect_friends_map_iter!=id_2_indirect_friends_map.end(); ++id_2_indirect_friends_map_iter) {

                  for (IntSeq::iterator indirect_friends_iter=(id_2_indirect_friends_map_iter->second).begin();
                      indirect_friends_iter!=(id_2_indirect_friends_map_iter->second).end(); ++indirect_friends_iter) {

                    {
                      IceUtil::RWRecMutex::RLock lock(set_rw_rec_lock_);
                      if (!special_user_id_set_.empty() && special_user_id_set_.find(*indirect_friends_iter) != special_user_id_set_.end()) {
                        MCE_INFO("find " << *indirect_friends_iter << " in special_user_id_set_, his friends num is so huge, so dump it");
                        continue;
                      }
                    }

                    Int2FriendInfoMap::iterator pos_iter = id_2_friend_info_map.find(*indirect_friends_iter);
                    //已经确定好友的类型了，只需更改共同好友数
                    if (pos_iter != id_2_friend_info_map.end()) {
                      ++(pos_iter->second).commonFriendsNum;
                    } else {
                      //到此时还未确定好友类型， 则一定是二度好友
                      FriendInfo info;
                      info.id = *indirect_friends_iter;
                      info.type = 2;
                      info.commonFriendsNum = 1;
                      //3.存储二度好友的相关信息
                      id_2_friend_info_map.insert(make_pair(info.id, info));
                    }
                  }
                }

              }
            }

#ifdef DEBUG_TIME
            gettimeofday(&end, NULL);
            traversal_cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);
            //MCE_INFO("Traversal cost " << cost / 1000 << " ms");
#endif

            //在缓存中存储二度好友(id_2_friend_info_map);
            FriendInfoSeq friend_info_seq;
            for (Int2FriendInfoMap::iterator iter=id_2_friend_info_map.begin(); iter!=id_2_friend_info_map.end(); ++iter) {
              friend_info_seq.push_back(iter->second);
            }
            mem_cache_handler_.Set(user_id, friend_info_seq);

            MCE_INFO("uid=" << user_id << ", first_degree=" << direct_friends_list.size() << ", total_num=" << id_2_friend_info_map.size() << ", ""getFriendListAsc_cost=" << getFriendListAsc_cost << ", getFriendLists_cost=" << getFriendLists_cost << " , traversal_cost=" << traversal_cost << ", space of friend_info_seq=" << friend_info_seq.size()*sizeof(FriendInfo));

          } catch (IceUtil::Exception& e) {
            MCE_WARN("FriendOfFriendCacheManagerI::load "<<user_id << " occurs IceUtil::Exception! " << e.what()); 
          } catch (std::exception& e) {
            MCE_WARN("FriendOfFriendCacheManagerI::load "<< user_id << " occurs std::exception! " << e.what()); 
          } catch (...) {
            MCE_WARN("FriendOfFriendCacheManagerI::load " << user_id << " occurs unknown exception! "); 
          }

          return true;
        }

        FriendInfoSeq FriendOfFriendCacheManagerI::get(const int user_id, const int begin, const int limit, 
            bool& is_finished, const Ice::Current&ic) {
          const Ice::ConnectionPtr con = ic.con;
          if (con.get() != NULL) {
            MCE_INFO("FriendOfFriendCacheManagerI::get, uid=" << user_id << ", begin=" << begin << ", limit=" << limit << ", invoker is :" << con->toString());
          }
          is_finished = true; //默认应该结束

          FriendInfoSeq results;
          if (0 > user_id) {
            return results;
          }

          try {
            FriendInfoSeq friend_info_seq;
            if (!mem_cache_handler_.Get(user_id, friend_info_seq)) {
              load(user_id, ic);
              if (!mem_cache_handler_.Get(user_id, friend_info_seq)) {
                return results;
              }
            } 

            int size = friend_info_seq.size();
            int real_limit;
            if (0 == begin && -1 == limit) {
              return friend_info_seq;
            } else if (begin < size && begin + limit >= size) {
              real_limit = size-begin;
              results.assign(friend_info_seq.begin()+begin, friend_info_seq.begin()+begin+real_limit);
              results.assign(friend_info_seq.begin()+begin, friend_info_seq.begin()+begin+real_limit);
              return results;
            } else if (begin < size && begin + limit < size) {
              is_finished = false;
              real_limit = limit;
              results.assign(friend_info_seq.begin()+begin, friend_info_seq.begin()+begin+real_limit);
              return results;
            } 
          } catch (IceUtil::Exception& e) {
            MCE_WARN("FriendOfFriendCacheManagerI::get occurs IceUtil::Exception! " << e.what()); 
          } catch (std::exception& e) {
            MCE_WARN("FriendOfFriendCacheManagerI::get occurs std::exception! " << e.what()); 
          } catch (...) {
            MCE_WARN("FriendOfFriendCacheManagerI::get occurs unknown exception! "); 
          }

          return results;
        }


        bool FriendOfFriendCacheManagerI::remove(const int user_id, 
            const Ice::Current& ic) {
          MCE_INFO("remove " << user_id);
          if (!mem_cache_handler_.Erase(user_id)) {
            MCE_DEBUG("mem_cache_handler_.Erase(" << user_id << ") Failed!");
          }
          return true;
        }


        bool FriendOfFriendCacheManagerI::UpdateAllRelatedCache(const int user_id, 
            const Ice::Current& ic) {
          const Ice::ConnectionPtr con = ic.con;
          if (con.get() != NULL) {
            MCE_INFO("FriendOfFriendCacheManagerI::UpdateAllRelatedCache, uid=" << user_id  <<", invoker is :" << con->toString() );
          }     
          if (user_id > 0) {
            (first_level_message_queue_thread_vec_.at(user_id % kFirstLevelMessageQueueThreadNum))->invoke(user_id);
          }
          return true;
        }

        bool FriendOfFriendCacheManagerI::UpdateAllRelatedCache(const int user_id) {
          FriendInfoSeq friend_info_seq;
          Ice::Current ic;
          if (!mem_cache_handler_.Get(user_id, friend_info_seq)) {
            load(user_id, ic);
            if (mem_cache_handler_.Get(user_id, friend_info_seq)) {
              for (FriendInfoSeq::iterator iter=friend_info_seq.begin(); iter!=friend_info_seq.end(); ++iter) {
                MCE_INFO("Erase " << iter->id)
                mem_cache_handler_.Erase(iter->id);
              }
            }
          } else {
            for (FriendInfoSeq::iterator iter=friend_info_seq.begin(); iter!=friend_info_seq.end(); ++iter) {
                mem_cache_handler_.Erase(iter->id);
            }
            load(user_id, ic);
          }

          return true;
        }

        bool FriendOfFriendCacheManagerI::UpdateSpecialUserIdSet(hash_set<int>& new_set) {
          IceUtil::RWRecMutex::WLock lock(set_rw_rec_lock_);
          if (new_set.size() > special_user_id_set_.size()) {
            special_user_id_set_.swap(new_set);
          }
          MCE_DEBUG("FriendOfFriendCacheManagerI::UpdateSpecialUserIdSet done, special_user_id_set_.size()=" << special_user_id_set_.size());
          return true;
        }

        UpdateSpecialUserIdSetTask::UpdateSpecialUserIdSetTask() : Task(kTaskLevelOfUpdateSpecialUserIdSet) {
        }

        void UpdateSpecialUserIdSetTask::handle() {
          while (true) {

            Statement sql;
            sql << "SELECT id from user_friendcount where  friendcount > " << 2000;
            hash_set<int> temp_set(kHashSetDefaultSize);
            BatchResultHandlerI handler(temp_set);
            QueryRunner(kDBSearchFriendCount, CDbRServer).query(sql, handler);

            MCE_INFO("UpdateSpecialUserIdSetTask Query MYSQL once done, temp_set.size()=" << temp_set.size() << ", sleep 60*60 sec...");

            FriendOfFriendCacheManagerI::instance().UpdateSpecialUserIdSet(temp_set);

            //每小时检查一次
            sleep(60*60);
          }
        }

        
        BatchResultHandlerI::BatchResultHandlerI(hash_set<int>& special_user_id_set) : special_user_id_set_(special_user_id_set) {
        }
        bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
          //TODO:
          int user_id = static_cast<int>(row["id"]);
          special_user_id_set_.insert(user_id);
          return true;
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
              MCE_INFO("UpdateCache Thread, mod=" << mod_ << ", drop id " << temp_id);
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
              FriendOfFriendCacheManagerI::instance().UpdateAllRelatedCache(user_id);
              //删除与该用户相关的人的索引(不包括自己), 异步调用
              MCE_INFO("UpdateCache Thread, mod=" << mod_ << ", cache_.size()=" << cache_.size());
            }
          }
        }
      }
    }
  }
}
