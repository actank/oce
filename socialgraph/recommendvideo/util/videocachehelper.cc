//#include <boost/lexical_cast.hpp>
//#include <boost/foreach.hpp>
//#include <vector>
#include <string>
#include "socialgraph/recommendvideo/util/videocachehelper.h"

using namespace xce::socialgraph;

typedef std::vector<int> IntVec;
typedef std::vector<std::string> StrVec;
typedef std::set<int> IntSet;

/*
Cache storage structure:
--------------------------------------------------------------------------
int     long       long        int               int 
user_id | video_id1 | share_id1 |  share_user_id1 | share_count | ... |
--------------------------------------------------------------------------
*/

void VideoCacheHelper::setRcdVideoInforToCache(UserVideo user_video) 
{
  //CLOG(INFO) << "************** build video rcd cache data: store user's recommended video infomation to cache *********************";
  //CLOG(INFO) << "Fetch video block list from socialgraph_block data instance";
  int user_id = user_video.getUserId();
  IntSet block_set;
  IntVec sg_block_video_vec = DbUtil::GetInstance().GetSgBlockListByBiz(user_id, "RCDVIDEO");
  if(!sg_block_video_vec.empty()) {
    block_set.insert(sg_block_video_vec.begin(), sg_block_video_vec.end());
  }

  Items items;
  VideoVec video_items = user_video.getVideoVec();
  if(!video_items.empty()) {
    for(VideoVec::iterator it=video_items.begin(); it!=video_items.end(); ++it) {
      IntSet::iterator result=block_set.find( (*it).video_id );
      if(result == block_set.end()) {
        Item* item = items.add_items();
        item->set_id( (*it).video_id );
        StrVec videocache_items;
        videocache_items.push_back( boost::lexical_cast<std::string>( (*it).share_id) );
        videocache_items.push_back( boost::lexical_cast<std::string>( (*it).share_user_id) );
        videocache_items.push_back( boost::lexical_cast<std::string>( (*it).share_count) );
        for(StrVec::iterator s=videocache_items.begin(); s!=videocache_items.end();++s) {
          item->add_messages(*s);
        } 
        //BOOST_FOREACH(const std::string& item, videocache_items) {
        //  item->add_messages( item );
        //}
      }
      
    }
  }

  //Seiralize the data structure built by protobuf
  std::vector<unsigned char> bin_data;
  int byte_size = items.ByteSize();
  CLOG(INFO) << "user_id: " << user_id << " protobuf set size: " << byte_size;
  bin_data.resize(byte_size);
  items.SerializeToArray(bin_data.data(), byte_size);

  CLOG(INFO) << "store user's recommended video infomation to cache";
  const static std::string kBiz = "RCDVIDEO";
  BusinessCacheAdapter::GetInstance().Set(kBiz, user_id, bin_data);

  CLOG(INFO) << "test";
  std::vector<unsigned char> test_data;
  BusinessCacheAdapter::GetInstance().Get(kBiz, user_id, test_data);
  CLOG(INFO) << "user_id: " << user_id << " protobuf get size: " << test_data.size();
  Items test_items;
  test_items.ParseFromArray(test_data.data(), test_data.size());
  ::google::protobuf::RepeatedPtrField<Item>* seq = test_items.mutable_items();
  for(int i=0; i < seq->size(); ++i) {
    Item* item = test_items.mutable_items( i );
    //::google::protobuf::RepeatedPtrField<std::string>* msgs = item->mutable_messages();
    CLOG(INFO) << "video_id: " << item->id() 
               << " share_id: " << boost::lexical_cast<long>(*item->mutable_messages(0))
               << " share_user_id: " << boost::lexical_cast<int>(*item->mutable_messages(1))
               << " share_count: " << boost::lexical_cast<int>(*item->mutable_messages(2))
               << std::endl;
  }
}
