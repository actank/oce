#ifndef __SBJ_TOPIC_I__
#define __SBJ_TOPIC_I__

#include "SubjectObserver.h"
#include "Channel.h"
#include "AdapterI.h"
#include "util/cpp/IpAddr.hpp"

namespace MyUtil	{

// ======== SbjTopicI ==================================================
class SbjTopicI : virtual public SbjTopic,
		  public AdapterI,
		  virtual public IceUtil::Thread 
{
public:
    SbjTopicI(const string& topicName, const SbjSubscriberPtr subscriber, const string& endpoints, int interval=300, const MyUtil::ChannelPtr& channel = new MyUtil::OceChannel):
	    			_topicname(topicName),
	    			_endpoints(endpoints),
	    			_interval(interval),
            _channel(channel)
    {
	srand((unsigned)time(NULL));
	// 获取本地IP
	string localIP = MyUtil::IpAddr::inner();
	// 生成要注册subscriber的prx
	_icCore= _channelCore.getCommunicator();
	ostringstream _ename;
	_ename << "SubscriberRandom_" << rand();
	string adaptername=_ename.str();
	_ename << ".Endpoints";
	_icCore->getProperties()->setProperty(_ename.str(),"tcp -h " + localIP);
	Ice::ObjectAdapterPtr adapter=_icCore->createObjectAdapter(adaptername);
	adapter->activate();
	_subscriber=SbjSubscriberPrx::uncheckedCast(adapter->addWithUUID(subscriber)->ice_timeout(300));

	// 找到Observer的prx
	_ic = _channel->getCommunicator();
	std::string ep = string("SO@") + _endpoints;
	Ice::ObjectPrx object = _ic->stringToProxy(ep)->ice_timeout(1000);
	_proxy =  SubjectObserverPrx::uncheckedCast(object);

	// 运行
	start(65535).detach();
    }


    virtual void subscribe(const ::MyUtil::SbjSubscriberPrx&, const Ice::Current& = Ice::Current());

    virtual void unsubscribe(const ::MyUtil::SbjSubscriberPrx&,  const Ice::Current& = Ice::Current());

    virtual void run();
    
private:
    virtual std::string name() {
	    return _endpoints;
    }

    virtual std::string endpoints() {
	    return "@"+_endpoints;
    }

    virtual size_t cluster() {
	    return 0;
    }

    SubjectObserverPrx getProxy();
    
    string _topicname;				/* @breif 注册项的标志 */
    std::string _endpoints;			/* @brief 组件注册所在服务的endpoints */ 
    MyUtil::CoreChannel _channelCore;
    MyUtil::OceChannel _channelOce;
    Ice::CommunicatorPtr _icCore;			    
    Ice::CommunicatorPtr _ic;			    
    SbjSubscriberPrx _subscriber;		/* @brief 组件要注册的subscriber */
    SubjectObserverPrx _proxy;			/* @brief Observer的代理 */
    int _interval;				/* @breif 注册信息发送间隔(秒) */
    MyUtil::ChannelPtr _channel;
    
   
};

typedef IceInternal::Handle<SbjTopicI> SbjTopicIPtr;

}

#endif
