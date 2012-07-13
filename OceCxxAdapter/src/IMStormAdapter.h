#ifndef _IMSTORM_ADAPTER_H_
#define _IMSTORM_ADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "IMStorm.h"
#include "TaskManager.h"

namespace xce {
namespace talk{
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;

class IMStormAdapter : public MyUtil::ReplicatedClusterAdapterI<IMStormPrx>, public MyUtil::Singleton<IMStormAdapter> {
public:
	IMStormAdapter() : MyUtil::ReplicatedClusterAdapterI<IMStormPrx> ("ControllerPIMStorm",120,300,new TalkChannel){
	}
	void UserOnlineStatChange(int userid, int onlinestat, int onlinetype, const MyUtil::IntSeq& notifybuddys);
	void notify(const string& name, const Str2StrMap& context, int servercount, int channel);//发布者调用此接口发布消息
	void subscribe(const string& name, const ObserverPtr& o, int phoneservercount, int phoneserverindex);//消息接收者的订阅接口
	void unsubscribe(const string& name, const ObserverPtr& o);//消息接收者取消订阅的接口
	void deliver(const MessageSeq& msgs);


	void ensureSubscription();

	IMStormPrx getManager(int id);
	IMStormPrx getManagerOneway(int id);


protected:
	virtual void initialize();
	virtual string name() {
		return "PIMStorm";
	}
	virtual string endpoints() {//注意，此处写P 开头的原因是因为原来的PTalkProxy的endpoint 是P开头的，逻辑代码里面有相应判断，所以以P开头，做兼容。
		return "@PIMStorm";
	}
	virtual size_t cluster() {
		return 5;
	}

private:
	IceUtil::Mutex _mutex;
	IMStormPrx _managerOneway;
	Ice::ObjectAdapterPtr _adapter;
	typedef map<string, map<Ice::ObjectPtr, Ice::ObjectPrx> > ObserverMap;
	ObserverMap _observers;

	vector<IMStormPrx> _managersOneway;
	vector<IMStormPrx> _managers;

};

class SubscriptionGuard: public MyUtil::Timer {//隔一段时间重复订阅。 防止Storm服务挂掉了，客户端可以自己再重新订阅
public:
	SubscriptionGuard() :
		MyUtil::Timer((20 * 60 * 1000 )){
		;
	}
	virtual void handle();
};

};
};

#endif /*WEBEVENTSADAPTER_H_*/
