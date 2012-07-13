#include "arch/configserverI.h"

#include <fstream>
#include <boost/thread/mutex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "base/logging.h"
#include "base/threadpool.h"
#include "base/common.h"
#include "base/mkdirs.h"
#include "base/asyncall.h"
#include "arch/service.h"


namespace xce {

static const char* kDUMP_DIRECTORY = "/data/cache/configserver";
static const char* kCONTENT_FILENAME = "content.txt";
static const char* kLISTENER_FILENAME = "proxy.txt";

ConfigServerI::ConfigServerI()
  : content_changed_(false)
  , listener_changed_(false)
  , quit_(true)
  , validating_(false) {

  Init();
}

ConfigServerI::~ConfigServerI() {
  // Store everything?

  pool_->Stop();
  pool_->Join();
  delete pool_;

  quit_ = true;
  // TODO: join
}

typedef std::pair<std::string, std::string> StringPair;
typedef std::list<std::pair<std::string, std::string> > StringPairList;

bool Load(const std::string& filename, StringPairList* content) {
  ASSERT(!filename.empty());
  ASSERT(content);

  std::ifstream istem(filename.c_str());
  if (!istem)
    return false;

  std::string text;
  while(getline(istem, text)) {
    // split by FIRST =
    std::string::size_type pos_equal = text.find_first_of('=');
    if (pos_equal != std::string::npos) {
      content->push_back(std::make_pair(
        text.substr(0, pos_equal)
        , text.substr(pos_equal + 1)
        ));
    }
  }
  return true;
}

bool Store(const std::string& filename, const StringPairList* content) {
  std::ofstream ostem(filename.c_str(), std::ios::trunc);
  if (!ostem)
    return false;

  BOOST_FOREACH(const StringPair& i, *content) {
    ostem << i.first << '=' << i.second << "\n";
  }
  return true;
}

static ConfigServerI* g_instance = 0;

#ifdef POSIX
static void SignalHandler(int sig, siginfo_t*, void*) {
  ASSERT(g_instance);
  if (sig == SIGUSR1) {
    g_instance->StoreListener();
    g_instance->StoreContent();
  } else if (sig == SIGUSR2) {
    g_instance->LoadListener();
    g_instance->LoadContent();
  }
}
#endif

void ConfigServerI::Init() {
  // 0 thread pool
  // 1 从文件读取读取Content/Listener信息
  // 2 设定定时任务
  // 3 install signal handler

  pool_ = new ThreadPool(2, 10);

  // 1
  LoadContent();
  LoadListener();

  // 2
  // Post(boost::bind(&ConfigServerI::StoreConfigMap, this), 1000, 0);
  Post(boost::bind(&ConfigServerI::ValidateProc, this), 1000 * 60 * 60, 0);

  // 3
  g_instance = this;
#ifdef POSIX
  struct sigaction sa;
  sa.sa_sigaction = SignalHandler;
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  int ret = sigaction(SIGUSR1, &sa, NULL);
  ASSERT(ret == 0);
  ret = sigaction(SIGUSR2, &sa, NULL);
  ASSERT(ret == 0);
#endif
}

std::string ConfigServerI::content_file_path() const {
  std::string f(kDUMP_DIRECTORY);
  f += '/';
  f += kCONTENT_FILENAME;
  return f;
}

std::string ConfigServerI::listener_file_path() const {
  std::string f(kDUMP_DIRECTORY);
  f += '/';
  f += kLISTENER_FILENAME;
  return f;
}

void ConfigServerI::LoadListener() {
  StringPairList content;
  if (!Load(listener_file_path().c_str(), &content))
    return;

  if (content.empty())
    return;

  Ice::CommunicatorPtr cm;
  if (Service::Adapter())
    cm = Service::Adapter()->getCommunicator();
  else
    cm = Ice::initialize();

  ASSERT(cm);
  if (!cm)
    return;

  boost::mutex::scoped_lock lock(listener_mutex_);
  listener_map_.clear(); // is right?

  BOOST_FOREACH(const StringPair& i, content) {
    // TODO: IsValidEndpoint(i.second) ?
    try {
      Ice::ObjectPrx base = cm->stringToProxy(i.second);
      ConfigListenerPrx prx = ConfigListenerPrx::uncheckedCast(base);

      listener_map_[i.first].insert(prx);
    } catch(Ice::Exception& e) {
      LOG(WARNING) << "endpoint " << i.second << " cast error: " << e;
    }
  }
}

void ConfigServerI::StoreListener() {
  StringPairList content;
  {
    boost::mutex::scoped_lock lock(listener_mutex_);
    BOOST_FOREACH(ListenerMap::value_type& v, listener_map_) {
      BOOST_FOREACH(const ConfigListenerPrx& prx, v.second) {
        content.push_back(std::make_pair(
          v.first, prx->ice_toString()
          ));
      }
    }
  }

  if (!content.empty()) {
#ifdef POSIX
    xce::mkdirs(kDUMP_DIRECTORY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    bool f = Store(listener_file_path(), &content);
    ASSERT(f);
  }
}

void ConfigServerI::LoadContent() {
  StringPairList content;
  if (Load(content_file_path(), &content)) {
    if (content.empty())
      return;
    boost::mutex::scoped_lock lock(content_mutex_);
    content_map_.clear();
    BOOST_FOREACH(const StringPair& i, content) {
      content_map_.insert(std::make_pair(
          i.first, i.second
          ));
    }
  }
}

void ConfigServerI::StoreContent() {
  StringPairList content;
  {
    boost::mutex::scoped_lock lock(content_mutex_);
    BOOST_FOREACH(const ContentMap::value_type& i, content_map_) {
      content.push_back(std::make_pair(
          i.first, i.second
	  ));
    }
  }
  if (!content.empty()) {
#ifdef POSIX
    xce::mkdirs(kDUMP_DIRECTORY, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    Store(content_file_path(), &content);
  }
}

// 
void ConfigServerI::Subscribe(const ::std::string& key, const ConfigListenerPrx& prx, const Ice::Current&) {
  ASSERT(!key.empty());
  if (key.empty())
    return;

  LOG(VERBOSE) << "Subscribe " << key << " by " << prx->ice_toString();
  boost::mutex::scoped_lock lock(listener_mutex_);
  listener_map_[key].insert(prx);

  listener_changed_ = true;
}

void ConfigServerI::Unsubscribe(const ::std::string& key, const ConfigListenerPrx& prx, const Ice::Current&) {
  ASSERT(!key.empty());
  if (key.empty())
    return;

  boost::mutex::scoped_lock lock(listener_mutex_);
  ListenerMap::iterator i = listener_map_.find(key);
  ASSERT(i != listener_map_.end());
  if (i != listener_map_.end()) {
    i->second.erase(prx);
  }
  listener_changed_ = true;
}

std::string ConfigServerI::Query(const std::string& key, const Ice::Current&) const {
  ASSERT(!key.empty());
  if (key.empty())
    return std::string();

  boost::mutex::scoped_lock lock(content_mutex_);  
  ContentMap::const_iterator i = content_map_.find(key);
  if (i != content_map_.end())
    return i->second;
    
  return std::string();
}

ConfigContent ConfigServerI::BatchQuery(const std::string& key, const Ice::Current&) const {
  ConfigContent ret;
  ASSERT(!key.empty());
  if (key.empty())
    return ret;

  std::string match_key(key);
  std::string::size_type star = match_key.find_first_of('*');
  if (star != std::string::npos)
    match_key.resize(star);

  boost::mutex::scoped_lock lock(content_mutex_);  

  ContentMap::const_iterator begin = content_map_.lower_bound(match_key);
  match_key += (char)'z'+1;
  ContentMap::const_iterator end = content_map_.upper_bound(match_key);

  for (ContentMap::const_iterator i=begin; i!=end; ++i)
    ret.insert(std::make_pair(i->first, i->second));

  return ret;
}

class NotifyToListener : public Message {
public:
  virtual void Run() {
    bool has_error = false;
    try {
      prx_->Update(key_, val_);
    } catch(Ice::NoEndpointException& e) {
      LOG(INFO) << prx_->ice_toString() << " " << e;
      has_error = true;
    }
    catch(Ice::Exception& e) {
      LOG(INFO) << prx_->ice_toString() << " " << e;
      has_error = true;
    }

    if (has_error) {
      ASSERT(false);
      // 如何重试呢?
      // 搞第二级ThreadPool，慢慢执行?
      // use Post?
    }
  }

  NotifyToListener(ConfigListenerPrx prx, const std::string& key, const std::string& val) 
    : key_(key), val_(val), prx_(prx)  {}
  
  std::string key_, val_;
  ConfigListenerPrx prx_;
};

void ConfigServerI::Update(const std::string& key, const std::string& value, const Ice::Current&) {
  ASSERT(!key.empty());
  if (key.empty() || key.find('*') != std::string::npos)
    return;

  LOG(INFO) << "Update " << key << " to " << value;
  
  {
    boost::mutex::scoped_lock lock(content_mutex_);
    content_map_[key] = value;
    content_changed_ = true;
  }

  {
    boost::mutex::scoped_lock lock(listener_mutex_);
    for(ListenerMap::const_iterator i=listener_map_.begin(); i!=listener_map_.end(); ++ i) {
      std::string the_key = i->first;
      // 去掉最后的 *
      std::string::size_type star = the_key.find_first_of('*');
      if (star != std::string::npos)
        the_key.resize(star);

      // 只要匹配部分即可
      if (boost::starts_with(key, the_key)) {
        for (ListenerSet::const_iterator j= i->second.begin(); j!=i->second.end(); ++j)
          pool_->Post(new NotifyToListener(*j, key, value));
      }
    }
  }
  
#if 0
  {
    // TODO: 应该异步执行
    boost::mutex::scoped_lock lock(listener_mutex_);
    for(ListenerMap::iterator mi = listener_map_.begin(); mi != listener_map_.end(); ++ mi)  
    {
      if (mi->first == key || (mi->first[mi->first.size() - 1] == '*' 
        && boost::starts_with(key, mi->first.substr(0, mi->first.size() - 1))))
      { 
        LOG(VERBOSE) << "Listeners of " << key << " notified.";
        ListenerSet & l = mi->second;
        for (ListenerSet::iterator i = l.begin(); i != l.end(); ++i)
        {
          xce::Post(boost::bind(&ConfigServerI::UpdateTask, this, *i, key, value));
        }
      }
    }
  }
#endif
}

void ConfigServerI::Updates(const StringList& keys, const StringList& values, const Ice::Current&) {
  if (keys.empty() || keys.size() != values.size())
    return;
  for(size_t i = 0; i < keys.size(); ++i) {
    Update(keys[i], values[i]);
  }
}

void ConfigServerI::ValidateProc() {
  if(quit_)
    return;
  if(validating_)
    return;
  validating_ = true;

  ListenerSet error_listeners;
  {
    boost::mutex::scoped_lock lock(listener_mutex_); // 一把大锁
    for(ListenerMap::iterator i = listener_map_.begin(); i != listener_map_.end(); ++i)
    {
      ListenerSet & ls = i->second;        
      for(ListenerSet::iterator j = ls.begin(); j!= ls.end();)
      {
        if (quit_)
          return;

        try
        {
          (*j)->ice_ping();  // 判断 proxy 是否有效
        }
        catch(...)// const Ice::Exception&)
        {
          error_listeners.insert(*j);
          if (suspected_listeners_.find(*j) != suspected_listeners_.end())
          {
            LOG(LS_INFO) << "listerner proxy " << (*j)->ice_toString() << " removed.";
            ls.erase(j++);
            continue;
          }
        }
        LOG(LS_VERBOSE) << "proxy " << (*j)->ice_toString() << " OK.";
        ++j;
      }
    }
  }
    
  // 确定出错的listener是否需要删除.
  for(ListenerSet::iterator e = error_listeners.begin(); e != error_listeners.end(); ++e)
  {
    if (suspected_listeners_.find(*e) == suspected_listeners_.end())
    {
      suspected_listeners_.insert(*e);
    }
    else
    {
      suspected_listeners_.erase(*e);
    } 
  }

  validating_ = false;
}

}
//GLOBAL_INIT(congit111, {config_name_ = "config";})
BEGIN_SERVANT_MAP("config")
  SERVANT_ENTRY(config, &xce::ServantFactory<xce::ConfigServerI>::CreateServant
      , &xce::ServantFactory<xce::ConfigServerI>::Destroy)
  SERVANT_ENTRY(listener, &xce::ServantFactory<xce::ConfigServerI>::CreateConfigListener, 0)
END_SERVANT_MAP()
