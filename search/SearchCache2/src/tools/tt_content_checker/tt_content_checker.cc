#include "tt_content_checker.h"

#include "MessageQueue.h"
#include "Consumer.h"
#include "Producer.h"

#define CONSUMER_NUM 7
FillTask::FillTask() { }
void FillTask::handle(int cycles, int begin_id, int valid_id_num) {
  while(1) {
      MCE_INFO("FillTask begin...");
      MessageQueue message_queue(valid_id_num);
      // 启动生产者线程
      Producer producer(message_queue, cycles, begin_id, valid_id_num);
      producer.start();

      //启动多个消费者线程
      vector<IceUtil::ThreadControl> consumers;
      for (int i=0; i<CONSUMER_NUM; ++i) {
        IceUtil::ThreadPtr t = new Consumer(message_queue);
        consumers.push_back(t->start());
      }

      for (vector<IceUtil::ThreadControl>::iterator iter=consumers.begin(); iter!=consumers.end(); ++iter) {
        iter->join();
      }
      MCE_INFO("FillTask done!");
  }
}

void Usage(char* cmd) {
  std::cout <<  cmd << " 功能:遍历数据库中所有的有效用户信息，与指定的TT中的内容比较(TT在配置文件中指明, 行如:tokyotryant://s44:1981)" << std::endl;
  std::cout << "用法: " << cmd << " <TT服务器的配置文件(完整路径)  已经查询数据库的次数(cycles, 用于程序中断后继续加载) 数据库查询的初始用户ID(begin_id) 已经往TT中加载的用户总数(valid_id_num)>" << std::endl;
}

std::map<int, boost::shared_ptr<xce::searchcache::IDbCache> > db_cache_map_;

int main(int argc, char ** argv)  
{
  MyUtil::init_logger("Mce", "tt_content_checker.log", "INFO");

  if (argc != 5) {
    Usage(argv[0]);
    return -1;
  }

  int cluster = 4;
  //int mod = 4;
  std::vector<std::string> dsn;
  //初始化sql连接池
  com::xiaonei::xce::ConnectionPoolManager::instance().initialize();
  xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().initialize();
  ifstream input(argv[1]);
  string temp;
  while(!input.eof())  {
    getline(input,temp,'\n');
    dsn.push_back(temp);
  }


  for (int i=0; i<cluster; ++i) {
    //获得需要初始化的数据源
    //数据源格式：SearchPersistentCache.InitDsn0=tokyotyrant://127.0.0.1:1981
    //有多少个cluster就有多少个数据源，数据源的其实id=0
    string strDSN = dsn.at(i);
    MCE_INFO("TT" << i << " : " << strDSN);
    db_cache_map_[i] = DatabaseCacheFactory::instance().getDbCache(strDSN);
    if (db_cache_map_[i]->Initialize() == false) {
      MCE_ERROR("Db" << i << " initialize failed");
      std::cerr<<"Db" << i << " initialize failed";
      exit(-1);
    }

  }

  int begin_id, cycles, valid_id_num;
  stringstream ss;
  ss << argv[2];
  ss >> cycles;
  ss.clear();

  ss << argv[3];
  ss >> begin_id;
  ss.clear();

  ss << argv[4];
  ss >> valid_id_num;
  //MCE_INFO("cycles=" << cycles << ",begin_id=" << begin_id << ", valid_id_num=" << valid_id_num);
  cout<<"cycles=" << cycles << ",begin_id=" << begin_id << ", valid_id_num=" << valid_id_num<<endl;

  FillTask task;
  task.handle(cycles, begin_id, valid_id_num);

  return 0;
}



