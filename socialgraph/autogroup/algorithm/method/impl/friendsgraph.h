#ifndef _FRIENDSGRAPH_H_
#define _FRIENDSGRAPH_H_

struct FriendsGraph {
	FriendsGraph(int friendsnum) { 
		friendsnum_ = friendsnum;
		graph_ = new int *[friendsnum_];
		for (int i = 0; i < friendsnum_; ++i) {
			graph_[i] = new int[friendsnum_];
		}

		for (int i = 0; i < friendsnum_; ++i) {
			graph_[i][i] = 0;
			for (int j = i + 1; j < friendsnum_; ++j) {
				graph_[i][j] = 0;
				graph_[j][i] = 0;
			}
		}
	}

	~FriendsGraph() {
		if (NULL != graph_ && 0 != friendsnum_) {
			for (int i = 0; i < friendsnum_; ++i) {
				delete[] graph_[i];
			}		
			delete[] graph_;
		}
	}
	
	int friendsnum_;
	int **graph_;
};

#endif
