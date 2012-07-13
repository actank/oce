#ifndef _UGC_ITEM_H_
#define _UGC_ITEM_H_

#include <set>

namespace item {

class UGCItem {
public:
	UGCItem() {}
	UGCItem(long ugcId, int commentCount, int viewCount, int type) {
		_ugcId = ugcId;
		_commentCount = commentCount;
		_viewCount = viewCount;
		_type = type;
		_interest = 0;
	}

	~UGCItem() {}

	long getUgcId() const {
		return _ugcId;
	}

	int getCommentCount() const {
		return _commentCount;
	}

	int getViewCount() const {
		return _viewCount;
	}

	int getUGCType() const {
		return _type;
	}

	int getUGCWeight() const {
		//return _commentCount * 5 + _viewCount * 1;             //回复一次算5分，浏览一次算1分
		return _commentCount * 5 + _viewCount * 1 + _interest;             //回复一次算5分，浏览一次算1分
	}

	void setInterest(int interest) {
		_interest = interest;
	}

	int getInterest() {
		return _interest;
	}

private:
	long _ugcId;
	int _commentCount;
	int _viewCount;
	int _type;
	int _interest;
};

}

#endif
