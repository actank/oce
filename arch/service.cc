#include <list>
#include <boost/detail/atomic_count.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp> // TODO: use gflags maybe better
namespace po = boost::program_options;

#include "base/common.h"
#include "base/pcount.h"
#include "base/logging.h"
#include "base/once.h"
#include "base/logrotate.h"
#include "base/startuplist.h"
#include "base/asyncall.h"
#include "arch/logger.h"
#include "arch/service.h"


#ifdef POSIX
//#include <google/profiler.h>
#endif

#ifdef XCE_TEST
#include "test/samples.h"
#endif

std::string config_name_ = "no_name";


namespace xce {

namespace detail {

// for log only
std::string PrintStringSeq(const Ice::StringSeq& seq) {
  std::ostringstream ostem;
  for(Ice::StringSeq::const_iterator i=seq.begin(); i!=seq.end(); ++i)
    ostem << *i << " ";
  return ostem.str();
}

std::string PrintProperty(const Ice::PropertiesPtr& prop) {
  if (prop)
    return PrintStringSeq(prop->getCommandLineOptions());
  return "error PropertiesPtr";
}

class IceThreadHook : public virtual Ice::ThreadNotification {
public:
  void start() {
    ++ count_;
  }
  void stop() {
    -- count_;
  }
  size_t size() const {
    return count_;
  }

  IceThreadHook() : count_(0) {}
private:
  boost::detail::atomic_count count_;
};

} // detail

std::string Service::name_("noname");

// TODO: 实现一个 gcc's weakref
// std::string config_name_;

static std::list<ServantEntry>* servant_list_ = 0;
static boost::mutex servant_guard_;

bool Service::AddServant(const ServantEntry& se) {
  boost::mutex::scoped_lock lock(servant_guard_);
  if (!servant_list_) {
    servant_list_ = new std::list<ServantEntry>;
  }

  servant_list_->push_back(se);
  return true;
}

void Service::start(const ::std::string& name, const Ice::CommunicatorPtr& communicator
  , const ::Ice::StringSeq& args) {

  Ice::CommunicatorPtr cloned;
  Ice::PropertiesPtr props = communicator->getProperties();

  name_ = name;

  {
    // 1 
    // 用 Service 前缀,没有啥创意
    // Service.name.LogPath  ../log/name/oce_log
    // Service.name.LogLevel 
    std::string log_dir = props->getPropertyWithDefault(
      "Service." + name + ".LogPath"
      , "../log/" + name);


    int log_level = LS_INFO;
    {
      std::string level = props->getProperty("Service." + name + ".LogLevel");
      if (!level.empty()) {
        log_level = LogRotate::LogLevel(level);
      }
    }

    if (log_level != -1)
      LogRotate::instance().Start(log_dir, log_level);

    // TODO: 合并 args into Communicator?      
    // Communicator 没有setLogger接口，需要clone Communicator
    detail::IceThreadHook* hook = new detail::IceThreadHook;

    Ice::InitializationData init;
    init.logger = new IceLogger;
    init.threadHook = hook;
    init.properties = props;
    init.stats = communicator->getStats(); // ??? TODO: use
    cloned = Ice::initialize(init);

    XAR_POLL("icethr", boost::bind(&detail::IceThreadHook::size, hook));
  }

  LOG(INFO) << "service name: " << name;
  LOG(INFO) << "args: " << detail::PrintStringSeq(args);
  LOG(INFO) << "properties: " << detail::PrintProperty(communicator->getProperties());

  // ugly, be careful
  Adapter() = cloned->createObjectAdapter(name);

  Start(); // in base/common.h
  xar::start();

  // 2
  for (std::list<ServantEntry>::const_iterator elem=servant_list_->begin();
    elem != servant_list_->end(); ++elem) {
      ASSERT(elem->identity);
      try {
        Ice::ObjectPtr p = elem->creator();
        if (p) {
          LOG(INFO) << "create servant " << elem->identity << " sucesses.";
          Ice::Identity id = cloned->stringToIdentity(elem->identity);
          Adapter()->add(p, id);
        }
      } catch (std::exception* e) {
        LOG(LS_ERROR) << "create servant " << elem->identity << " failed " << e->what();
      }    
  };

  Adapter()->activate();

  // 开始异步执行StartupList
  detail::Async::Post(&RunStartupList);
}

void Service::stop() {
  LOG(INFO) << "stop called";

  // ObjectPtr 由引用计数机制释放
  Adapter()->deactivate();
  Adapter() = 0;
  LOG_F(INFO) << "1";

  for (std::list<ServantEntry>::const_iterator elem=servant_list_->begin();
    elem != servant_list_->end(); ++elem) {
    if (elem->destroyer)
      elem->destroyer();
  }
  LOG_F(INFO) << "2";

  // stop threadpool
  Stop(); // in base/common.h
  LOG_F(INFO) << "3";

  LOG(INFO) << "Good bye.";

  LogRotate::instance().Stop();

  DestoryStartupList();
}

/// CreateObjectAdapter 系列函数，没有保存Communicator 
/// 请使用 Ice::CommunicatorPtr ObjectAdapter::getCommunicator

Ice::ObjectAdapterPtr CreateObjectAdapter(int argc, char* argv[]) {
  Ice::CommunicatorPtr c;
  Ice::ObjectAdapterPtr result;

  try {
    c = Ice::initialize(argc, argv);
  } catch(std::exception& e) {
    LOG_F(LS_ERROR) << "Create Communicator error: " << e.what();
    return result;
  }

  // -n name -h host -p port
  // name:default -h 10.x.x.x -p 1000
  std::string host, name(config_name_);
  int port;
  {
    po::options_description desc("Allowed options");
    desc.add_options()
      ("name,n", po::value<std::string>(&name), "servant name")
      ("host,h", po::value<std::string>(&host), "ip address")
      ("port,p", po::value<int>(&port)->default_value(1000), "port")
      ("help,H", "display this help and exit")
      ;

    po::command_line_parser parser(argc, argv);
    po::parsed_options parsed = parser.options(desc).allow_unregistered().run();

    po::variables_map vm;
    po::store(parsed, vm);

    if (vm.count("help")) {
      //std::cout << desc << "\n";
      return result;
    }

    if (host.empty())
      host = Network::local_ip();

    po::notify(vm);
  }

  try {
    result = CreateObjectAdapter(c, name, host, port);
  } catch(std::exception& e) {
    LOG(LS_ERROR) << "CreateObjectAdapter " << name << " "
      << host << ":" << port
      << " error: " << e.what();
  }
  return result;
}

Ice::ObjectAdapterPtr CreateObjectAdapter(Ice::CommunicatorPtr c
    , const std::string& name, const std::string& host, int port) {
  ASSERT(!name.empty());
  std::ostringstream oss;
  oss << "default -h " << host << " -p " << port;

  Ice::ObjectAdapterPtr adapter = 
    c->createObjectAdapterWithEndpoints(name, oss.str());
  return adapter;
}

Ice::ObjectAdapterPtr CreateObjectAdapter(const std::string& name
    , const std::string& host, int port) {
  int argc = 0;
  char* argv[] = {0};
  Ice::CommunicatorPtr c = Ice::initialize(argc, argv);
  Ice::ObjectAdapterPtr result = CreateObjectAdapter(c, name, host, port);
  return result;
}

#ifdef POSIX
static void BoxSignalHandler(int sig, siginfo_t* sinfo, void* ucontext) {
  if (sig == SIGINT) {
    Service::Adapter()->getCommunicator()->shutdown();
  }    
}
#endif

void Usage() {
  std::puts("box "
    "Usage: box [-e=end points] [-p=Profile Filename]\n"
    "\n"
    "\nOptions:\n"
    " -e <end points>    like: tcp -h 10.22.202.47 -p 1025\n"
    " -p      use profiler\n"
    " -h, -?  show this help\n"
    );
}

int box_main(int argc, char* argv[]) {
  int status = 0;
  Ice::CommunicatorPtr ic;
  Ice::ObjectAdapterPtr adapter;

  // 缺省log至 ../log/box_shop.log
  std::string filename_log("../log/box_");
  filename_log += config_name_;
  filename_log += ".log";

  std::ofstream* stem = new std::ofstream(filename_log.c_str(), std::ios::binary);
  LogMessage::LogToStream(stem, LS_INFO);
#ifndef _DEBUG
  LogMessage::LogToDebug(100);
#endif
  
  try {
    ic = Ice::initialize(argc, argv);

    std::string endpoint;
    if (argc > 1)
      endpoint = argv[1];
    else {
      endpoint = "default -h ";
      endpoint += Network::local_ip();
      endpoint += " -p 10000";
    }
    std::cerr << endpoint << std::endl;
    adapter = ic->createObjectAdapterWithEndpoints(config_name_, endpoint);
    if (!adapter) {
      std::cerr << "object adapter " << config_name_ << " requires configuration";
      return 1;
    }
  } catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
    Ice::PropertiesPtr p = ic->getProperties();
    if (p)
      std::cerr << p->getProperty(config_name_ +".Endpoints") << std::endl;
    
    // TODO: LOG exception
    return 1;
  }

  Start(); // in common.h

#ifdef XCE_TEST
  bool f = test::Sample::LoadUserId("ids");
  if (!f)
    std::cerr << "ids load error, check it\n";

  f = xce::xar::start();
  if (!f)
    std::cerr << "xce::xar not running, check it\n";
#endif

#ifdef POSIX
  if (argc > 2) {
//    ProfilerStart(argv[2]);
  }

  // 可以让 box 优雅的结束，这样Profiler等能正常工作
  struct sigaction sa;
  sa.sa_sigaction = BoxSignalHandler;
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  int ret = sigaction(SIGINT, &sa, NULL);
  ASSERT(ret == 0);
#endif

#if 1
  Service::Adapter() = adapter;

  for (std::list<ServantEntry>::const_iterator elem=servant_list_->begin();
    elem != servant_list_->end(); ++elem) {
      ASSERT(elem->identity);
      try {
        Ice::ObjectPtr p = elem->creator();
        if (p) {
          LOG(INFO) << "create servant " << elem->identity << " sucesses.";
          Ice::Identity id = ic->stringToIdentity(elem->identity);
          adapter->add(p, id);
        }
      } catch (std::exception* e) {
        LOG(LS_ERROR) << "create servant " << elem->identity << " failed " << e->what();
      }
  }

  adapter->activate();
  ic->waitForShutdown();

#else

  {
    Service* s = new Service();
    Ice::StringSeq args;
    s->start(config_name_, ic, args);

    ic->waitForShutdown();

    s->stop();
    delete s;
  }
#endif

#ifdef POSIX
  if (argc > 2) {
 //   ProfilerStop();
  }
#endif

  Stop(); // in common.h

  if (ic) {
    try {
      ic->destroy();
    } catch (const Ice::Exception& e) {
      std::cerr << e << std::endl;
      status = 1;
    }
  }
  return status;
}

} // namespace xce

extern "C" {
ICE_DECLSPEC_EXPORT 
IceBox::Service* create(Ice::CommunicatorPtr communicator) {
  // TODO: 安装崩溃监控

#ifdef _MSC_VER
  __asm int 3;
#endif
  
  // 必须使用 new， TODO: IceBox::Service::stop 会 delete
  return new xce::Service();
}
} // extern "C"
