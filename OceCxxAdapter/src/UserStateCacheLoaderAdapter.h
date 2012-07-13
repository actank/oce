#ifndef _USERSTATECACHELOADERADAPTER_H_
#define _USERSTATECACHELOADERADAPTER_H_

#include <AdapterI.h>
#include <UserStateCache.h>

namespace xce {
namespace userstatecache {

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


class UserStateCacheLoaderAdapter : public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::CoreChannel, UserStateCacheLoaderAdapter> {
public:
	UserStateCacheLoaderAdapter();
	virtual ~UserStateCacheLoaderAdapter();
	virtual void load(Ice::Int id);
	void doLoad(const vector<int>& userIds);
        virtual std::string name();
        virtual std::string endpoints();
        virtual size_t cluster();
private:
	InvokeLoad _invokeLoad;
	UserStateCacheLoaderPrx getUserStateCacheLoader(int id);
	std::vector<UserStateCacheLoaderPrx> _managers;
};

}
}

#endif
