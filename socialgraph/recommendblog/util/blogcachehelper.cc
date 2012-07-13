//#include <boost/lexical_cast.hpp>
//#include <boost/foreach.hpp>
//#include <vector>
#include <string>
#include "socialgraph/recommendblog/util/blogcachehelper.h"

using namespace xce::socialgraph;

typedef std::vector<int> IntVec;
typedef std::vector<std::string> StrVec;
typedef std::set<int> IntSet;

/*
Cache storage structure:
--------------------------------------------------------------------------
int     long       long        int               int 
user_id | blog_id1 | share_id1 |  share_user_id1 | share_count | ... |
--------------------------------------------------------------------------
*/

void BlogCacheHelper::setRcdBlogInforToCache(UserBlog user_blog) 
{
  //CLOG(INFO) << "************** build blog rcd cache data: store user's recommended blog infomation to cache *********************";
  //CLOG(INFO) << "Fetch blog block list from socialgraph_block data instance";
  int user_id = user_blog.getUserId();
  IntSet block_set;
  IntVec sg_block_blog_vec = DbUtil::GetInstance().GetSgBlockListByBiz(user_id, "RCDBLOG");
  if(!sg_block_blog_vec.empty()) {
    block_set.insert(sg_block_blog_vec.begin(), sg_block_blog_vec.end());
  }

  Items items;
  BlogVec blog_items = user_blog.getBlogVec();
  if(!blog_items.empty()) {
    for(BlogVec::iterator it=blog_items.begin(); it!=blog_items.end(); ++it) {
      IntSet::iterator result=block_set.find( (*it).blog_id );
      if(result == block_set.end()) {
        Item* item = items.add_items();
        item->set_id( (*it).blog_id );
        StrVec blogcache_items;
        blogcache_items.push_back( boost::lexical_cast<std::string>( (*it).share_id) );
        blogcache_items.push_back( boost::lexical_cast<std::string>( (*it).share_user_id) );
        blogcache_items.push_back( boost::lexical_cast<std::string>( (*it).share_count) );
        for(StrVec::iterator s=blogcache_items.begin(); s!=blogcache_items.end();++s) {
          item->add_messages(*s);
        } 
        //BOOST_FOREACH(const std::string& item, blogcache_items) {
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

  CLOG(INFO) << "store user's recommended blog infomation to cache";
  const static std::string kBiz = "RCDBLOG";
  BusinessCacheAdapter::GetInstance().Set(kBiz, user_id, bin_data);

  //CLOG(INFO) << "test";
  //std::vector<unsigned char> test_data;
  //BusinessCacheAdapter::GetInstance().Get(kBiz, user_id, test_data);
  //CLOG(INFO) << "user_id: " << user_id << " protobuf get size: " << test_data.size();
  //Items test_items;
  //test_items.ParseFromArray(test_data.data(), test_data.size());
  //::google::protobuf::RepeatedPtrField<Item>* seq = test_items.mutable_items();
  //for(int i=0; i < seq->size(); ++i) {
  //  Item* item = test_items.mutable_items( i );
  //  //::google::protobuf::RepeatedPtrField<std::string>* msgs = item->mutable_messages();
  //  CLOG(INFO) << "blog_id: " << item->id() 
  //             << " share_id: " << boost::lexical_cast<long>(*item->mutable_messages(0))
  //             << " share_user_id: " << boost::lexical_cast<int>(*item->mutable_messages(1))
  //             << " share_count: " << boost::lexical_cast<int>(*item->mutable_messages(2))
  //             << std::endl;
  //}
}
