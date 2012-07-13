// #include "/data/home/yuwei.mu/trunk/release-newarch/thrift-gen/gen-cpp/SearchEngine.h" 
#include "gen-cpp/SearchEngine.h" 
#include "xoa2/client/client_pool.h"
#include "xoa2/client/xoa_client_factory.h"

long success, timeout, fail;

void * TestThread(void * data) {
  ClientPool<SearchEngineClient> * pool = (ClientPool<SearchEngineClient> *)data;

  SearchEngineClient * client = NULL;
  SearchResult res;
  while (true) {
    client = pool->Alloc(3);
    if (client) {
      MCE_DEBUG("Client allocation success.");
      for (int i = 0; i < 10; ++i) {
        if (i % 1 == 0) {
          usleep(10);
        }

        try {
          client->Search(res, "文档", 0, 5);
          ++success;
          MCE_DEBUG("Search() ok.");
        } catch (TTransportException& e) {
          MCE_WARN("Search() error : " << e.what());
          ++timeout;
        } catch (...) {
          MCE_WARN("Search() error.");
          ++fail;
        }

        if (success % 1 == 0) {
          std::cerr << "success " << success 
            << " timeout " << timeout
            << " failure " << fail 
            << std::endl;
        }
      }
      pool->Release(client);
    } else {
      MCE_WARN("NULL Client.");
    }
  }

  return NULL;
}

int main(int argc, char **argv) {
  MCE_INIT("./test.log", "DEBUG");
  // 监听zookeeper事件，须使用shared_ptr方式管理内存!
  boost::shared_ptr<ClientPool<SearchEngineClient> > pool(new ClientPool<SearchEngineClient>("localhost", "/xoa2/com/renren/xoa/search"));

  pool->InitConfig();
  int thread_count = 3;
  pthread_t * threads = new pthread_t[thread_count];

  for(int i = 0; i < thread_count; i++) {
     pthread_create(&threads[i], NULL, TestThread, (void *)pool.get());
  }

  for(int i = 0; i < thread_count; i++)
  {
    pthread_join(threads[i],NULL);
  }

  return 0;
}

