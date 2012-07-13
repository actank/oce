#ifndef __DoingSearchFacadeI_h__
#define __DoingSearchFacadeI_h__

#include <DoingSearch.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce {
namespace doingsearch {


class Job: public IceUtil::Shared {
public:
	Job(Ice::Long, Ice::Long, Ice::Long, const std::string&);
	void notifyDoingSearchNewContainer();
private:
	Ice::Long _doingID;
	Ice::Long _userID;
	Ice::Long _timestamp;
	std::string _content;
};
typedef IceUtil::Handle<Job> JobPtr;

class NotifyDoingSearchNewContainerTask : virtual public MyUtil::Task {
public:
	NotifyDoingSearchNewContainerTask(const std::vector<JobPtr>& jobs);
	virtual void handle();
private:
	std::vector<JobPtr> _jobs;
};

class DoingSearchFacadeI;
typedef IceUtil::Handle<DoingSearchFacadeI> DoingSearchFacadeIPtr;

class DoingSearchFacadeI : virtual public DoingSearchFacade, virtual public IceUtil::Monitor<IceUtil::Mutex>  {
public:
	DoingSearchFacadeI();
	void initialize();
	void enableNotifyDoingSearchNewContainer(bool flag);
	virtual void access(Ice::Long, Ice::Long, Ice::Long, const std::string&, const Ice::Current&);
private:
	bool _enableNotifyDoingSearchNewContainer;
	std::vector<JobPtr> _jobs;
	void retrive(std::vector<JobPtr>& toInvokeCache); // modify is applied to toInvokeCache
	class Dispatcher: virtual public IceUtil::Thread {
	public:
		Dispatcher(DoingSearchFacadeIPtr facade);
		virtual void run();
	private:
		void invoke();
		bool shouldInvoke();
		void invokeAndClearCache();

		std::vector<JobPtr> _toInvokeCache;
		time_t _lastInvokeTime;
		DoingSearchFacadeIPtr _facade;
	};

	typedef IceUtil::Handle<Dispatcher> DispatcherPtr;
	DispatcherPtr _dispatchThread;
};

}
}
#endif
