#include "RecommendFriendAdapter.h"
using namespace xce::recommendfriend::adapter;
using namespace std;

//---------------------------------------------------------------------------

void RecommendFriendAdapter::removeRecommendFriend(int ownerId, int otherId){
	getRecommendFriendManagerOneway(ownerId)->removeRecommendFriend(ownerId,otherId);
}

RecommendFriendPrx RecommendFriendAdapter::getRecommendFriendManagerOneway(int id) {
	return locate<RecommendFriendPrx>("M", ONE_WAY);
//	return locate<RecommendFriendPrx>(_managersOneway, "M", id, ONE_WAY);
}
