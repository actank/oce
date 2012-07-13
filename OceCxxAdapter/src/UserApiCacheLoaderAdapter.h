#ifndef __USERAPICACHE_LOADER_ADAPTER_H__
#define __USERAPICACHE_LOADER_ADAPTER_H__

#include <AdapterI.h>
#include <UserApiCache.h>
#include <TaskManager.h>

namespace xce {
namespace userapicache {

const int CLUSTER = 0; 

class InvokeLoad : virtual public IceUtil::Thread {
public:
	void invoke(int userId);
	void invoke(const MyUtil::IntSeq& userIds);
protected:
	virtual void run();
private:
	MyUtil::IntSeq _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class UserApiCacheLoaderAdapter : public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::CoreChannel, UserApiCacheLoaderAdapter> {
public:
	UserApiCacheLoaderAdapter();
	void load(int userId);
	void load(const vector<int>& userIds);
	void doLoad(const vector<int>& userIds);
private:
	virtual std::string name();
	virtual std::string endpoints();
	virtual size_t cluster();
	UserApiCacheLoaderPrx getUserApiCacheLoader(int id);
	UserApiCacheLoaderPrx getUserApiCacheLoaderOneway(int id);
	InvokeLoad _invokeLoad;

	std::vector<UserApiCacheLoaderPrx> _managers;
	std::vector<UserApiCacheLoaderPrx> _managersOneway;
};

}
}

#endif
