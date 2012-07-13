#include "LinkViewReaderI.h"
#include "PersonalInfoWriterAdapter.h"

#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::personalinfo;
using namespace xce::adapter::personalinfo;

//初始化
//--------------------------------------------------------------
void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  //读取配置
  string cfg_type = service.getCommunicator()->getProperties()->getPropertyWithDefault(SERVER_CONFIG_PREFIX + ".ObjectCacheType", "Retention");
  int cfg_mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(SERVER_CONFIG_PREFIX + ".Mod", 0);
  int cfg_interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(SERVER_CONFIG_PREFIX + ".Interval", 5);
  int cfg_status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(SERVER_CONFIG_PREFIX + ".Status", 0);

  //初始化缓存
  if (cfg_type == "Retention") {
    service.registerObjectCache(CACHE_CATEGORY, CACHE_ALIAS, new LinkViewDataFactoryI, false);
  } else {
    service.registerObjectCache(CACHE_CATEGORY, CACHE_ALIAS, new LinkViewDataFactoryI, true);
  }

  //注册服务
  service.getAdapter()->add(&LinkViewReaderI::instance(), service.createIdentity(SERVICE_IDENTITY, ""));

  //用来setdata的task
  TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(CORE_THREAD_FILL, MAX_THREAD_FILL));

  //HA
  ServerStateSubscriber::instance().initialize(CONTROLLER_ENDPOINT, &LinkViewReaderI::instance(), cfg_mod, cfg_interval);
  {
    ServerStateSubscriber::instance().setStatus(cfg_status);
    std::ostringstream os;
    os << "MyUtil::initialize set status : " << cfg_status;
    MCE_WARN(os.str());
  }

  //监控
  ServiceMonitorManager::instance().start();
}

//LinkViewReaderI
//--------------------------------------------------------------
MyUtil::Str2StrMap LinkViewReaderI::getLinkView(Ice::Int userId, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

  MyUtil::Str2StrMap result;
  LinkViewDataIPtr data = _getLinkViewData(userId);
  if(data) {
    result = data->getProps();
  }

  return result;
}

MyUtil::Str2StrMap LinkViewReaderI::getLinkViewSync(Ice::Int userId, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  MyUtil::Str2StrMap result;
  LinkViewDataIPtr data = _getLinkViewDataSync(userId);
  if(data) {
    result = data->getProps();
  }

  return result;
}

void LinkViewReaderI::setLinkView(Ice::Int userId, const MyUtil::Str2StrMap& props, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  LinkViewDataIPtr data = _getLinkViewDataSync(userId);
  if(data) {
    data->setProps(props);
  }
}

void LinkViewReaderI::setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& current) {
  ostringstream oss;
  oss << "size=" << userData->data.size();
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  TaskManager::instance().execute(new FillTask<LinkViewDataIPtr, LinkViewDataI>(userData));
}

bool LinkViewReaderI::isValid(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

  return ServerStateSubscriber::instance().getStatus();
}

void LinkViewReaderI::setValid(bool valid, const Ice::Current& current) {
  ostringstream oss;
  oss << "valid=" << valid;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  ServerStateSubscriber::instance().setStatus((int) valid);
}

LinkViewDataIPtr LinkViewReaderI::_getLinkViewData(int userId) {
  LinkViewDataIPtr dataI = NULL;
  dataI = ServiceI::instance().findObject<LinkViewDataIPtr> (CACHE_CATEGORY, userId);
  if(!dataI) {
    PersonalInfoWriterAdapter::instance().reloadLinkView(userId);
  }
  return dataI;
}

LinkViewDataIPtr LinkViewReaderI::_getLinkViewDataSync(int userId) {
  LinkViewDataIPtr result = NULL;
  try {
    result = ServiceI::instance().findObject<LinkViewDataIPtr> (CACHE_CATEGORY, userId);
    if (!result) {
      result = ServiceI::instance().locateObject<LinkViewDataIPtr> (CACHE_CATEGORY, userId);
      if (isValid()) {
        PersonalInfoWriterAdapter::instance().reloadLinkView(userId);
      }
    }
  } catch (const MyUtil::NoSuchObjectException& e) {
    MCE_WARN("LinkViewReaderI::_getLinkViewDataSync NoSuchLinkViewDataException [" << userId << "]");
    throw NoSuchObjectException();
  }

  return result;
}

//FillTask
//--------------------------------------------------------------
template<class T, class P>
FillTask<T, P>::FillTask(const ::MyUtil::ObjectResultPtr& userData) :
    _userData(userData) {
}

template<class T, class P>
void FillTask<T, P>::handle() {
  MCE_INFO("FillTask::handle set data size = " << _userData->data.size());
  MyUtil::ObjectMap result;
  for(MyUtil::ObjectMap::iterator it = _userData->data.begin();it != _userData->data.end();++it)
  {
    T obj = new P;
    obj->parse(it->second);
    result.insert(make_pair(it->first, obj));
  }
  MyUtil::ServiceI::instance().getObjectCache()->addObjects(CACHE_CATEGORY, result);
}
