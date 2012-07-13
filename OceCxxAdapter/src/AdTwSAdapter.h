/*
 * AdTwSAdapter.h
 *
 *  Created on: 2010-9-14
 *      Author: ark
 */

#ifndef ADTWSADAPTER_H_
#define ADTWSADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdEngine.h"

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
	AdTwAdapter():server_index_(0) {
	    _managers.resize(cluster() > 0 ? cluster() : 1);
	    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	virtual ~AdTwAdapter() {}

xce::ad::AdResultMap GetAds(const string& userProfile, const string& referer, int rotate_index) {
	//xce::ad::AdResultMap GetAds(const string& userProfile,
	//			  ::Ice::Int count,const string& referer) {
  server_index_ = (server_index_+1)%5;
		return getManager(server_index_)->GetAds(userProfile, referer, rotate_index);
	}

	void Click(int uid,
				  const string& ip,
				  const string& text,
				  const string& referer,
				  const string& click_url) {
	  server_index_ = (server_index_+1)%5;
		getManager(server_index_)->Click(uid, ip, text, referer, click_url);
	}

	AdMatchManagerPrx getManager(int id);
	AdMatchManagerPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdTwS";
  }
  virtual size_t cluster() {
    return 5;
  }

  int server_index_;
  vector<AdMatchManagerPrx> _managersOneway;
  vector<AdMatchManagerPrx> _managers;
};



#endif /* ADTWADAPTER_H_ */
