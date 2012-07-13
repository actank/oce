#include "genericsortutil.h"

void GenericSortUtil::SortFriendsOrderByRelationNum(boost::shared_ptr<FriendsGraph>& friendgraphptr, std::vector<int>& friendlist) {
	std::vector<RelationNumItem> itemlist;
	for (int i = 0; i < friendgraphptr->friendsnum_; ++i) {
		RelationNumItem item;
		item.index = i;
		item.relation = AccountNumber(friendgraphptr->graph_[i], friendgraphptr->friendsnum_);
		itemlist.push_back(item);
	}

	std::sort(itemlist.begin(), itemlist.end(), RelationNumComp());
	
	std::vector<int> newfriendlist;
	for (std::vector<RelationNumItem>::iterator iter = itemlist.begin();
			iter != itemlist.end(); ++iter) {
		try {
			int friendid = friendlist.at(iter->index);
			newfriendlist.push_back(friendid);
		} catch (std::exception e) {
			std::cout << "[GenericSortUtil] SortFriendsOrderByRelationNum std::exception " << e.what() << std::endl;
		} catch (...) {
			std::cout << "[GenericSortUtil] SortFriendsOrderByRelationNum unknown::Exception " << std::endl;
		}
	}

	friendlist.swap(newfriendlist);

	boost::shared_ptr<FriendsGraph> newfriendgraphptr = boost::shared_ptr<FriendsGraph>(new FriendsGraph((int)friendlist.size()));
	for (int i = 0; i < newfriendgraphptr->friendsnum_; ++i) {
		newfriendgraphptr->graph_[i][i] = 0;
		for (int j = i + 1; j < newfriendgraphptr->friendsnum_; ++j) {
			newfriendgraphptr->graph_[i][j] = 0;
			newfriendgraphptr->graph_[j][i] = 0;
		}
	}

	for (int i = 0; i < friendgraphptr->friendsnum_; ++i) {
		int oldindex1 = itemlist.at(i).index;
		for (int j = i + 1; j < friendgraphptr->friendsnum_; ++j) {
			int oldindex2 = itemlist.at(j).index;
			if (1 == friendgraphptr->graph_[oldindex1][oldindex2]) {
				newfriendgraphptr->graph_[i][j] = 1;
				newfriendgraphptr->graph_[j][i] = 1;
			}
		}
	}

	friendgraphptr = newfriendgraphptr;
}

int GenericSortUtil::AccountNumber(int* members, int friendsnum) { 
  int account = 0;
  int *p = members;

  while((p = std::find(p, members+friendsnum, 1)) != members+friendsnum){
      ++account;       
      ++p;
  }
  return account;
}
