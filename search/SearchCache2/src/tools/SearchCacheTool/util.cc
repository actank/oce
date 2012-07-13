#include "util.h"
#include <time.h>
void BinStr2ByteSeqMap(long userId, BinStringPtr pBinString, Int2ByteSeq& id2Map) {
  char * pBuffer=NULL;
  int len=0;
  if (NULL == pBinString.get() || 0 >= userId) {

    return;
  }

  pBinString->getBuf(&pBuffer, &len);

  if (NULL == pBuffer || 0 == len) {

    return;
  }

  id2Map[userId].assign(pBuffer, pBuffer+len);
}

int get_rand()
{

    //int i = srand(time(NULL));
    int i = rand() % 80000000;
//    sleep(1);
    return i;   
}


SearchCacheManagerPrx getSearchManager(SearchCacheChannel& channel, string search_cache_adapter_name) {
  string object_name("M@");
  std::cout << "Using " << object_name << search_cache_adapter_name << endl;
  return channel.locate<SearchCacheManagerPrx>(object_name, search_cache_adapter_name);
}

namespace xce  {
  namespace searchcache  {
    namespace test  {
BasicStatusResultHandlerI::BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<long>& ids, std::map<long, int>& id2status) : beginId_(beginId), ids_(ids), id2status_(id2status) {
}

bool BasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
  long id = (long) row["id"];
  int status = (int) row["status"];
  if(status < 3)  {
    cout<<id<<endl;
  } 
  if(status ==3)  {
    ids_.push_back(id);
    id2status_[id] = status;
  } 
  if(id>beginId_)
    beginId_=id;
  return true;
}
  
}
  }
}
