/// 
/// @author lijie@opi-corp.com
/// @date 2010-05-17
/// 
#ifndef __FRIEND_OF_FRIEND_SEARCHER_ADAPTER_H__
#define __FRIEND_OF_FRIEND_SEARCHER_ADAPTER_H__

#include "SearchModel.h"
#include "SearchServer.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

//#define USE_SEARCH_TEST_CHANNEL
namespace com {
namespace renren {
namespace search2 {
namespace friendoffriend {
using namespace com::xiaonei::search2::model;
using namespace com::xiaonei::search2::server;

class FriendOfFriendSearcherAdapter : public MyUtil::AdapterI, 
//#ifdef USE_SEARCH_TEST_CHANNEL
  public MyUtil::AdapterISingleton<MyUtil::AddressbookChannel, FriendOfFriendSearcherAdapter> {
//#else 
//  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, FriendOfFriendSearcherAdapter> {
//#endif
   public:
     FriendOfFriendSearcherAdapter(); 
     IndexResultPtr FriendOfFriendSearch(IndexCondition& query, int begin, int limit);
   
   private:
     virtual string name() {
       return "FOF";
     }
     virtual string endpoints() {
       return "@FriendOfFriendSearcher";
     }
     virtual size_t cluster() {
       return 1;
     }

     
     IndexManagerPrx GetFriendOfFriendSearcherManager(int id);
     vector<IndexManagerPrx> _managers;
  };


}
}
}
}



#endif
