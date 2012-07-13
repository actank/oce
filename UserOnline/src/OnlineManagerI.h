#ifndef __OnlineManagerI_h__
#define __OnlineManagerI_h__

#include <UserOnline.h>
#include "OnlineEvictor.h"
#include "TaskManager.h"
namespace mop
{
namespace hi
{
namespace svc
{
namespace online
{
const string FB_SLAVE = "fb_slave";

//using namespace mop::hi::svc::adapter;
class OnlineManagerI : virtual public OnlineManager,
virtual public MyUtil::Singleton<OnlineManagerI>
{
public:
	//OnlineManagerI(::Ice::Int , ::Ice::Int);
	
	~OnlineManagerI();
	
	void init(::Ice::Int evictStep, ::Ice::Int expStep);
	
	
    virtual void setStat(::Ice::Int, ::Ice::Int, bool,
			   const Ice::Current& = Ice::Current());

    virtual void setStats(const ::MyUtil::IntSeq&, ::Ice::Int, bool,
			const Ice::Current&);

	virtual ::Ice::Int size(::Ice::Int, const Ice::Current&);
	
	virtual ::Ice::Int getStat(::Ice::Int, const Ice::Current&);
    virtual IntSeq getStats(::Ice::Int, ::Ice::Int, ::Ice::Int, const Ice::Current&);
	
    virtual MyUtil::Int2IntMap checkStats(const MyUtil::IntSeq& userIds, const Ice::Current&);

    
    
    ActiveUserManagerPrx getActiveUserManager();
    string& getImOnlineDb();
    string& getOnlineDb();
    ActiveUserManagerPrx _activeUserManager;
    string _imOnlineDb;
    string _onlineDb;
private:
	OnlineEvictorPtr _sessions;
};


class KickTimeoutTask : public Task
{               
	public:         
		KickTimeoutTask(const OnlineEvictorPtr& sessions, int evictStep);
		virtual void handle();
	private:
		OnlineEvictorPtr _sessions;
		int _evictStep;
};

class UpdateOnlineStatTask : public Task
{
	protected:
		int _timeslot;
		bool _isAuto;
		OnlineEvictorPtr _evictor;
	public:
		UpdateOnlineStatTask(OnlineEvictorPtr evictor, int timeslot = 5, bool isAuto = false);

		virtual void handle();
};

}
}
}
}

#endif
