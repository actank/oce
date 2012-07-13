/*
  作用：推荐热门公共主页
  作者：熊君武
  时间：2011-11-28
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <assert.h>
#include "socialgraph/recommendpage/util/pageitem.h"
#include "socialgraph/recommendpage/util/pagedbhelper.h"
#include "item.pb.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/timecost.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "socialgraph/socialgraphutil/dbutil.h"
#include "DbCxxPool/src/QueryRunner.h"
/*
HOTPAGE推荐cache加载
*/

namespace xce {
namespace socialgraph {

//typedef std::vector<int> IntVec;
//typedef std::set<int> IntSet;

struct HotPage {
  HotPage() {
    page_id = 0;
    fans_counter = 0;
  } 
   
  int page_id;
  int fans_counter;
}; 
typedef std::vector<HotPage> HotPageVec;

bool FansCmp(const HotPage& h1, const HotPage& h2)
{
  return h1.fans_counter > h2.fans_counter;
}

void getUserIds(std::ifstream* file_in, IntVec& user_ids_vec)
{
  CLOG(INFO) << "查询昨天日登数据库文件, 获取昨天日登用户列表";
  RunTime time_getuids = RunTime::create("getUserIds");

  std::string line;
  int user_id = 0;
  std::getline(*file_in, line);
  while(!line.empty()) {
    user_id = atoi(line.c_str());
    if(0 != user_id) {
      user_ids_vec.push_back(user_id);
      //CLOG(INFO) << "user_id: " << user_id;
    }
    line.clear();
    std::getline(*file_in, line);
  }
  CLOG(INFO) << "users' login size: " << user_ids_vec.size(); 
}

void getPageIds(std::ifstream* file_in, HotPageVec& hotpage_vec)
{
  CLOG(INFO) << "加载全站推荐page id列表";
  RunTime time_getpids = RunTime::create("getPageIds");

  std::string line;
  int page_id = 0; 
  std::getline(*file_in, line);
  while(!line.empty()) {
    page_id = atoi(line.c_str());
    if(0 != page_id) {
      Page page_item;
      page_item.page_id = page_id;
      CLOG(INFO) << "page_id: " << page_item.page_id;
      //获取全站推荐page的page_id, fans_counter
      PageDbHelper::getPageInforFromDb(page_item);
      //CLOG(INFO) << "page_id: " << page_item.page_id << "  fans_counter " << page_item.fans_counter;
      HotPage hotpage_item; 
      hotpage_item.page_id = page_item.page_id;
      hotpage_item.fans_counter = page_item.fans_counter;
      hotpage_vec.push_back(hotpage_item);
    }
    line.clear();
    std::getline(*file_in, line);
  }
  CLOG(INFO) << "rcd page size: " << hotpage_vec.size(); 
}

void setRcdHotPage(int user_id, const HotPageVec& hotpage_vec)
{
  //CLOG(INFO) << "构造该用户的hotpage列表，并载入cache";
  RunTime time_getrcdhp = RunTime::create("getRcdHotPage");

  if((0 != user_id) && !hotpage_vec.empty()) {
    //将需要过滤的page列表放到block_set中
    IntSet block_set;

    //CLOG(INFO) << "获取已经推荐给该用户的page ID列表" ;
    //获取每个用户推荐page列表: 访问socialgraph_recommend_page数据源recommend_by_page表
    //CLOG(INFO) << "get the user's all pages' page_id and page_value";
    PageVec rcd_page_vec;
    const static unsigned int kRcdPageNum = 50;   //upbound of recommended pages number to the user 
    rcd_page_vec.reserve(kRcdPageNum);
    PageDbHelper::getRcdPageInforFromDb(user_id, rcd_page_vec);
    //将已经推荐给该用户的page列表加入到过滤表过滤
    if(!rcd_page_vec.empty()) {
      for(PageVec::iterator it = rcd_page_vec.begin(); it != rcd_page_vec.end(); ++it) {
        block_set.insert((*it).page_id);
      }
    }
    //CLOG(INFO) << "block page size: " << block_set.size();
   
    //CLOG(INFO) << "获取该用户的sg_block PAGE列表";
    //IntVec sg_block_page_vec = xce::socialgraph::DbUtil::GetInstance().GetSgBlockListByBiz(user_id, "PAGE");
    IntVec sg_block_page_vec = DbUtil::GetInstance().GetSgBlockListByBiz(user_id, "PAGE");
    if(!sg_block_page_vec.empty()) {
      block_set.insert(sg_block_page_vec.begin(), sg_block_page_vec.end());
    } 
    //CLOG(INFO) << "block page size: " << block_set.size();
   
    //CLOG(INFO) << "对该用户过滤已经推荐的page列表和sg_block PAGE列表";
    HotPageVec rcd_hotpage_vec;
    for(HotPageVec::const_iterator it = hotpage_vec.begin(); it != hotpage_vec.end(); ++it) {
      IntSet::iterator result = block_set.find( (*it).page_id);  
      if(result == block_set.end()) {
        HotPage hotpage_item;
        hotpage_item.page_id = (*it).page_id;
        hotpage_item.fans_counter = (*it).fans_counter;
        rcd_hotpage_vec.push_back( hotpage_item );
      }
    } 
    //CLOG(INFO) << "filtered hot page size: " << rcd_hotpage_vec.size();
   
    //选取最热门，即粉丝数最多的一定数量的公共主页
    const static unsigned int kRcdHotPageNum = 30;  //给每个用户cache默认加载的Hot Page的总数
    //如果实际提供的推荐热门公共主页总数小于需要展示的最大热门公共主页数，则只对提供实际数进行排序，防止比较函数出现段错误
    unsigned int kmax_num = kRcdHotPageNum < rcd_hotpage_vec.size() ? kRcdHotPageNum :  rcd_hotpage_vec.size();
    std::partial_sort(rcd_hotpage_vec.begin(), rcd_hotpage_vec.begin() + kmax_num, rcd_hotpage_vec.end(), FansCmp);
    //CLOG(INFO) << "user_id: " << user_id << " kmax_num: " << kmax_num;
     
    //CLOG(INFO) << "将推荐的hot page列表组装成protobuf序列化的数据结构";
    Items items;
    for(HotPageVec::iterator it = rcd_hotpage_vec.begin(); it < rcd_hotpage_vec.begin()+kmax_num; ++it) {
      Item* item = items.add_items();
      item->set_id( (*it).page_id );
      item->set_field( (*it).fans_counter );
    }
   
    //将组装后的数据结构利用protobuf进行序列化
    std::vector<unsigned char> bin_data;
    int byte_size = items.ByteSize();
    //CLOG(INFO) << "user_id: " << user_id << " protobuf set size: " << byte_size;
    bin_data.resize(byte_size);
    items.SerializeToArray(bin_data.data(), byte_size); 

    //CLOG(INFO) << "将序列化后的热门公共主页推荐列表加载到缓存";
    const static std::string kBiz = "HOTPAGE";
    BusinessCacheAdapter::GetInstance().Set(kBiz, user_id, bin_data); 
     
    /* 
    std::vector<unsigned char> test_data; 
    BusinessCacheAdapter::GetInstance().Get(kBiz, user_id, test_data);
    CLOG(INFO) << "user_id: " << user_id << " memcache get size: " << test_data.size();
    Items test_items;
    test_items.ParseFromArray(test_data.data(), test_data.size());
    ::google::protobuf::RepeatedPtrField<Item>* seq = test_items.mutable_items();
    for(int index=0; index < seq->size(); ++index) {
      Item* item = test_items.mutable_items(index);
      CLOG(INFO) << "page_id: " << item->id() << "fans_counter: " << item->field(); 
    }
    */
  }
}

}
}

int main(int argc, char** argv)
{
  using namespace com::xiaonei::xce;
  using namespace xce::socialgraph;
  using namespace MyUtil;
  RunTime main_cost = RunTime::create("HOTPAGE");
  ConnectionPoolManager::instance().initialize();
  InitLogger("rcdhotpage");
  CLOG(INFO) << "SocialGraph 热门公共主页推荐!";
  if(argc != 3) {
    CLOG(ERROR) << "Usage: " << argv[0] << " login_ids.201x-xx-xx pageids.txt";   
    exit(1);
  }
  
  CLOG(INFO) << "Load users' login ids file";
  std::ifstream login_file(argv[1]);
  if(!login_file) {
    CLOG(ERROR) << "Can not open users' login ids file";
    exit(1);
  }  
  IntVec user_ids_vec;
  const static int kmax_daylogin_num = 15000000; 
  user_ids_vec.reserve(kmax_daylogin_num);
  getUserIds(&login_file, user_ids_vec);
  
  CLOG(INFO) << "Load users' recommended page ids file";
  std::ifstream pageids_file(argv[2]);
  if(!pageids_file) {
    CLOG(ERROR) << "Can not open page ids files";
    exit(1);
  }
  HotPageVec hotpage_vec;
  const static int kmax_hotpage_num = 1000; 
  hotpage_vec.reserve(kmax_hotpage_num);
  getPageIds(&pageids_file, hotpage_vec);

  CLOG(INFO) << "Set each user's rcd hotpage's (page,fans_counter) pair sequence";
  int user_count = 0;
  for(IntVec::iterator it = user_ids_vec.begin(); it != user_ids_vec.end(); ++it) {
    setRcdHotPage( *it, hotpage_vec);
    CLOG(INFO) << "rcdhotpage user count: " << ++user_count;
  }

 //assert(false);
  return 0;
}

