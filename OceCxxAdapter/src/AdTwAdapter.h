/*
 * AdTwAdapter.h
 *
 *  Created on: 2010-9-14
 *      Author: ark
 */

#ifndef ADTWADAPTER_H_
#define ADTWADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdMatch.h"

using namespace MyUtil;
using namespace xce::ad;

namespace MyUtil {

class AdT19Channel : public Channel {
public:
  AdT19Channel() {
    Ice::PropertiesPtr properties = Ice::createProperties();
    properties->setProperty("Ice.Override.ConnectTimeout", "150");
    properties->setProperty("IceGrid.InstanceName", "Ad");
    
    properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
    properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");
    
    properties->setProperty("Ice.Default.Host", IpAddr::inner());
    
    properties->setProperty(
                  "Ice.Default.Locator",
                  "Ad/Locator:default -h 10.3.18.186 -p 14800");
    Ice::InitializationData id;
    id.properties = properties;
    _communicator = Ice::initialize(id);
    
    _query = IceGrid::QueryPrx::uncheckedCast(_communicator->stringToProxy("Ad/Query"));
  }
};

}


//class AdTwAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::AdT19Channel, AdTwAdapter> {
class AdTwAdapter: public MyUtil::AdapterI, public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdTwAdapter> {
public:
	AdTwAdapter() {
	    _managers.resize(cluster() > 0 ? cluster() : 1);
	    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	virtual ~AdTwAdapter() {}

	xce::ad::AdInfoSeq GetAds(const string& userProfile,
				  ::Ice::Int count,const string& referer) {
		return getManager(0)->GetAds(userProfile, count, referer);
	}

	void Click(int uid,
				  const string& ip,
				  const string& text,
				  const string& referer,
				  const string& click_url) {
		getManager(0)->Click(uid, ip, text, referer, click_url);
	}

	AdMatchPrx getManager(int id);
	AdMatchPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdTw";
  }
  virtual size_t cluster() {
    return 1;
  }

  vector<AdMatchPrx> _managersOneway;
  vector<AdMatchPrx> _managers;
};



#endif /* ADTWADAPTER_H_ */
