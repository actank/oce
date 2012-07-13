/*
 * =====================================================================================
 *
 *       Filename:  FansPagePreloader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2010 04:53:40 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __FANSPAGE_PRELOAERI_H__
#define __FANSPAGE_PRELOAERI_H__

#include <map>
#include <Singleton.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include "FansPage.h"

namespace xce {
namespace buddy {

const int PRX_DATAQUEUE_MAX_SIZE = 100;

class FansPagePreloaderI : virtual public MyUtil::Singleton<FansPagePreloaderI> {
private:
  friend class MyUtil::Singleton<FansPagePreloaderI>;
};

class FansPagePreloaderFactory: virtual public xce::generalpreloader::IdListFactory, virtual public MyUtil::Singleton<FansPagePreloaderFactory>  {
protected:
  friend class MyUtil::Singleton<FansPagePreloaderFactory>;
public:
  virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

class ServiceSchemaI: public MyUtil::Descriptor {
public:
  virtual void configure(const Ice::PropertiesPtr& props);
};

}
}
#endif
