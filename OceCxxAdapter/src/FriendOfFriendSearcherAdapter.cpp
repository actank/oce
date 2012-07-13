#include "FriendOfFriendSearcherAdapter.h"
#include <vector>
//TODO: 1. 所有接口提供批量操作 2. 利用异步机制
//

using namespace MyUtil;
namespace com {
namespace renren {
namespace search2 {
namespace friendoffriend  {
  using namespace std;

  FriendOfFriendSearcherAdapter::FriendOfFriendSearcherAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
  }

  IndexManagerPrx FriendOfFriendSearcherAdapter::GetFriendOfFriendSearcherManager (int id) {
    return locate<IndexManagerPrx>(_managers, "FOF", id, TWO_WAY, -1)->ice_locatorCacheTimeout(60);
    //locateAll<FriendOfFriendSearcherManagerPrx>(_managers, "F", id % cluster(), TWO_WAY);
  }

  IndexResultPtr FriendOfFriendSearcherAdapter::FriendOfFriendSearch(IndexCondition& query, int begin, int limit)  {
    IndexResultPtr search_null = NULL;
    int user_id = atoi((query.opUserCondition["GENERAL|user_basic.id"]).c_str());
    if(user_id < 0)  {
      return search_null;
    }
    IndexManagerPrx proxy = GetFriendOfFriendSearcherManager(user_id);
          try  {
            if(proxy)  {
                IndexResultPtr search_result;
                search_result = proxy->searchIndex(query, begin, limit);
                std::cerr<<"The final combined results are below: ";
                std::cerr<<"currentPos:"<<search_result->currentPos<<" totalCount:"<<search_result->totalCount<<" timeCost:"
                                  <<search_result->timeCost<<" searchTypeCount:"<<search_result->typedCount[User];
                return search_result;
            }
          }  catch (Ice::Exception& e) {
            std::cerr<<"FriendOfFriendSearcherAdapter::get Ice::Exception " << e << ", proxy=" << proxy<<std::endl;
          } catch (std::exception& e) {
            std::cerr<<"FriendOfFriendSearcherAdapter::get std::exception " << e.what() << ", proxy=" << proxy<<std::endl;
          } catch (...) {
            std::cerr<<"FriendOfFriendSearcherAdapter::get Unknown exception" << ", proxy=" << proxy<<std::endl;
          } 
        return search_null;  
  }


/*bool FriendOfFriendSearcherAdapter::isValid(const Ice::ObjectPrx& proxy){
	return FriendOfFriendSearcherManagerPrx::uncheckedCast(proxy)->isValid();
}*/

}
}
}
}
