/*
 * FeedViewAdapter.h
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#ifndef FEEDNEWSADAPTER_H_
#define FEEDNEWSADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"
#include "FeedView.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedViewAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel,
				FeedViewAdapter> {
public:
	FeedViewAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

  void Test(int uid, string name, int render, int from, int count);

  FeedDataSeq GetByIds2View(int uid, const string& name, int render, const FeedIdSeq & fids, int view);


protected:
	virtual string name() {
		return "FeedViewN";
	}
	virtual string endpoints() {
		return "@FeedViewN";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedViewPrx getManager(int id);
	FeedViewPrx getManagerOneway(int id);

	vector<FeedViewPrx> _managersOneway;
	vector<FeedViewPrx> _managers;
};

}
;
}
;
#endif /* FEEDITEMADAPTER_H_ */
