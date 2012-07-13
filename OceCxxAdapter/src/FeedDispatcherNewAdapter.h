#ifndef FEEDDISPATCHERADAPTER_H_
#define FEEDDISPATCHERADAPTER_H_

#include "RFeed.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"


namespace xce {
namespace feed {

class FeedDispatcherNewAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedDispatcherNewAdapter> {
public:
	FeedDispatcherNewAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
//	void deliverSupply(int supplier, int acceptor, const Ice::Context& ctx);
	void dispatch(const FeedSeedPtr& seed, const FeedConfigPtr&);
	void dispatchReply( const ReplyDataPtr&,const FeedConfigPtr&);

  void InvalidateFactor(const string & factor);
  void SetSendConfig(int uid, int stype , int appid , bool newsconfig , bool miniconfig);
  void SetRecvConfig(int uid, int stype , int appid , bool newsconfig);

	void addBuddyNotify(int host, int guest);
	void joinPageNotify(int page, int fans);
protected:
	virtual string name() {
		return "FeedDispatcherNew";
	}
	virtual string endpoints() {
		return "@FeedDispatcherNew";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedDispatcherNewPrx getManager(int id);
	FeedDispatcherNewPrx getManagerOneway(int id);

	vector<FeedDispatcherNewPrx> _managersOneway;
	vector<FeedDispatcherNewPrx> _managers;
};

};
};

#endif /*BUDDYCOREADAPTER_H_*/
