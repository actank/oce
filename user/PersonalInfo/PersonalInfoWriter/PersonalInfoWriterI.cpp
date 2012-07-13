#include "PersonalInfoWriterI.h"
#include "LinkViewReaderAdapter.h"
#include "Link2UserIdReaderAdapter.h"

#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;
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
  int cfg_status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(SERVER_CONFIG_PREFIX + ".Status", 1);

  //注册服务
  service.getAdapter()->add(&PersonalInfoWriterI::instance(), service.createIdentity(SERVICE_IDENTITY, ""));

  //HA
  ServerStateSubscriber::instance().initialize(CONTROLLER_ENDPOINT, &PersonalInfoWriterI::instance(), cfg_mod, cfg_interval);
  {
    ServerStateSubscriber::instance().setStatus(cfg_status);
    std::ostringstream os;
    os << "MyUtil::initialize set status : " << cfg_status;
    MCE_WARN(os.str());
  }

  //监控
  ServiceMonitorManager::instance().start();
}

//PersonalInfoWriterI
//--------------------------------------------------------------
PersonalInfoWriterI::PersonalInfoWriterI() {
  _tempMutexManager = new xce::tempmutex::TempMutexManager();
  _factory = new LinkViewDataFactoryI();
}

void PersonalInfoWriterI::createPersonalInfo(const MyUtil::Str2StrMap& props, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

  int userId;
  Str2StrMap::const_iterator it = props.find(CPIID);
  if(it != props.end()) {
    userId = boost::lexical_cast<int>(it->second);
  } else {
    MCE_WARN("PersonalInfoWriterI::createPersonalInfo Exception::Can not find " << CPIID << " in props");
    return;
  }
  try {
    xce::tempmutex::Lock lock(_tempMutexManager, userId);
    QueryRunner(DB_PERSONAL, CDbWServer).execute(Statement::buildInsert(TABLE_PERSONAL, props));
    ostringstream idMsg;
    idMsg << "userId=" << userId;
    MCE_INFO("PersonalInfoWriterI::createPersonalInfo Loading " << userId);
    
    Ice::ObjectPtr data = _factory->create(userId, CDbWServer);
    if(data) {
      LinkDataIPtr linkData = new LinkDataI(LinkViewDataIPtr::dynamicCast(data));
      LinkViewReaderAdapter::instance().setData(userId, data);
      Link2UserIdReaderAdapter::instance().setData(userId, linkData);
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("[PersonalInfoWriterI::createPersonalInfo] Ice::Exception : " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[PersonalInfoWriterI::createPersonalInfo] std::exception : " << e.what());
  } catch (...) {
    MCE_WARN("[PersonalInfoWriterI::createPersonalInfo] UNKNOWN exception");
  }
}

void PersonalInfoWriterI::setPersonalInfo(int userId, const MyUtil::Str2StrMap& props, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  //去掉map中的用户ID，防止被错误修改
  const_cast<Str2StrMap&>(props).erase(CPIID);

  Str2StrMap filter;
  filter[CPIID] = boost::lexical_cast<string>(userId);

  try {
    xce::tempmutex::Lock lock(_tempMutexManager, userId);
    QueryRunner(DB_PERSONAL, CDbWServer).execute(Statement::buildUpdate(TABLE_PERSONAL, filter,  props)); 

    LinkViewDataIPtr linkViewDataOld = new LinkViewDataI;
    LinkViewDataIPtr linkViewDataNew = new LinkViewDataI;
    linkViewDataOld->setProps(LinkViewReaderAdapter::instance().getLinkView(userId));
    linkViewDataNew->setProps(props);
    if(linkViewDataNew->link != linkViewDataOld->link) {
      MCE_DEBUG("[PersonalInfoWriterI::setPersonalInfo] link diff : new="
        << linkViewDataNew->link <<", old=" << linkViewDataOld->link);
      LinkViewReaderAdapter::instance().setLinkView(userId, props);
      Link2UserIdReaderAdapter::instance().remove(linkViewDataOld->link);
      Link2UserIdReaderAdapter::instance().add(linkViewDataNew->link, userId);
    } else if (linkViewDataNew->linkStatus != linkViewDataOld->linkStatus) {
      MCE_DEBUG("[PersonalInfoWriterI::setPersonalInfo] linkStatus diff : new="
        << linkViewDataNew->linkStatus <<", old=" << linkViewDataOld->linkStatus);
      LinkViewReaderAdapter::instance().setLinkView(userId, props);
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("[PersonalInfoWriterI::setPersonalInfo] Ice::Exception : " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[PersonalInfoWriterI::setPersonalInfo] std::exception : " << e.what());
  } catch (...) {
    MCE_WARN("[PersonalInfoWriterI::setPersonalInfo] UNKNOWN exception");
  }
}

void PersonalInfoWriterI::reloadLinkView(int userId, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  try {
    xce::tempmutex::Lock lock(_tempMutexManager, userId);
    Ice::ObjectPtr data = _factory->create(userId, CDbRServer);
    if(data) {
      LinkDataIPtr linkData = new LinkDataI(LinkViewDataIPtr::dynamicCast(data)); 
      LinkViewReaderAdapter::instance().setData(userId, data);
      Link2UserIdReaderAdapter::instance().setData(userId, linkData);
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("[PersonalInfoWriterI::reloadLinkView] Ice::Exception : " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[PersonalInfoWriterI::reloadLinkView] std::exception : " << e.what());
  } catch (...) {
    MCE_WARN("[PersonalInfoWriterI::reloadLinkView] UNKNOWN exception");
  }
}

bool PersonalInfoWriterI::isValid(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

  return ServerStateSubscriber::instance().getStatus();
}

void PersonalInfoWriterI::setValid(bool valid, const Ice::Current& current) {
  ostringstream oss;
  oss << "valid=" << valid;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  ServerStateSubscriber::instance().setStatus((int) valid);
}
