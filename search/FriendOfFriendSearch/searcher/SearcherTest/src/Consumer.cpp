#include <FriendOfFriendSearcherAdapter.h>
#include <SearchModel.h>
#include <vector>

#include "Consumer.h"
using namespace std;
using namespace com::renren::search2::friendoffriend;
using namespace com::xiaonei::search2::model;


Consumer::Consumer(QueryQueue& message_queue, int begin, int limit):message_queue_(message_queue), begin_(begin), limit_(limit)  {
}


void Consumer::run()  {
  while (true) {
    QueryUnit query_unit_;
    message_queue_.Get(query_unit_);
    //for(int i = 0; i < query_unit_.size(); i++)  {
      //cout<<"Get size is :"<<query_unit_.size()<<endl;
      IndexCondition query;
      query.queryCondition["query"] = query_unit_.query;
      query.opUserCondition["GENERAL|user_basic.id"] = query_unit_.id;
      //cout<<"search_content: "<<query.queryCondition["query"]<<endl;
      //cout<<"GENERAL|user_basic.id: "<<query.opUserCondition["GENERAL|user_basic.id"]<<endl;
      //cout<<"begin:"<<begin_<<endl;
      //cout<<"limit:"<<limit_<<endl;
      query.type = OpenSearch;
      IndexResultPtr result;
      result = FriendOfFriendSearcherAdapter::instance().FriendOfFriendSearch(query, begin_, limit_);
      if(result != NULL)  {
        //std::cout<<"totalCount:"<<result->totalCount<<std::endl;
        //std::cout<<"currentPos:"<<result->currentPos<<std::endl;
        //std::cout<<"timeCost:"<<result->timeCost<<" ms"<<std::endl;
        //std::cout<<"typedCount:"<<result->typedCount[User]<<std::endl;
        //std::cout<<"result contents size:"<<result->contents.size()<<std::endl;
        /*for(int j = 0; j < result->contents.size(); j++)  {
          std::cerr<<"id:"<<result->contents.at(j).id<<" friendType:"<<result->contents.at(j).content["FriendsType"]
            <<" commonFriends:"<<result->contents.at(j).content["CommonFriendsNum"]<<" searchType:"<<result->contents.at(j).type<<std::endl;
        }*/
      }
   // }
  }
}

