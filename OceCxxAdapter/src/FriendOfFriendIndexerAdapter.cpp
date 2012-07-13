#include "FriendOfFriendIndexerAdapter.h"

using namespace MyUtil;
namespace com {
  namespace renren {
    namespace search2 {
      namespace friendoffriend {

        const int kTaskLevelForRebuildIndex = 6;
        const int kTaskLevelForBuildIndex = 7;

        FriendOfFriendIndexerAdapter::FriendOfFriendIndexerAdapter() {
          invoke_rebuild_index_.start(128*1024).detach();
          invoke_build_index_.start(128*1024).detach();
          managers_.resize(cluster() > 0 ? cluster() : 1);

          //数字越大，级别越高，线程数越多, 执行的机会更多
          TaskManager::instance().config(kTaskLevelForRebuildIndex, ThreadPoolConfig(8,16));
          TaskManager::instance().config(kTaskLevelForBuildIndex, ThreadPoolConfig(16,32));
        }

        FriendOfFriendIndexerManagerPrx FriendOfFriendIndexerAdapter::GetFriendOfFriendIndexerManager (int user_id) {
          FriendOfFriendIndexerManagerPrx proxy;
          if (user_id < 0) {
            return proxy;
          }

          if (proxies_.empty()) {
            //为负载均衡
            for (int i=0; i<kProxyNum; ++i) {
              FriendOfFriendIndexerManagerPrx temp_proxy = locate<FriendOfFriendIndexerManagerPrx>(managers_, "FOF", 0, TWO_WAY, 5000)->ice_locatorCacheTimeout(60)->ice_connectionCached(false);
              MCE_TRACE("proxy[" << i << "]=" << temp_proxy->ice_getConnection()->toString());
              proxies_.push_back(temp_proxy);
            }
          }

          srand((unsigned)time(0));
          int rand_index = rand() % proxies_.size();
          MCE_TRACE("select proxy [ " << rand_index  << "]=" << proxies_[rand_index]->ice_getConnection()->toString());
          return proxies_[rand_index];
        }

        bool FriendOfFriendIndexerAdapter::BuildIndex(const int user_id, const Ice::Short level) {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::BuildIndex " << user_id << ", level=" << level);
          if (user_id < 0) {
            return false;
          }
          FriendOfFriendIndexerManagerPrx proxy = GetFriendOfFriendIndexerManager(user_id);
          if (proxy) {
            try {
              return proxy->BuildIndex(user_id, level);
            } catch (Ice::Exception& e) {
              MCE_FATAL("FriendOfFriendIndexerAdapter::BuildIndex Ice::Exception " << e);
            } catch (std::exception& e) {
              MCE_FATAL("FriendOfFriendIndexerAdapter::BuildIndex std::exception " << e.what());
            } catch (...) {
              MCE_FATAL("FriendOfFriendIndexerAdapter::BuildIndex Unknown exception");
            }
          }
          return false;
        }

        bool FriendOfFriendIndexerAdapter::RebuildIndex(const int user_id, const Ice::Short level) {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::RebuildIndex " << user_id << ", level=" << level);
          if (user_id < 0) {
            return false;
          }
          FriendOfFriendIndexerManagerPrx proxy = GetFriendOfFriendIndexerManager(user_id);
          if (proxy) {
            try {
              return proxy->RebuildAllRelatedIndex(user_id, level);
            } catch (Ice::Exception& e) {
              MCE_FATAL("FriendOfFriendIndexerAdapter::RebuildIndexForNameChange Ice::Exception " << e);
            } catch (std::exception& e) {
              MCE_FATAL("FriendOfFriendIndexerAdapter::RebuildIndexForNameChange std::exception " << e.what());
            } catch (...) {
              MCE_FATAL("FriendOfFriendIndexerAdapter::RebuildIndexForNameChange Unknown exception");
            }
          }
          return false;
        }

        bool FriendOfFriendIndexerAdapter::BuildIndexAsync(const int user_id, const Ice::Short level) {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::BuildIndex_Async id=" << user_id << ", level=" << level);
          if (user_id < 0) {
            return false;
          }
          return invoke_build_index_.invoke(user_id, level);
        }

        bool FriendOfFriendIndexerAdapter::RebuildIndexAsync(const int user_id, const Ice::Short level) {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::BuildIndex id=" << user_id << ", level=" << level);
          if (user_id < 0) {
            return false;
          }
          return invoke_rebuild_index_.invoke(user_id, level);
        }

        bool FriendOfFriendIndexerAdapter::RebuildIndexThread::invoke(const int user_id, const Ice::Short level) {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::RebuildIndexThread::invoke " << user_id << ", ");
          if (user_id < 0) {
            return false;
          }
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
          cache_.push(Pair(user_id, level));
          mutex_.notify();
          return true;
        }

        void FriendOfFriendIndexerAdapter::RebuildIndexThread::run() {
          while (true) {
            try {
              {
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
                if (cache_.empty()) {
                  mutex_.timedWait(IceUtil::Time::seconds(1));
                }
                if (!cache_.empty()) {
                  MyUtil::TaskManager::instance().execute(new RebuildIndexTask(cache_.front()));
                  cache_.pop();
                }
              }
            } catch (...) {
              MCE_WARN("Error while RebuildIndexThread run ");
            }
          }
        }

        FriendOfFriendIndexerAdapter::RebuildIndexTask::RebuildIndexTask(Pair& pair) : Task(kTaskLevelForRebuildIndex), pair_(pair) {
        }

        FriendOfFriendIndexerAdapter::RebuildIndexTask::~RebuildIndexTask() { }

        void FriendOfFriendIndexerAdapter::RebuildIndexTask::handle() {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::RebuildIndexTask::handle(), id_=" << pair_.id_ << ", level=" << pair_.level_);
          FriendOfFriendIndexerAdapter::instance().RebuildIndex(pair_.id_, pair_.level_);
        }

        //TODO: RebuildIndex、BuildIndex 异步线程机制代码重复，尝试模块化
        bool FriendOfFriendIndexerAdapter::BuildIndexThread::invoke(const int user_id, const Ice::Short level) {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::BuildIndexThread::invoke " << user_id);
          if (user_id < 0) {
            return false;
          }
          IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
          cache_.push(Pair(user_id, level));
          mutex_.notify();
          return true;
        }

        void FriendOfFriendIndexerAdapter::BuildIndexThread::run() {
          while (true) {
            try {
              {
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
                if (cache_.empty()) {
                  mutex_.timedWait(IceUtil::Time::seconds(1));
                }
                if (!cache_.empty()) {
                  MyUtil::TaskManager::instance().execute(new BuildIndexTask(cache_.front()));
                  cache_.pop();
                }
              }
            } catch (...) {
              MCE_WARN("Error while BuildIndexThread run ");
            }
          }
        }

        FriendOfFriendIndexerAdapter::BuildIndexTask::BuildIndexTask(Pair& pair) : Task(kTaskLevelForBuildIndex), pair_(pair) {
        }


        FriendOfFriendIndexerAdapter::BuildIndexTask::~BuildIndexTask() { }

        void FriendOfFriendIndexerAdapter::BuildIndexTask::handle() {
          MCE_DEBUG("FriendOfFriendIndexerAdapter::BuildIndexTask::handle(), id_=" << pair_.id_ << ", level=" << pair_.level_);
          FriendOfFriendIndexerAdapter::instance().BuildIndex(pair_.id_, pair_.level_);
        }

      }
    }
  }
}
