#include "Link2UserIdReaderI.h"
#include "PersonalInfoWriterAdapter.h"
#include "LinkViewReaderAdapter.h"

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
    service.registerObjectCache(CACHE_CATEGORY, CACHE_ALIAS, new Link2UserIdDataFactoryI, false);
  } else {
    service.registerObjectCache(CACHE_CATEGORY, CACHE_ALIAS, new Link2UserIdDataFactoryI, true);
  }

  //注册服务
  service.getAdapter()->add(&Link2UserIdReaderI::instance(), service.createIdentity(SERVICE_IDENTITY, ""));

  //用来setdata的task
  TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(CORE_THREAD_FILL, MAX_THREAD_FILL));

  //HA
  ServerStateSubscriber::instance().initialize(CONTROLLER_ENDPOINT, &Link2UserIdReaderI::instance(), cfg_mod, cfg_interval);
  {
    ServerStateSubscriber::instance().setStatus(cfg_status);
    std::ostringstream os;
    os << "MyUtil::initialize set status : " << cfg_status;
    MCE_WARN(os.str());
  }

  //监控
  ServiceMonitorManager::instance().start();
}

//Link2UserIdReaderI
//--------------------------------------------------------------
int Link2UserIdReaderI::find(const string& link, const Ice::Current& current) {
  ostringstream oss;
  oss << "link=" << link;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  int result = 0;
  if(link != "") {
    int bucketId = BucketTool::getBucketId(link); 
    Link2UserIdDataIPtr dataI = ServiceI::instance().findObject<Link2UserIdDataIPtr> (CACHE_CATEGORY, bucketId);
    if(dataI) {
      result = dataI->get(link);
    }
  }

  return result;
}

int Link2UserIdReaderI::locate(const string& link, const Ice::Current& current) {
  ostringstream oss;
  oss << "link=" << link;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  int result = 0;
  if(link != "") {
    int bucketId = BucketTool::getBucketId(link);
    MCE_DEBUG("Link2UserIdReaderI::locate link=" << link << ", bucketId=" << bucketId);
    Link2UserIdDataIPtr dataI = ServiceI::instance().findObject<Link2UserIdDataIPtr> (CACHE_CATEGORY, bucketId);
    if (!dataI || dataI->count(link) == 0) {
      LinkDataIPtr linkData = new LinkDataI;
      com::xiaonei::xce::Statement sql;
      MCE_INFO("Link2UserIdReaderI::locate load from DB, link=" << link << ", bucketId=" << bucketId);
      sql << "SELECT " << CPIID << ", " << CPILINK << " FROM " << TABLE_PERSONAL << " WHERE " << CPILINK << " = '" << link << "'";
      LinkDataResultHandlerI handler(linkData);
      size_t count = com::xiaonei::xce::QueryRunner(DB_PERSONAL, com::xiaonei::xce::CDbRServer).query(sql, handler);
      if (count != 0) {
        dataI = ServiceI::instance().locateObject<Link2UserIdDataIPtr> (CACHE_CATEGORY, bucketId);
        dataI->add(linkData->link, linkData->userId);
        result = linkData->userId;
        if (isValid()) {
          PersonalInfoWriterAdapter::instance().reloadLinkView(linkData->userId);
        }
      } else {
        MCE_WARN("Link2UserIdReaderI::locate Exception NoSuchLinkData, link = " << link);
      }
    } else {
      result = dataI->get(link);
    }
  }

  return result;
}

MyUtil::Str2StrMap Link2UserIdReaderI::getLinkView(const string& link, const Ice::Current& current) {
  ostringstream oss;
  oss << "link=" << link;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

  MyUtil::Str2StrMap result;
  if(link != "") {
    int userId = find(link);
    if(userId) {
      result = LinkViewReaderAdapter::instance().getLinkViewSync(userId);
    } else {
      MCE_INFO("Link2UserIdReaderI::getLinkViewSync link not found : [" << link << "]");
    }
  }
  return result;
}

MyUtil::Str2StrMap Link2UserIdReaderI::getLinkViewSync(const string& link, const Ice::Current& current) {
  ostringstream oss;
  oss << "link=" << link;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  MyUtil::Str2StrMap result;
  if(link != "") {
    int userId = locate(link);
    if(userId) {
      try {
        result = LinkViewReaderAdapter::instance().getLinkViewSync(userId);
      } catch (Ice::Exception& e) {
        MCE_WARN("Link2UserIdReaderI::getLinkViewSync Ice::Exception:" << e.what());
      } catch (std::exception& e) {
        MCE_WARN("Link2UserIdReaderI::getLinkViewSync std::exception:" << e.what());
      }
    } else {
      MCE_INFO("Link2UserIdReaderI::getLinkViewSync link not found : [" << link << "]");
    }
  }

  return result;
}

void Link2UserIdReaderI::add(const string& link, Ice::Int userId, const Ice::Current& current) {
  ostringstream oss;
  oss << "link=" << link << ", userId=" << userId;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if(link != "" && userId > 0) {
    int bucketId = BucketTool::getBucketId(link);
    Link2UserIdDataIPtr dataI = ServiceI::instance().locateObject<Link2UserIdDataIPtr> (CACHE_CATEGORY, bucketId);
    dataI->add(link, userId);
  }
}

void Link2UserIdReaderI::remove(const string& link, const Ice::Current& current) {
  ostringstream oss;
  oss << "link=" << link;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  if(link != "") {
    int bucketId = BucketTool::getBucketId(link);
    Link2UserIdDataIPtr dataI = ServiceI::instance().findObject<Link2UserIdDataIPtr> (CACHE_CATEGORY, bucketId);
    if(dataI) {
      dataI->remove(link);
      if(dataI->size() == 0) {
        ServiceI::instance().removeObject(CACHE_CATEGORY, bucketId);
      }
    }
  }
}

void Link2UserIdReaderI::setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& current) {
  ostringstream oss;
  oss << "size=" << userData->data.size();
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  TaskManager::instance().execute(new FillTask<LinkDataIPtr, LinkDataI>(userData));
}

bool Link2UserIdReaderI::isValid(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

  return ServerStateSubscriber::instance().getStatus();
}

void Link2UserIdReaderI::setValid(bool valid, const Ice::Current& current) {
  ostringstream oss;
  oss << "valid=" << valid;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

  ServerStateSubscriber::instance().setStatus((int) valid);
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
  for(MyUtil::ObjectMap::iterator it = _userData->data.begin(); it != _userData->data.end(); ++it) {
    T linkData = new P;
    linkData->parse(it->second);
    if(linkData->link != "NULL") {
      int bucketId = BucketTool::getBucketId(linkData->link);
      Link2UserIdDataIPtr dataI  = ServiceI::instance().locateObject<Link2UserIdDataIPtr> (CACHE_CATEGORY, bucketId);
      dataI->add(linkData->link, linkData->userId);
      MCE_DEBUG("FillTask::handle set data link=" << linkData->link << ", bucketId=" << bucketId << ", bucketSize=" << dataI->size());
    }
  }
}
