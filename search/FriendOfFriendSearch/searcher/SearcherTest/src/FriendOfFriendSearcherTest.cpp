#include <FriendOfFriendSearcherAdapter.h>
#include <SearchModel.h>
#include <vector>
#include "QueryQueue.h"
#include "Consumer.h"
#include <fstream>
#include <iostream>
//#include <glog/logging.h>

#define BATCH 50

using namespace std;
using namespace MyUtil;
using namespace IceUtil;
using namespace com::renren::search2::friendoffriend;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::search2::model;
using namespace com::xiaonei::search2::server;
;
void usage()  {
  std::cerr<<"FriendOfFriendSearch <-f> <file> <id> <query1> <query2> ... <queryn> <begin> <limit>"<<std::endl;
}
const int kConsumerNum = 5;

int main(int argc, char** argv)
{
  if(strcmp(argv[1], "-f") == 0)  {
    //google::InstallFailureSignalHandler();
    std::cerr<<"entering in..."<<std::endl;
    ifstream file(argv[2]);
    string line, query, id;
    int begin = 0, limit = 10;
    size_t index;
    QueryQueue query_queue;
    for (int i=0; i<kConsumerNum; i++) {
      IceUtil::ThreadPtr t = new Consumer(query_queue, begin, limit);
      t->start(128*1024);
    }
    int count = 0;
    while(!file.eof())  {
      getline(file, line, '\n');
      index = line.find('\t');
      if(index == -1) break;
      //cout<<line<<endl;
      id = line.substr(0, index);
      query = line.substr(index+1,line.length() - index - 1);
      QueryUnit temp, output;
      temp.id = id;
      temp.query = query;
      query_queue.Put(temp);
      //cout<<count<<endl;
      count++;
    }
    sleep(57600);
    cout<<"Done!"<<endl;
    return 0;
  }
  else if(argc == 5 && strcmp(argv[1], "-f") != 0)  {
    int begin, limit;
    string user_id;
    string search_content = "";
    user_id = argv[1];
    begin = atoi(argv[argc-2]);
    limit = atoi(argv[argc-1]);
    for(int i = 2; i < argc - 2; i++)  {
      search_content += (string)argv[i];
      if(i != argc - 3)
        search_content += " ";
    }

    IndexCondition query;
    query.queryCondition["query"] = search_content;
    query.opUserCondition["GENERAL|user_basic.id"] = user_id;
    cout<<"search_content: "<<search_content<<endl;
    cout<<"GENERAL|user_basic.id: "<<user_id<<endl;
    cout<<"begin:"<<begin<<endl;
    cout<<"limit:"<<limit<<endl;
    query.type = OpenSearch;
    IndexResultPtr result;
    result = FriendOfFriendSearcherAdapter::instance().FriendOfFriendSearch(query, begin, limit);
    if(result != NULL)  { 
      std::cout<<"totalCount:"<<result->totalCount<<std::endl;
      std::cout<<"currentPos:"<<result->currentPos<<std::endl;
      std::cout<<"timeCost:"<<result->timeCost<<" ms"<<std::endl; 
      std::cout<<"typedCount:"<<result->typedCount[User]<<std::endl;
      std::cout<<"result contents size:"<<result->contents.size()<<std::endl;
      for(int i = 0; i < result->contents.size(); i++)  {
        std::cerr<<"id:"<<result->contents.at(i).id<<" friendType:"<<result->contents.at(i).content["FriendsType"]
          <<" commonFriends:"<<result->contents.at(i).content["CommonFriendsNum"]<<" searchType:"<<result->contents.at(i).type<<std::endl;
      }
    }
    return 0;
  }
  else  {
    std::cerr<<argc<<" "<<argv[1]<<" skip out"<<std::endl;
    usage();
    return 0;
  }
}
