#ifndef _MUTUAL_FRIENDS_LOADER_I_H_
#define _MUTUAL_FRIENDS_LOADER_I_H_

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "MutualFriendsCache.h"
#include "../share/Factory.h"

namespace xce {
namespace buddy {

const int TASK_LEVEL_FANS_LOADER_RELOAD = 1;

//--------------------------MutualFriendsLoaderI--------------------------------
class MutualFriendsLoaderI: virtual public MutualFriendsLoader, virtual public MyUtil::Singleton<MutualFriendsLoaderI> {
public:
	virtual void reload(Ice::Long gid, const Ice::Current&);
private:
	friend class MyUtil::Singleton<MutualFriendsLoaderI>;
};

//-----------------------------SetDataHelper------------------------------------
class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
public:
	void initialize();
	void invokeDelay(Ice::Long, const Ice::ObjectPtr&);
	void invokeNow(Ice::Long, const Ice::ObjectPtr&);
	void swap(std::vector<pair<long, Ice::ObjectPtr> >& empty_vector);
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	std::vector<pair<long, Ice::ObjectPtr> > datas_;
	class SetDataThread: virtual public IceUtil::Thread {
	public:
		SetDataThread(SetDataHelper& helper) :
			helper_(helper) {
		}
		virtual void run();
	private:
		SetDataHelper& helper_;
	};
	typedef IceUtil::Handle<SetDataThread> SetDataThreadPtr;
	SetDataThreadPtr setter_;
};

//--------------------------------Task------------------------------------------
class ReloadTask: virtual public MyUtil::Task {
public:
	ReloadTask(Ice::Long gid, Ice::Int level = TASK_LEVEL_FANS_LOADER_RELOAD) :
		Task(level), _gid(gid) {
	}
protected:
	virtual void handle();
private:
	Ice::Long _gid;
};

//--------------------------------Config----------------------------------------
class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

}
}

#endif
