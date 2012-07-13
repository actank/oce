#ifndef __PASSPORTCACHE_LOADER_ADAPTER_H__
#define __PASSPORTCACHE_LOADER_ADAPTER_H__

#include <AdapterI.h>
#include <PassportCache.h>
#include <TaskManager.h>

namespace xce {
namespace passportcache {

const int CLUSTER = 0; 

class InvokeLoad : virtual public IceUtil::Thread {
public:
	void invoke(const PassportCacheDataPtr& ticket);
	void invoke(const PassportCacheDataSeq& tickets);
protected:
	virtual void run();
private:
	PassportCacheDataSeq _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class PassportCacheLoaderAdapter : public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::CoreChannel, PassportCacheLoaderAdapter> {
public:
	PassportCacheLoaderAdapter();
	void load(const PassportCacheDataPtr& ticket);
	void reload(int userId);
	void load(const PassportCacheDataSeq& tickets);
	void doLoad(const PassportCacheDataSeq& tickets);
private:
	virtual std::string name();
	virtual std::string endpoints();
	virtual size_t cluster();
	PassportCacheLoaderPrx getPassportCacheLoader(int id);
	PassportCacheLoaderPrx getPassportCacheLoaderOneway(int id);
	InvokeLoad _invokeLoad;

	std::vector<PassportCacheLoaderPrx> _managers;
	std::vector<PassportCacheLoaderPrx> _managersOneway;
};

}
}

#endif
