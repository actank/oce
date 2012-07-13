#ifndef __ShareLogicI_h__
#define __ShareLogicI_h__

#include <ShareSearch.h>
#include <Ice/Ice.h>
#include "ServiceI.h"

namespace xce {
namespace sharesearch{

class NotifyShareIndexTask : virtual public MyUtil::Task {
public:
	NotifyShareIndexTask(UrlDetailInfoSeq& infos):infos_(infos){};
	virtual void handle();
private:
	UrlDetailInfoSeq& infos_;
};
class NotifyShareCacheTask : virtual public MyUtil::Task {
public:
	NotifyShareCacheTask(UrlInfoSeq& infos):infos_(infos){};
	virtual void handle();
private:
	UrlInfoSeq& infos_;
};

class ShareLogicI;
typedef IceUtil::Handle<ShareLogicI> ShareLogicIPtr;

class ShareLogicI : virtual public ShareLogic{
public:
	ShareLogicI(){};
	void initialize();
	virtual void create(const UrlDetailInfo& info, const Ice::Current& = Ice::Current());
	virtual void share(const UrlInfo& info, const Ice::Current& = Ice::Current());
	void swapCreate(UrlDetailInfoSeq& infos);
	void swapShare(UrlInfoSeq& infos);

    virtual bool isValid(const Ice::Current& = Ice::Current());

    virtual void setValid(bool,
                          const Ice::Current& = Ice::Current());

private:
	std::vector<UrlDetailInfo> createUrlInfos;
	std::vector<UrlInfo> shareUrlInfos;
	bool state_;
	IceUtil::RWRecMutex state_lock_;
	IceUtil::Mutex create_mutex_;
	IceUtil::Mutex share_mutex_;

	class Dispatcher: virtual public IceUtil::Thread {
	public:
		Dispatcher(ShareLogicIPtr logic);
		virtual void run();
		void invoke();
		void invokeAndClearCache();
		void swap();
	private:
		ShareLogicIPtr logic_;
		time_t lastIndexTime_;
		time_t lastUpdateTime_;
		std::vector<UrlInfo> shareInfos_;
		std::vector<UrlDetailInfo> createInfos_;
		size_t maxTime_;
		size_t maxSize_;
	    IceUtil::Monitor<IceUtil::Mutex> thread_mutex_;
	};

	typedef IceUtil::Handle<Dispatcher> DispatcherPtr;
	DispatcherPtr _dispatchThread;
};

}
}
#endif
