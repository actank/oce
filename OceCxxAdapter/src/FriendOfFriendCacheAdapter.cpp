#include "FriendOfFriendCacheAdapter.h"
#include <ctime>
#include <vector>
//TODO: 1. 所有接口提供批量操作 2. 利用异步机制
//

using namespace MyUtil;
namespace com {
namespace renren {
namespace search2 {
namespace friendoffriend {
  const int kBatchNum = 10000;
  using std::vector;

  FriendOfFriendCacheAdapter::FriendOfFriendCacheAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
  }

  FriendOfFriendCacheManagerPrx FriendOfFriendCacheAdapter::GetFriendOfFriendCacheManager (int id) {
    if (_proxies.empty()) {
      //为负载均衡
      for (int i=0; i<kProxyNum; ++i) {
        FriendOfFriendCacheManagerPrx temp_proxy = locate<FriendOfFriendCacheManagerPrx>(_managers, "FOF", 0, TWO_WAY, 5000)->ice_locatorCacheTimeout(60)->ice_connectionCached(false);
        MCE_INFO("proxy[" << i << "]=" << temp_proxy->ice_getConnection()->toString());
        _proxies.push_back(temp_proxy);
      }
    }

    srand((unsigned)time(0));
    int rand_index = rand() % _proxies.size();
    MCE_INFO("select proxy [ " << rand_index  << "]=" << _proxies[rand_index]->ice_getConnection()->toString());
    return _proxies[rand_index];

  }

  FriendInfoSeq FriendOfFriendCacheAdapter::get(int user_id) {
    FriendInfoSeq info_seq;
    if (user_id < 0) {
      return info_seq;
    }

    FriendOfFriendCacheManagerPrx proxy = GetFriendOfFriendCacheManager(user_id);
    if (proxy) {
      try {
        FriendInfoSeq result;
        //防止二度好友数过多，分批取
        int begin = 0;
        int limit = kBatchNum;
        bool is_finished = true;
        while (true) {
          FriendInfoSeq temp_result = proxy->get(user_id, begin, limit, is_finished);
          result.insert(result.end(), temp_result.begin(), temp_result.end());
          //退出条件
          if (is_finished) break;
          begin += limit;
        } 
        //MCE_INFO("Final result.size=" << result.size());
        return result;
      } catch (Ice::Exception& e) {
        MCE_FATAL("FriendOfFriendCacheAdapter::get Ice::Exception " << e << ", proxy=" << proxy);
      } catch (std::exception& e) {
        MCE_FATAL("FriendOfFriendCacheAdapter::get std::exception " << e.what() << ", proxy=" << proxy);
      } catch (...) {
        MCE_FATAL("FriendOfFriendCacheAdapter::get Unknown exception" << ", proxy=" << proxy);
      }
    }

    return info_seq;
  }

  bool FriendOfFriendCacheAdapter::load(int user_id) {
      FriendOfFriendCacheManagerPrx proxy = GetFriendOfFriendCacheManager(user_id);
      if (proxy) {
        try {
            return proxy->load(user_id);
        } catch (Ice::Exception& e) {
          MCE_FATAL("FriendOfFriendCacheAdapter::load Ice::Exception " << e);
        } catch (std::exception& e) {
          MCE_FATAL("FriendOfFriendCacheAdapter::load std::exception " << e.what());
        } catch (...) {
          MCE_FATAL("FriendOfFriendCacheAdapter::load Unknown exception");
        }
      }

    return false;
  }

  bool FriendOfFriendCacheAdapter::UpdateAllRelatedCache(int user_id) {

    FriendOfFriendCacheManagerPrx proxy = GetFriendOfFriendCacheManager(user_id);
    if (proxy) {
      try {
          return proxy->UpdateAllRelatedCache(user_id);
      } catch (Ice::Exception& e) {
        MCE_FATAL("FriendOfFriendCacheAdapter::UpdateAllRelatedCache Ice::Exception " << e);
      } catch (std::exception& e) {
        MCE_FATAL("FriendOfFriendCacheAdapter::UpdateAllRelatedCache std::exception " << e.what());
      } catch (...) {
        MCE_FATAL("FriendOfFriendCacheAdapter::UpdateAllRelatedCache Unknown exception");
      }
    }

    return false;
  }

  bool FriendOfFriendCacheAdapter::remove(int user_id) {
    MCE_INFO("FriendOfFriendCacheAdapter::remove" << user_id);
    FriendOfFriendCacheManagerPrx proxy = GetFriendOfFriendCacheManager(user_id);
    if (proxy) {
      try {
        return proxy->remove(user_id);
      } catch (Ice::Exception& e) {
        MCE_FATAL("FriendOfFriendCacheAdapter::remove Ice::Exception " << e);
      } catch (std::exception& e) {
        MCE_FATAL("FriendOfFriendCacheAdapter::remove std::exception " << e.what());
      } catch (...) {
        MCE_FATAL("FriendOfFriendCacheAdapter::remove Unknown exception");
      }
    }

    return false;
  }

}
}
}
}
