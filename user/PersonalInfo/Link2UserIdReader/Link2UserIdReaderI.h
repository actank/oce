#ifndef __XCE_PERSONALINFO_LINK2USERIDREADERI_H__
#define __XCE_PERSONALINFO_LINK2USERIDREADERI_H__

#include "PersonalInfo.h"
#include "../share/LinkViewDataI.cpp"
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include "TaskManager.h"
#include "ObjectCacheI.h"
#include "ServiceI.h"

namespace xce {
namespace personalinfo {

//用于setData的TaskManager配置
const int TASK_LEVEL_FILL = 1;
const int CORE_THREAD_FILL = 1;
const int MAX_THREAD_FILL = 10;

//ObjectCache配置
const int CACHE_CATEGORY = 0;
const string CACHE_ALIAS = "LU";

//服务配置
const string SERVICE_IDENTITY = "LUR";
const string SERVER_CONFIG_PREFIX = "Link2UserIdReader";
const string CONTROLLER_ENDPOINT = "ControllerLink2UserIdReader";

//Link2UserIdReaderI
//--------------------------------------------------------------
class Link2UserIdReaderI: virtual public Link2UserIdReader, public MyUtil::Singleton<Link2UserIdReaderI> {
public:
  virtual int find(const string& link, const Ice::Current& = Ice::Current());
  virtual int locate(const string& link, const Ice::Current& = Ice::Current());
  MyUtil::Str2StrMap getLinkView(const string& link, const Ice::Current& = Ice::Current());
  MyUtil::Str2StrMap getLinkViewSync(const string& link, const Ice::Current& = Ice::Current());

  virtual void add(const string& link, Ice::Int userId, const Ice::Current& = Ice::Current());
  virtual void remove(const string& link, const Ice::Current& = Ice::Current());
  virtual void setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& = Ice::Current());
  virtual bool isValid(const Ice::Current& = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

private:
};

//FillTask
//--------------------------------------------------------------
template<class T, class P>
class FillTask : virtual public MyUtil::Task {
public:
  FillTask(const MyUtil::ObjectResultPtr& userData);
  virtual void handle();
private:
  MyUtil::ObjectResultPtr _userData;
};

}
}
#endif
