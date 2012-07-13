/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintReplicaI.h
 *
 *    Description:  replica for HomeFootprint
 *
 *        Version:  1.0
 *        Created:  2011年09月13日 18时18分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __HOME_FOOTPRINT_REPLICE_I__
#define __HOME_FOOTPRINT_REPLICE_I__

#include "app/FootprintUnite/share/HomeFootprintBufReaderI.h"
#include "app/FootprintUnite/share/HomeFootprintReaderI.h"
#include "app/FootprintUnite/share/HomeUserVisitCountReaderI.h"

#include <FootprintUnite.h>
#include <IceUtil/IceUtil.h>
#include <TaskManager.h>
#include <boost/circular_buffer.hpp>

namespace xce 
{
namespace footprint
{
class HomeFootprintReplicaI : virtual public HomeFootprintReplica,
	public MyUtil::Singleton<HomeFootprintReplicaI> {
public:
	HomeFootprintReplicaI();
	~HomeFootprintReplicaI(){};
  virtual void setFootprint(const MyUtil::ObjectResultPtr& pData, int category, const Ice::Current& crt = Ice::Current());
  virtual void setUserVisitCount(const MyUtil::ObjectResultPtr& pData, const Ice::Current& crt = Ice::Current());
  virtual HomeInfoPtr getAll(long id, int begin, int nLimit, int tLimit = 0, const Ice::Current& crt = Ice::Current());
  virtual int getUserVisitCount(long uid, const Ice::Current& crt = Ice::Current());
  virtual int getSize(long id, const Ice::Current& crt = Ice::Current());

  virtual bool isValid(const Ice::Current& crt = Ice::Current());
  virtual void setValid(bool newState, const Ice::Current& crt = Ice::Current());

private:
  int getHomeFootprintCacheSize(int category) {
    if (category == FOOTPRINT_SET) {
      return CACHE_SIZE;
    } else if (category == FOOTPRINT_BUF_SET) {
      return CACHE_BUF_SIZE;
    }
    return 0;
  }

  HomeFootprintSetPtr getFootprintSet(long id);
  HomeFootprintSetPtr getFootprintBufSet(long id);
  UserVisitCountInfoPtr locateUserVisitCount(long id);
  bool isValid_;
  IceUtil::RWRecMutex validMutex_;
};

class HomeFootprintBufReplicaReaderI : public HomeFootprintBufReaderI, public Singleton<HomeFootprintBufReplicaReaderI> {
public:
  virtual void setServerStatus() {
    HomeFootprintReplicaI::instance().setValid(false);
  }
  virtual string getIdentifier() {
    return "HFBRR";
  }
};

class HomeFootprintReplicaReaderI : public HomeFootprintReaderI, public Singleton<HomeFootprintReplicaReaderI> {
public:
  virtual void setServerStatus() {
    HomeFootprintReplicaI::instance().setValid(false);
  }
  virtual string getIdentifier() {
    return "HFRR";
  }
};

class HomeUserVisitCountReplicaReaderI : public HomeUserVisitCountReaderI, public Singleton<HomeUserVisitCountReplicaReaderI> {
public:
  virtual void setServerStatus() {
    HomeFootprintReplicaI::instance().setValid(false);
  }
  virtual string getIdentifier() {
    return "HUVCRR";
  }
};

class HomeFootprintFactoryI : public MyUtil::ServantFactory {
public:
  virtual Ice::ObjectPtr create(long id);
};

class HomeFootprintBufFactoryI : public MyUtil::ServantFactory {
public:
  virtual Ice::ObjectPtr create(long id);
};

class HomeUserVisitCountFactoryI : public MyUtil::ServantFactory {
public:
  Ice::ObjectPtr create(long id);
};

}
}

#endif
