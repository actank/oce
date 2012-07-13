#ifndef WEBTRANSITSFORIQADAPTER_H_
#define WEBTRANSITSFORIQADAPTER_H_


#include "AdapterI.h"
#include "Singleton.h"
#include "WTransit.h"
#include "RFeed.h"

namespace talk{
namespace adapter {

using namespace talk::wtransit;
using namespace xce::feed;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;

class WTransitForIqAdapter : public MyUtil::ReplicatedClusterAdapterI<WTransitManagerPrx>, public MyUtil::Singleton<WTransitForIqAdapter> {
public:
	WTransitForIqAdapter() : MyUtil::ReplicatedClusterAdapterI<WTransitManagerPrx> ("ControllerWTransitForIq",120,300,new TalkChannel){
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	//void deliverNotify(Ice::Int userid,const string& title, const string& msg);
	void deliverFeed(const FeedSeedPtr& feed, const MyUtil::IntSeq& target);
	void deliverFeedReply( const ReplyDataPtr& replyData, const MyUtil::IntSeq& target);
	void loadOfflineFeed(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId);
	//void deliverReply(const mop::hi::svc::dispatch::NotePtr& note);
	void deliverNotifyWebpager(const MyUtil::IntSeq& toids, const string& content);
	void deliverNotifyClient(const MyUtil::IntSeq& toids, const string& content);
	void deliverNotifyPhone(const MyUtil::IntSeq& toids, const string& content);
	void deliverAppNotice(int toid, int appid, const string& appName, const string& appIcon, const string& msg);
	
	void loadOfflineFeedReply(const JidPtr& jid, int stype, int owner, Ice::Long source, const string& iqId);
	map<Ice::Long, int> getFeedReplyCount(int userid, const vector<Ice::Long>& feeds);
  void deliverFeedReplyCount(const JidPtr& jid, const vector<Ice::Long>& feeds, const string& iqid, const string& xmlns);
protected:

	WTransitManagerPrx getManagerOneway(int id);
	WTransitManagerPrx getManager(int id);
	virtual string name() {
		return "WTransitForIq";
	}
	virtual string endpoints() {
		return "@WTransitForIq";
	}
	virtual size_t cluster() {
		return 5;  
	}
	vector<WTransitManagerPrx> _managersOneway;
	vector<WTransitManagerPrx> _managers;

};
}
;
}
;



#endif /*WEBTRANSITSADAPTER_H_*/
