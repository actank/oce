#ifndef XCE_BASE_ADAPTER2_H__
#define XCE_BASE_ADAPTER2_H__

#include <sstream>
#include <map>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "base/asyncall.h"
#include <Ice/Ice.h>
#include "base/common.h"
#include "base/logging.h"
#include "base/network.h"
#include "base/rwlock.h"
#include "UtilSlice/slice/ReplicaCluster.h" // 支持usercache等模式

namespace xce {

#define DEFAULT_ARRAY \
  {"Ice.Override.ConnectTimeout", "300"},     \
  {"Ice.MessageSizeMax", "20480"},            \
  {"Ice.Default.Host", Network::local_ip().c_str()},

#if 0 // TODO: WORDSIZE == 32
  {"Ice.ThreadPool.Client.StackSize", "65535"},
  {"Ice.ThreadPool.Server.StackSize", "65535"},
#endif

struct Grid {
  struct StringPair {
    const char* key;
    const char* value;
  };

  Ice::CommunicatorPtr c_;

  // xiaonei, XiaoNeiRegistry 12000
  Grid(const char* name, const char* host, int port) {
    // "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"
    std::ostringstream oss;
    oss << name << "/Locator:tcp -h " << host
      << " -p " << port;

    std::string str(oss.str());

    const StringPair arr [] = {
      DEFAULT_ARRAY
      {"IceGrid.InstanceName", name},
      {"Ice.Default.Locator", str.c_str()},
      {0, 0}
    };
    c_ = CreateCommunicator(arr);
  }

  // "xiaonei", "host:port"
  // 参数不正确，可能会导致崩溃
  Grid(const char* name, const std::string &host_and_port) {
    // xiaonei 10.22.202.47:12000
    std::string::size_type pos = host_and_port.find_first_of(':');
    ASSERT(pos != std::string::npos);
    std::string host = host_and_port.substr(0, pos);
    std::string sport = host_and_port.substr(pos + 1);

    std::ostringstream oss;
    oss << name << "/Locator:tcp -h " << host
      << " -p " << sport;
    std::string str(oss.str());

    const StringPair arr [] = {
      DEFAULT_ARRAY
      {"IceGrid.InstanceName", name},
      {"Ice.Default.Locator", str.c_str()},
      {0, 0}
    };
    c_ = CreateCommunicator(arr);
  }

  // "XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002"
  Grid(const std::string &default_locator) {
    std::string::size_type pos = default_locator.find_first_of('/');
    ASSERT(pos != std::string::npos);
    std::string name = default_locator.substr(0, pos);

    const StringPair arr [] = {
      DEFAULT_ARRAY
      {"IceGrid.InstanceName", name.c_str()},
      {"Ice.Default.Locator", default_locator.c_str()},
      {0, 0}
    };
    c_ = CreateCommunicator(arr);
  }

  // TODO: hosts ports
  Grid(const StringPair* properties);
  Grid(Ice::CommunicatorPtr c) : c_(c) {}
  Grid() {
    c_ = Ice::initialize();
  }

  static Ice::CommunicatorPtr CreateCommunicator(const StringPair* arr) {
    Ice::PropertiesPtr prs = Ice::createProperties();
    for (const StringPair* i = arr; i->key; ++i) {
      prs->setProperty(i->key, i->value);
    }

    Ice::InitializationData id;
    id.properties = prs;
    return Ice::initialize(id);
  }
};

struct GridTest {
  static Grid& DefaultGrid() {
    static Grid grid_("ConfigGird", "10.22.202.47:4061");
    return grid_;
  }
};

struct GridFeed {
  static Grid DefaultGrid() {
    static Grid grid_("XceFeed/Locator:default -h XceFeedRegistry -p 14300:default -h XceFeedRegistryReplica1 -p 14301:default -h XceFeedRegistryReplica2 -p 14302");
    return grid_;
  }
};

struct GridMiniFeed {
  static Grid DefaultGrid() {
    static Grid grid_("XceFeed/Locator:default -h XceFeedRegistry -p 14300:default -h XceFeedRegistryReplica1 -p 14301:default -h XceFeedRegistryReplica2 -p 14302");
    return grid_;
  }
};

struct GridPassport {
  static Grid DefaultGrid() {
    static Grid grid_("Passport/Locator:default -h PassportMaster -p 4076:default -h PassportReplica1 -p 4076:default -h PassportReplica2 -p 4076");
    return grid_;
  }
};

struct GridCache {
  static Grid DefaultGrid() {
    static Grid grid_("CacheGrid/Locator:default -h 10.22.202.47 -p 12000");
    return grid_;
  }
};

struct GridOce { // 1 cluster
  static Grid DefaultGrid() {
    static Grid grid_("XiaoNei/Locator:default -h XiaoNeiRegistry -p 12000:default -h XiaoNeiRegistryReplica1 -p 12001:default -h XiaoNeiRegistryReplica2 -p 12002");
    return grid_;
  }
};

struct GridILikeIt { // 1 cluster
  static Grid DefaultGrid() {
    static Grid grid_("Like/Locator:tcp -h LikeMaster -p 4071:tcp -h LikeReplica1 -p 4071:tcp -h LikeReplica2 -p 4071");
    return grid_;
  }
};

struct GridUserBase {  // 100 cluster
  static Grid DefaultGrid() {
    static Grid grid_("User/Locator:default -h UserMaster -p 4075:default -h UserReplica1 -p 4075:default -h UserReplica2 -p 4075");
    return grid_;
  }
};

struct GridCore {  // 100 cluster
  static Grid DefaultGrid() {
    static Grid grid_("CoreCache/Locator:default -h CoreCacheMaster -p 4068:default -h CoreCacheReplica1 -p 4068:default -h CoreCacheReplica2 -p 4068");
    return grid_;
  }
};

template<typename ElementType, typename Cate, int Count>
struct UnlockedStaticArray {
  static ElementType Get(int index) {
    return values[index];
  }
  static ElementType Put(int index, ElementType ele) {
    return values[index] = ele;
  }

private:
  static ElementType values[Count];
};

template <typename ElementType, typename Cate, int Count>
ElementType UnlockedStaticArray<ElementType, Cate, Count>::values[Count];

// 多个副本模式,一定要加锁
template<typename ElementType, typename Cate>
struct LockedStaticArray {
  static std::map<size_t, std::vector<ElementType> > _values_;
  typedef std::map<size_t, std::vector<ElementType> > MapType;

  static std::vector<ElementType> Get(int index) {
    return _values_[index];
  }
  static ElementType GetOneByMod(size_t mod) {
    static size_t rand = 0;
    ElementType prx;
    _lock_.ReadLock();  // read lock
    typename MapType::iterator it = _values_.find(index);
    if (it != _values_.end()) {
      size_t n = rand++ % it->second.size();
      prx = (it->second)[n];
    }
    _lock_.ReadUnlock(); //read unlock
    return prx;
  }
  static size_t GetMod() {
    _lock_.ReadLock();  // read lock
    size_t mod = _values_.size();
    _lock_.ReadUnlock(); //read unlock
    return mod;
  }
  static ElementType GetOneByUid(size_t uid) { //
    static size_t rand = 0;
    ElementType prx;
    _lock_.ReadLock();  // read lock
    size_t c = _values_.size();
    if (c) {
      typename MapType::iterator it = _values_.find(uid%c);
      if (it != _values_.end() && !it->second.empty()) {
        size_t n = rand++ % it->second.size();
        prx = (it->second)[n];
      }
    }
    _lock_.ReadUnlock(); //read unlock
    return prx;
  }
  static void Put(int index, const std::vector<ElementType>& v) {
    _lock_.WriteLock(); // write lock
    _values_[index] = v;
    _lock_.WriteUnlock(); // write unlock
  }
  static void PutAll(MapType& map) {
    _lock_.WriteLock(); // write lock
    _values_.swap(map);
    _lock_.WriteUnlock(); // write unlock
  }
  static boost::mutex _mux_; // double lock use
  static xce::ReadWriteLock _lock_; // get proxy
  static bool _quit_;
};

template <typename ElementType, typename Cate>
 std::map<size_t, std::vector<ElementType> > LockedStaticArray<ElementType, Cate>::_values_;
template <typename ElementType, typename Cate>
  boost::mutex LockedStaticArray<ElementType, Cate>:: _mux_;
template <typename ElementType, typename Cate>
  xce::ReadWriteLock LockedStaticArray<ElementType, Cate>:: _lock_;
template <typename ElementType, typename Cate>
  bool LockedStaticArray<ElementType, Cate>:: _quit_ = false;

template<typename T>
struct AdapterBase {
  typedef T WrapType;

  // for Create
  template<typename Prx>
  static Prx Create(const Grid& grid, const char* endpoint) {
    Ice::ObjectPrx base = grid.c_->stringToProxy(endpoint);
    return Prx::checkedCast(base);
  }

  template<typename Prx>
  static Prx Create(const char* endpoint) {
    return Create<Prx>(WrapType::DefaultGrid(), endpoint);
  }

  template<typename Prx>
  static Prx Create(const char* endpoint, int mod) {
    std::ostringstream oss;
    oss << endpoint << mod;
    // LOG(LS_VERBOSE) << "Create " << oss.str() << " mod: " << mod;
    return Create<Prx>(WrapType::DefaultGrid(), oss.str().c_str());
  }

  // for cached ...
  template<typename Prx>
  static Prx Cached(const char* endpoint) {
    typedef UnlockedStaticArray<Prx, T, 1> HolderType;
    Prx ret = HolderType::Get(0);
    if (!ret) {
      ret = Create<Prx>(endpoint);
      HolderType::Put(0, ret);
    }
    return ret;
  }

  enum { MAX_CLUSTER = 200, };

  template<typename Prx>
  static Prx Cached(const char* endpoint, int mod, int total = MAX_CLUSTER) {
    ASSERT(total <= MAX_CLUSTER);

    typedef UnlockedStaticArray<Prx, T, MAX_CLUSTER> HolderType;
    Prx ret = HolderType::Get(mod);
    if (!ret) {
      ret = Create<Prx>(endpoint, mod);
      HolderType::Put(mod, ret);
    }
    return ret;
  }

  template<typename Prx>
  static bool ResetCached(const char* endpoint, int total = MAX_CLUSTER) {
    typedef UnlockedStaticArray<Prx, T, MAX_CLUSTER> HolderType;
    for (size_t i=0; i < total; ++i) {
      HolderType::Put(i, Create<Prx>(endpoint, i));
    }
    return true;
  }
};

template<typename T>
struct AdapterCore {
  typedef T WrapType;

  template<typename Prx>
  static void Renew(const xce::clusterstate::ServerStateMap& map) {
    typedef typename LockedStaticArray<Prx, T>::MapType MapType;
    MapType mt;
    for (xce::clusterstate::ServerStateMap::const_iterator it = map.begin(); it != map.end(); ++it) {
      size_t mod = it->first;
      std::vector<Prx> v;
      for (std::vector<xce::serverstate::ServerStateInfoPtr>::const_iterator sit = it->second.begin(); sit != it->second.end(); ++sit) {
        if ((*sit)->status == 1) // 需要无效列表吗
          v.push_back(Prx::uncheckedCast((*sit)->prx));
      }
      mt.insert(std::make_pair(mod,v));
      //LockedStaticArray<Prx, T>::Put(mod, v);
    }
    LockedStaticArray<Prx, T>::PutAll(mt);
  }

  template<typename Prx>
  static void Run(const std::string& topicname, const MyUtil::SbjSubscriberPrx& subscriber, const MyUtil::SubjectObserverPrx& proxy) {
    if (LockedStaticArray<Prx, T>::_quit_)
      return;

    try {
      proxy->subscribe(topicname, subscriber);
    } catch (Ice::Exception& e) {
      std::cout << "Subscribe is error:" << e.what() << std::endl;
      //return;
    } catch(...) {
      //return;
    }
    // 定期注册一次, 心跳,默认120ms
    xce::Post(boost::bind(Run<Prx>, "ClusterState", subscriber, proxy), 120);
  }

  template<typename Prx>
  class ClusterSubscripter: public ::MyUtil::SbjSubscriber {
   public:
    ClusterSubscripter() {
    }
    virtual ::MyUtil::SbjStatePtr opera(const ::MyUtil::SbjStatePtr& sbjstate, const ::Ice::Current& = ::Ice::Current()) {
      xce::clusterstate::ClusterStateInfoPtr csStatePtr = xce::clusterstate::ClusterStateInfoPtr::dynamicCast(sbjstate);
      Renew<Prx>(csStatePtr->states);
      return sbjstate;
    }
  };


  template<typename Prx>
  static void InitTarget(const std::string& endpoint) {
    srand((unsigned)time(NULL));
    std::string localIP = Network::local_ip().c_str();

    // 生成要注册subscriber的prx
    std::ostringstream ename;
    ename << "SubscriberRandom_" << rand();
    std::string adaptername=ename.str();
    ename << ".Endpoints";
    Ice::CommunicatorPtr ic = GridCore::DefaultGrid().c_;
    ic->getProperties()->setProperty(ename.str(),"tcp -h " + localIP);
    Ice::ObjectAdapterPtr adapter=ic->createObjectAdapter(adaptername);
    adapter->activate();
    MyUtil::SbjSubscriberPrx subscriber=MyUtil::SbjSubscriberPrx::uncheckedCast(adapter->addWithUUID(new ClusterSubscripter<Prx>()));
    //subscriber->ice_timeout(300);
    subscriber =MyUtil::SbjSubscriberPrx::uncheckedCast(subscriber->ice_timeout(300));
    

    // 找到Observer的prx
    Ice::CommunicatorPtr ic_oce = GridOce::DefaultGrid().c_;
    Ice::ObjectPrx object = ic_oce->stringToProxy(std::string("SO@") + endpoint)->ice_timeout(1000);
    MyUtil::SubjectObserverPrx proxy = MyUtil::SubjectObserverPrx::uncheckedCast(object);

    // 取一次服务器列表
    xce::clustercontroller::ControllerManagerPrx objprx =
        xce::clustercontroller::ControllerManagerPrx::checkedCast(ic_oce->stringToProxy(std::string("M@")+endpoint))->ice_timeout(1000);
    std::cerr << "ControllManagerPrx:" << objprx->ice_toString() << std::endl;

    xce::clusterstate::ServerStateMap clusterState;
    try {
      //clusterState = objprx->GetServerState()->states;
      xce::clusterstate::ClusterStateInfoPtr infoptr = objprx->GetServerState();
      clusterState = infoptr->states;
    } catch (Ice::Exception& e) {
      std::cout << "ice exception:" << e.what() << std::endl;
    } catch (std::exception& e) {
      std::cout << "c++ exception:" << e.what() << std::endl;
    } catch (...) {
      std::cout << "other exception:" << std::endl;
    }
    // debug
    for (xce::clusterstate::ServerStateMap::iterator it = clusterState.begin(); it != clusterState.end(); ++it) {
      int mod = it->first;
      std::vector<xce::serverstate::ServerStateInfoPtr> &v = it->second;
      std::cout << "modid:" << mod <<"------" << std::endl;
      for(std::vector<xce::serverstate::ServerStateInfoPtr>::iterator sit= v.begin(); sit!= v.end(); ++sit) {
        std::cout << "  server name:" << (*sit)->servername << " identity:" << (*sit)->identity << " prx:" << (*sit)->prx->ice_toString()
        << " cluster:" << (*sit)->cluster << " mod:" << (*sit)->mod << " status:" << (*sit)->status << " newset:" << (*sit)->newset << std::endl;
      }
    }
    Renew<Prx>(clusterState);
    xce::Post(boost::bind(Run<Prx>, "ClusterState", subscriber, proxy));
  }

  template<typename Prx>
  static Prx Cached(const char* endpoint, int uid) {
    // 双锁
    static bool first_run = true;
    if (first_run) {
      boost::mutex::scoped_lock lock(LockedStaticArray<Prx, T>::_mux_);
      if (first_run) {
        InitTarget<Prx>(endpoint);
        first_run = false;
      }
    }
    return LockedStaticArray<Prx, T>::GetOneByUid(uid);
  }

  template<typename Prx>
  static void Close() {
    LockedStaticArray<Prx, T>::_quit_ = true;
  }

  template<typename Prx>
  static bool ResetCached(const char* endpoint, int mod) {
    LockedStaticArray<Prx, T>::_quit_ = false;
    // for i from 0 to total
    //   HolderType::Put(mod, Create);
    return true;
  }
};

struct Adapter : public AdapterBase<Adapter> {
  typedef AdapterBase<GridTest> test;
  typedef AdapterBase<GridFeed> feed;
  typedef AdapterBase<GridMiniFeed> minifeed;
  typedef AdapterBase<GridCache> cache;

  typedef AdapterCore<GridOce> oce;
  typedef AdapterBase<GridOce> core;
  typedef AdapterBase<GridUserBase> userbase;
  typedef AdapterCore<GridUserBase> userbase2;
  typedef AdapterBase<GridPassport> passport;

  static Grid& DefaultGrid() {
    static Grid grid_;
    return grid_;
  }
};


} // namespace xce

#endif // XCE_BASE_ADAPTER2_H__
