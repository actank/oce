#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/mutex.hpp>
#include <fstream>

#include "arch/withconfig.h"
#include "base/hashmap.h"
#include "base/logging.h"

using namespace xce;

// test
TEST_F(WithRemoteConfig, Query) {
  EXPECT_TRUE(CachedConfigPrx()->Query("key.not-found").empty());

  CachedConfigPrx()->Update("foo.test", "foo.test.value");
  EXPECT_EQ(CachedConfigPrx()->Query("foo.test"), "foo.test.value");

  EXPECT_TRUE(CachedConfigPrx()->Query("key.*").empty());
  EXPECT_TRUE(CachedConfigPrx()->Query("*key").empty());
  EXPECT_TRUE(CachedConfigPrx()->Query("*").empty());
}

TEST_F(WithRemoteConfig, BatchQuery) {
  CachedConfigPrx()->Update("a", "v");
  CachedConfigPrx()->Update("b", "v");
  CachedConfigPrx()->Update("b.2", "v");
  CachedConfigPrx()->Update("b3", "v");
  CachedConfigPrx()->Update("c", "v");

  ConfigContent ret = CachedConfigPrx()->BatchQuery("b");

  EXPECT_EQ(3, ret.size());
}

TEST_F(WithRemoteConfig, Update) {
  const std::string key("key.1");

  CachedConfigPrx()->Update(key, "1");
  EXPECT_EQ(CachedConfigPrx()->Query(key), "1");

  CachedConfigPrx()->Update(key, "2");
  EXPECT_EQ(CachedConfigPrx()->Query(key), "2");

  CachedConfigPrx()->Update(key, "3");
  EXPECT_EQ(CachedConfigPrx()->Query(key), "3");

  StringList keys, vals;
  keys.push_back("key.1");
  keys.push_back("key.2");
  keys.push_back("key.3");

  vals.push_back("1");
  vals.push_back("2");
  vals.push_back("3");
  CachedConfigPrx()->Updates(keys, vals);
  EXPECT_EQ(CachedConfigPrx()->Query(key), "1");
}

class TheListener : public ConfigListener {
public:
  virtual bool Update(const std::string& key, const std::string& val
    , const Ice::Current& = Ice::Current()) {

      boost::mutex::scoped_lock g(lock_);
      map_.insert(std::make_pair(key, val));

      return true;
  }

  TheListener(ConfigContent& map, boost::mutex& lock) 
    : map_(map), lock_(lock) {}

private:
  ConfigContent& map_;
  boost::mutex& lock_;
};

bool IsListContain(const StringList& sl, const std::string& text) {
  return sl.end() != std::find(sl.begin(), sl.end(), text);
}

// 测试通知
TEST_F(WithRemoteConfig, UpdateCheck) {
  ConfigContent to_check;
  boost::mutex to_lock;

  TheListener* p = new TheListener(to_check, to_lock);
  
  ConfigListenerPrx prx;
  Ice::Identity listener_id = AddServant(p, prx);

  CachedConfigPrx()->Subscribe("key*", prx);

  CachedConfigPrx()->Update("key", "foo");
  CachedConfigPrx()->Update("key1", "foo1");

  xce::Sleep(1000);

  {
    boost::mutex::scoped_lock g(to_lock);
    EXPECT_EQ(to_check["key"], "foo");
    EXPECT_EQ(to_check["key1"], "foo1");
    to_check.clear();
  }

  {
    StringList keys, vals;
    keys.push_back("key1");
    keys.push_back("key.2");
    keys.push_back("key.3");
    keys.push_back("key.foo.1");

    vals.push_back("1");
    vals.push_back("2");
    vals.push_back("3");
    vals.push_back("n1");
    CachedConfigPrx()->Updates(keys, vals);

    xce::Sleep(1000);

    boost::mutex::scoped_lock g(to_lock);
    EXPECT_EQ(to_check["key1"], "1");
    EXPECT_EQ(to_check["key.2"], "2");
    EXPECT_EQ(to_check["key.3"], "3");
    EXPECT_EQ(to_check["key.foo.1"], "n1");
  }

  CachedConfigPrx()->Unsubscribe("key*", prx);
  RemoveServant(listener_id);
  prx = 0;
}

TEST_F(WithRemoteConfig, LoadAndStore) {
  ConfigContent to_check;
  boost::mutex to_lock;

  TheListener* p = new TheListener(to_check, to_lock);
  
  ConfigListenerPrx prx;
  Ice::Identity listener_id = AddServant(p, prx);

  CachedConfigPrx()->Subscribe("key*", prx);

  CachedConfigPrx()->Update("foo", "bar");
  xce::Sleep(1000);

  // make it Store, then Load it?
#ifdef POSIX
  raise(SIGUSR1);
#endif

  StopConfigServer();
  EXPECT_FALSE(CachedConfigPrx());

  // TODO: use mock 测试确实析构了

  StartConfigServer();
  EXPECT_TRUE(CachedConfigPrx());
  EXPECT_EQ("bar", CachedConfigPrx()->Query("foo"));

  {
    boost::mutex::scoped_lock g(to_lock);
    to_check.clear();
  }

  CachedConfigPrx()->Update("key", "hello");
  xce::Sleep(1000);
  {
    boost::mutex::scoped_lock g(to_lock);
    EXPECT_EQ(to_check["key"], "hello");
  }

  StopConfigServer();

  system("rm /data/cache/configserver/*.txt -f");
}

TEST(ConfigServerI, Persist) {
#if 0
  // 生成Persist文件，在ConfigServer创建时会自动加载该文件
  {
    std::ofstream os(ConfigServerI::ContentFileName().c_str(), std::ios::trunc);
    os << "testkey*cache@s1:100;cache@s2:200;cache@s3:200;cache@s4:300;cache@s5:200;" << std::endl;
    os << "testkey-cache@s1:100;cache@s2:200;cache@s3:200;cache@s4:300;cache@s5:200;" << std::endl;
    os << std::endl;
    os << "testkey-***" << std::endl;

    std::ofstream os2(ConfigServerI::ListenerFileName().c_str(), std::ios::trunc);
    os2 << "testkey" << std::endl;
    os2 << "4BAE5CEE-6F56-4712-BC24-DC476542577D -t:tcp -h 10.2.17.61 -p 10000" << std::endl;
    os2 << std::endl;
    os2 << "t*" << std::endl;
    os2 << "4BAE5CEE-6F56-4712-BC24-DC476542577D -t:tcp -h 10.2.17.61 -p 10000" << std::endl;
    os2 << std::endl;
  }

  xce::Start();
  ConfigServerI * cs = new ConfigServerI;
  EXPECT_TRUE(cs->Query("testkey") == "cache@s1:100;cache@s2:200;cache@s3:200;cache@s4:300;cache@s5:200;");
  xce::Stop();

  delete cs;

  {
    std::ofstream os("/data/cache/configserver/config_content.txt", std::ios::trunc);
    std::ofstream os2("/data/cache/configserver/config_listen_prx.txt", std::ios::trunc);
  }
#endif
}

class ConfigServerPersistTest : public testing::Test {
protected:
  virtual void SetUp() {
    xce::Start();
    CreateConfigServerAdapter();
  }

  virtual void TearDown() {
    xce::Stop();
    DestroyConfigServerAdapter();
  }

  void DestroyConfigServerAdapter()
  {
    xce::Service::Adapter() = 0;
    adapter_->deactivate();
    adapter_->waitForDeactivate();
    adapter_->destroy();
    adapter_ = 0;
  }

  void CreateConfigServerAdapter()
  {
    adapter_ = xce::CreateObjectAdapter("configserver-persist", xce::Network::local_ip(), 10242);
    ASSERT(adapter_);

    xce::ConfigServerI *cs = new xce::ConfigServerI();
    configserver_prx_ = ConfigServerPrx::checkedCast(adapter_->addWithUUID(cs));
    ASSERT(configserver_prx_);

    // 全局的....
    xce::Service::Adapter() = adapter_;
    adapter_->activate();
  }

  Ice::ObjectAdapterPtr adapter_;
  ConfigServerPrx configserver_prx_;
};

#if 0
TEST_F(ConfigServerPersistTest, Persist) {
#if 0
  Ice::CommunicatorPtr ic = Ice::initialize();

  std::string endpoint  = "default -h ";
  endpoint += xce::Network::local_ip();
  endpoint += " -p 10243";
  Ice::ObjectAdapterPtr listen_adapter = ic->createObjectAdapterWithEndpoints("configlistener-test", endpoint);
#else
  Ice::ObjectAdapterPtr listen_adapter = xce::CreateObjectAdapter("configlistener-test", xce::Network::local_ip(), 10243);
#endif

  EXPECT_TRUE(listen_adapter);

  StringList keys, vals; // single thread
  TheListener* l1 = new TheListener(keys, vals);
  TheListener* l2 = new TheListener(keys, vals);

  Ice::ObjectPrx prx1 = listen_adapter->addWithUUID(l1);
  Ice::ObjectPrx prx2 = listen_adapter->addWithUUID(l2);

  listen_adapter->activate();

  std::cout << "listener prx 1 : " << prx1 << std::endl;
  std::cout << "listener prx 2 : " << prx2 << std::endl;

  configserver_prx_->Subscribe("key*", ConfigListenerPrx::uncheckedCast(prx1));
  configserver_prx_->Subscribe("key*", ConfigListenerPrx::uncheckedCast(prx2));

  configserver_prx_->Update("key1", "1-0");
  configserver_prx_->Update("key2", "2-0");
  configserver_prx_->Update("key3", "3-0");
  //EXPECT_TRUE(cs->Query("testkey") == "cache@s1:100;cache@s2:200;cache@s3:200;cache@s4:300;cache@s5:200;");

  xce::Sleep(2000);

  EXPECT_EQ((*l1)["key1"], "1-0");

  DestroyConfigServerAdapter();

  // 重新创建servant adapter, 测试从文件加载的proxy 是否仍有效
  CreateConfigServerAdapter();

  configserver_prx_->Update("key1", "1-1");
  configserver_prx_->Update("key2", "2-1");
  configserver_prx_->Update("key3", "3-1");

  xce::Sleep(2000);

  EXPECT_EQ((*l1)["key1"], "1-1");
  EXPECT_EQ((*l2)["key1"], "1-1");

  EXPECT_EQ((*l1)["key2"], "2-1");
  EXPECT_EQ((*l2)["key2"], "2-1");

  EXPECT_EQ((*l1)["key3"], "3-1");
  EXPECT_EQ((*l2)["key3"], "3-1");

  listen_adapter->deactivate();
  listen_adapter->destroy();
}
#endif

int main(int argc, char* argv[]) {
  xce::LogMessage::LogToDebug(xce::WARNING);
  testing::InitGoogleTest(&argc,argv);
  int r = RUN_ALL_TESTS();
  return r;
}
