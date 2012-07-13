#ifndef __USERADSCACHE_LOADER_ADAPTER_H__
#define __USERADSCACHE_LOADER_ADAPTER_H__

#include <AdapterI.h>
#include <UserAdsCache.h>
#include <TaskManager.h>

namespace xce {
namespace useradscache {

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

class UserAdsCacheLoaderAdapter : public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::CoreChannel, UserAdsCacheLoaderAdapter> {
public:
	UserAdsCacheLoaderAdapter();
	void load(int userId);
	void load(const vector<int>& userIds);
	void doLoad(const vector<int>& userIds);
private:
	virtual std::string name();
	virtual std::string endpoints();
	virtual size_t cluster();
	UserAdsCacheLoaderPrx getUserAdsCacheLoader(int id);
	UserAdsCacheLoaderPrx getUserAdsCacheLoaderOneway(int id);
	InvokeLoad _invokeLoad;

	std::vector<UserAdsCacheLoaderPrx> _managers;
	std::vector<UserAdsCacheLoaderPrx> _managersOneway;
};

}
}

#endif
