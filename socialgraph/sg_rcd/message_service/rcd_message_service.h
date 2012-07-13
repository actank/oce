#ifndef _SOCIALGRAPH_RCD_MESSAGE_SERVICE_H_
#define _SOCIALGRAPH_RCD_MESSAGE_SERVICE_H

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "QueryRunner.h"
#include "SocialGraphRcdMessageService.h"

namespace xce {
namespace socialgraph {

typedef std::vector<unsigned char> BinaryData;

struct DbUtil {
  static void AddSgBlock(Ice::Long source, Ice::Long block, const std::string& biz = "NON");
};

class RcdMessageServiceImpl : virtual public RcdMessageService, virtual public MyUtil::Singleton<RcdMessageServiceImpl> {
 public:
  virtual void addSgBlock(::Ice::Long, ::Ice::Long, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
  virtual void removeRcd(::Ice::Long, ::Ice::Long, const ::Ice::Current& = ::Ice::Current());
  virtual void removeBizRcd(::Ice::Long, ::Ice::Long, const ::std::string&,const ::Ice::Current& = ::Ice::Current());
	virtual void removeBizCache(::Ice::Long, ::Ice::Long, const ::std::string&, const ::Ice::Current& = ::Ice::Current());
 private:
  RcdMessageServiceImpl();
  friend class MyUtil::Singleton<RcdMessageServiceImpl>;
};


}}

#endif

