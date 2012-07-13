#include "socialgraph/recommendpage/util/pagecachehelper.h"

using namespace xce::socialgraph;

/*
Cache storage structure:

------------------------------------------------------------------------------------------------------------
user_id | page_id1 | fans_counter1 | common_friend_ids1 | ... | page_idN | fans_counteN | common_friend_idsN
------------------------------------------------------------------------------------------------------------

*/

void PageCacheHelper::setRcdPageInforToCache(int user_id, const PageVec& page_items) 
{
  //CLOG(INFO) << "************** build page rcd cache data: store user's recommended page infomation to cache *********************";
  //CLOG(INFO) << "Fetch page block list from socialgraph_block data instance";
  IntSet block_set;
  IntVec sg_block_page_vec = DbUtil::GetInstance().GetSgBlockListByBiz(user_id, "PAGE");
  if(!sg_block_page_vec.empty()) {
    block_set.insert(sg_block_page_vec.begin(), sg_block_page_vec.end());
  }

  //CLOG(INFO) << "Build filtered page list data structure to be serialize by protobuf";
  //PageCacheVec pagecache_items;
  //pagecache_items.reserve(kRcdPageNum);
  Items items;
  if(!page_items.empty()) {
    for(PageVec::const_iterator it = page_items.begin(); it != page_items.end(); ++it) {
      IntSet::iterator result = block_set.find( (*it).page_id );
      if(result == block_set.end()) {
           Item* item = items.add_items();
           item->set_id( (*it).page_id );
           item->set_field( (*it).fans_counter );
           //PageCache pagecache_item;
           //pagecache_item.page_id = (*it).page_id;
           //pagecache_item.fans_counter = (*it).fans_counter;
           //pagecache_items.push_back( pagecache_item );
           for(IntVec::const_iterator idx=(*it).common_friend_ids.begin(); idx != (*it).common_friend_ids.end(); ++idx) {
             //CLOG(INFO) << "disp comm frd id: " << *idx << std::endl; 
	     item->add_fields(*idx);
           }
      }
    }
  }

  //for(PageCacheVec::const_iterator it = pagecache_items.begin(); it != pagecache_items.end(); ++it) {
  //  Item* item = items.add_items();
  //  item->set_id( (*it).page_id );
  //  item->set_field( (*it).fans_counter );
  //}    
  //Seiralize the data structure built by protobuf
  std::vector<unsigned char> bin_data;
  int byte_size = items.ByteSize();
  //CLOG(INFO) << "user_id: " << user_id << " protobuf set size: " << byte_size;
  bin_data.resize(byte_size);
  items.SerializeToArray(bin_data.data(), byte_size);

  //CLOG(INFO) << "store user's recommended page infomation to cache";
  const static std::string kBiz = "PAGE";
  BusinessCacheAdapter::GetInstance().Set(kBiz, user_id, bin_data);

  //CLOG(INFO) << "test";
  //std::vector<unsigned char> test_data;
  //BusinessCacheAdapter::GetInstance().Get(kBiz, user_id, test_data);
  ////CLOG(INFO) << "user_id: " << user_id << " protobuf get size: " << test_data.size();
  //Items test_items;
  //test_items.ParseFromArray(test_data.data(), test_data.size());
  //::google::protobuf::RepeatedPtrField<Item>* seq = test_items.mutable_items();
  //for(int i=0; i < seq->size(); ++i) {
  //  Item* item = test_items.mutable_items( i);
  //  CLOG(INFO) << "page_id: " << item->id() << " fans_counter: " << item->field() << " common_friend_ids: "; 
  //  CLOG(INFO) << "display comm frds size: " << item->fields_size() << std::endl;
  //  for(unsigned int j=0; j < item->fields_size(); ++j) {
  //    CLOG(INFO) << " " << item->fields(j) << " ";
  //  }
  //}
}

