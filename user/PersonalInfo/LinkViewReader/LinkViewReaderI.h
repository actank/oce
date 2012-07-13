#ifndef __XCE_PERSONALINFO_LINKVIEWREADERI_H__
#define __XCE_PERSONALINFO_LINKVIEWREADERI_H__

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
const string CACHE_ALIAS = "LV";

//服务配置
const string SERVICE_IDENTITY = "LVR";
const string SERVER_CONFIG_PREFIX = "LinkViewReader";
const string CONTROLLER_ENDPOINT = "ControllerLinkViewReader";

//LinkViewReaderI
//--------------------------------------------------------------
class LinkViewReaderI: virtual public LinkViewReader, public MyUtil::Singleton<LinkViewReaderI> {
public:
  virtual MyUtil::Str2StrMap getLinkView(Ice::Int userId, const Ice::Current& = Ice::Current());
  virtual MyUtil::Str2StrMap getLinkViewSync(Ice::Int userId, const Ice::Current& = Ice::Current());

  virtual void setLinkView(Ice::Int userId, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
  virtual void setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& = Ice::Current());
  virtual bool isValid(const Ice::Current& = Ice::Current());
  virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

private:
  LinkViewDataIPtr _getLinkViewData(int userId);
  LinkViewDataIPtr _getLinkViewDataSync(int userId);
};

//FillTask
//--------------------------------------------------------------
template <class T,class P>
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
