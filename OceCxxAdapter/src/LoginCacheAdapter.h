#ifndef LOGINCACHEADAPTER_H_
#define LOGINCACHEADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "Login.h"
namespace passport{
class AbstractLoginCacheAdapter : virtual public Ice::Object {
public:
	virtual void set(Ice::Int uid,const string& ticket,int second) = 0;
        virtual string id2Ticket(int userId) = 0;
	virtual ~AbstractLoginCacheAdapter(){};
};

class WapLoginCacheAdapter : virtual public AbstractLoginCacheAdapter, public MyUtil::ReplicatedClusterAdapterI<LoginCachePrx>,
	public MyUtil::Singleton<WapLoginCacheAdapter> {
public:
	WapLoginCacheAdapter() : MyUtil::ReplicatedClusterAdapterI<LoginCachePrx> ("ControllerWapLoginCache",120,300) {}
	virtual void initialize(){};
	virtual void set(Ice::Int uid,const string& ticket,int second);
        virtual string id2Ticket(int userId);
        virtual int ticket2Id(const string& ticket);
};

class WebLoginCacheAdapter : virtual public AbstractLoginCacheAdapter, public MyUtil::ReplicatedClusterAdapterI<LoginCachePrx>,
	public MyUtil::Singleton<WebLoginCacheAdapter> {
public:
	WebLoginCacheAdapter() : MyUtil::ReplicatedClusterAdapterI<LoginCachePrx> ("ControllerLoginCache",120,300){}
	virtual void initialize(){};
	virtual void set(Ice::Int uid,const string& ticket,int second);
        virtual string id2Ticket(int userId);
};

class MobileClientLoginCacheAdapter : virtual public AbstractLoginCacheAdapter, public MyUtil::ReplicatedClusterAdapterI<LoginCachePrx>,
	public MyUtil::Singleton<MobileClientLoginCacheAdapter> {
public:
	MobileClientLoginCacheAdapter() : MyUtil::ReplicatedClusterAdapterI<LoginCachePrx> ("ControllerMobileClientLoginCache",120,300){}
	virtual void initialize(){};
	virtual void set(Ice::Int uid,const string& ticket,int second);
        virtual string id2Ticket(int userId);
        virtual int ticket2Id(const string& ticket);
};

};
#endif /*LOGINCACHEADAPTER_H_*/
