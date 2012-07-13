#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "BuddyCacheByIdAdapter.h"
#include "BuddyApplyCacheAdapter.h"
#include "ServiceI.h"

using namespace ::std;
using namespace ::MyUtil;
//using namespace ::mop::hi::svc::feed;
using namespace xce::buddy;
using namespace xce::buddyapplycache;

void MyUtil::initialize() {
}

//int ice_client(int argc, char* argv[] ){
//	Ice::CommunicatorPtr ic;
//	try {
//		ic = Ice::initialize(argc,argv);
//		Ice::ObjectPrx object = ic -> stringToProxy("M@XceLogger")->ice_datagram();
//		Ice::PropertiesPtr props = ic->getProperties();
//		int property1 = props->getPropertyWithDefault("Ice.property1", "");
//		XceLoggerPrx xcelogger = XceLoggerPrx::checkedCast(object);
//		if(!xcelogger){
//			throw "Invalid proxy";
//		}
//		xcelogger->xxx();
//	} catch ( const Ice::Exception &ex ) {
//		cerr << ex << endl;
//		status = 1;
//	} catch ( const char * msg ) {
//		cerr << ex << endl;
//		status = 1;
//	}
//	return status;
//}

//void feedbodytest(){
//	IntSeq feedIds;
//	for(int id=1054693150;id<1054694150;++id){
//		feedIds.push_back(id);
//	}
//	cout << "FeedIds size is: " << feedIds.size() << endl;
//	{
//		IceUtil::Time begin = IceUtil::Time::now();
//		ObjectResultPtr om = FeedBodyAdapter::instance().get(0, feedIds);
//		IceUtil::Time end = IceUtil::Time::now();
//		cout << "Warmup Result size is: " << om->data.size() << " cost:" << (end-begin).toMilliSeconds() << "ms" << endl;
//	}
//	{
//		IceUtil::Time begin = IceUtil::Time::now();
//		ObjectResultPtr om = FeedBodyAdapter::instance().get(0, feedIds, 10, true);
//		IceUtil::Time end = IceUtil::Time::now();
//		cout << "Paralleled Result size is: " << om->data.size() << " cost:" << (end-begin).toMilliSeconds() << "ms" << endl;
//	}
//	{
//		IceUtil::Time begin = IceUtil::Time::now();
//		ObjectResultPtr om = FeedBodyAdapter::instance().get(0, feedIds, 10, false);
//		IceUtil::Time end = IceUtil::Time::now();
//		cout << "Synchonrized Result size is: " << om->data.size() << " cost:" << (end-begin).toMilliSeconds() << "ms" << endl;
//	}
//}

void buddycachebyidtest() {
	IceUtil::Time begin = IceUtil::Time::now();
	IntSeq abc = BuddyCacheByIdAdapter::instance().getFriendList(700000560);
	IceUtil::Time end = IceUtil::Time::now();
	cout << "Result size is: " << abc.size() << endl;
	cout << "Time cost:" << (end - begin).toMilliSeconds() << "ms" << endl;
}

void buddyapplycachetest() {
	IceUtil::Time begin = IceUtil::Time::now();
	BuddyApplyCacheDataSeq abc =
			BuddyApplyCacheAdapter::instance().getBuddyApplyCache(261947900, 0,
					-1);
	IceUtil::Time end = IceUtil::Time::now();
	cout << "Result size is: " << abc.size() << endl;
	cout << "Time cost:" << (end - begin).toMilliSeconds() << "ms" << endl;
}

int main(int argc, char* argv[]) {
	MyUtil::init_logger("Mce", "oce_log", "DEBUG");
	//	feedbodytest();
	for (int x = 0; x < 60; ++x) {
		buddycachebyidtest();
		sleep(1);
	}
	//		buddyapplycachetest();
}
