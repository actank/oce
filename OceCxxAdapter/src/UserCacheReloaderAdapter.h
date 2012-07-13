#ifndef USERCACHERELOADERADAPTER_H_
#define USERCACHERELOADERADAPTER_H_

#include <UserCache.h>
#include <AdapterI.h>
#include <IceUtil/Mutex.h>

#include <util/cpp/TimeCost.h>

namespace xce {
namespace usercache {

class UserCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserCacheReloaderPrx>,
								 public MyUtil::Singleton<UserCacheReloaderAdapter>,
								 public IceUtil::Thread{
public:
	UserCacheReloaderAdapter();
	
	void invoke(MyUtil::IntSeq& ids)
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		_ids.insert(_ids.end(), ids.begin(), ids.end());
		_mutex.notify();
	}

protected:
	void run()
	{
		while(true)
		{
			MyUtil::IntSeq ids;
			{
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if(_ids.size() <= 0)
				{
					_mutex.wait();
				}
				ids.swap(_ids);
			}
			ostringstream os;
			os << "UserCacheReloaderAdapter::run ids size = " << ids.size();
			MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str());
			try
			{
				getProxy(0)->reload(ids);
			}catch(...)
			{
				continue;
			}
		}
	}
private:
	MyUtil::IntSeq _ids;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

}
}

#endif
