
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "FeedBodyAdapter.h"
#include "ServiceI.h"

using namespace ::std;
using namespace ::MyUtil;
using namespace ::mop::hi::svc::feed;

void
MyUtil::initialize(){
}

int main(int argc, char* argv[]){
	IntSeq feedIds;
	for(int id=1054693150;id<1054694150;++id){
		feedIds.push_back(id);
	}
	cout << "FeedIds size is: " << feedIds.size() << endl;
	{
		IceUtil::Time begin = IceUtil::Time::now();
		ObjectResultPtr om = FeedBodyAdapter::instance().get(0, feedIds);
		IceUtil::Time end = IceUtil::Time::now();
		cout << "Warmup Result size is: " << om->data.size() << " cost:" << (end-begin).toMilliSeconds() << "ms" << endl;
	}
	{
		IceUtil::Time begin = IceUtil::Time::now();
		ObjectResultPtr om = FeedBodyAdapter::instance().get(0, feedIds, 10, true);
		IceUtil::Time end = IceUtil::Time::now();
		cout << "Paralleled Result size is: " << om->data.size() << " cost:" << (end-begin).toMilliSeconds() << "ms" << endl;
	}
	{
		IceUtil::Time begin = IceUtil::Time::now();
		ObjectResultPtr om = FeedBodyAdapter::instance().get(0, feedIds, 10, false);
		IceUtil::Time end = IceUtil::Time::now();
		cout << "Synchonrized Result size is: " << om->data.size() << " cost:" << (end-begin).toMilliSeconds() << "ms" << endl;
	}
}
