#ifndef FEEDDISPATCHERADAPTER_H_
#define FEEDDISPATCHERADAPTER_H_

#include "RFeed.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"


namespace xce {
namespace feed {

class FeedDispatcherAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedDispatcherAdapter> {
public:
	FeedDispatcherAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
//	void deliverSupply(int supplier, int acceptor, const Ice::Context& ctx);
	void dispatch(const FeedSeedPtr& seed, const FeedConfigPtr&);
	void dispatchReply( const ReplyDataPtr&,const FeedConfigPtr&);
	void addBuddyNotify(int host, int guest);
	void joinPageNotify(int page, int fans);
	void buddyApplyNotify(int host, int guest);

  void changeBuddyNotify(int host, int guest);

  void Reload(LoadType type, int id);
  void ReloadBatch(LoadType type, const vector<int> & ids);
protected:
	virtual string name() {
		return "FeedDispatcher";
	}
	virtual string endpoints() {
		return "@FeedDispatcher";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedDispatcherPrx getManager(int id);
	FeedDispatcherPrx getManagerOneway(int id);

	vector<FeedDispatcherPrx> _managersOneway;
	vector<FeedDispatcherPrx> _managers;
};

};
};

#endif /*BUDDYCOREADAPTER_H_*/
