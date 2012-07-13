/*
 * =====================================================================================
 *
 *       Filename:  WapOnlineAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年04月22日 18时58分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#ifndef WAPONLINEADAPTER_H_
#define WAPONLINEADAPTER_H_

#include "WapOnline.h"
//#include <AdapterI.h>
#include "Ice/Ice.h"
#include "IceUtil/IceUtil.h"
#include "IceLogger.h"
#include "UtilCxx/src/Singleton.h"

namespace com {
namespace xiaonei {
namespace wap {
namespace adapter {

enum OnlineStat {
	WAP_ONLINE = 1
};

class WapOnlineAdapter : public MyUtil::Singleton<WapOnlineAdapter> {
public:
	WapOnlineAdapter() {
		Ice::PropertiesPtr properties = Ice::createProperties();
		properties->setProperty("Ice.Override.ConnectTimeout", "300");
		properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
		properties->setProperty("Ice.MessageSizeMax", "20480");
		Ice::InitializationData id;
		id.properties = properties;
		communicator_ = Ice::initialize(id);
		online_service_prx_ = OnlineServicePrx::checkedCast(communicator_->stringToProxy(endpoints())->ice_timeout(300));
	}

	virtual ~ WapOnlineAdapter() {
	}

	MyUtil::Int2IntMap getOnlineStatus(const MyUtil::IntSeq& ids);

private:

	std::string endpoints() {
		return "WapOnlineStatus:tcp -h 10.3.19.191 -p 10000";
	}

	Ice::CommunicatorPtr communicator_;
	OnlineServicePrx online_service_prx_;
};

};
};
};
};
#endif

