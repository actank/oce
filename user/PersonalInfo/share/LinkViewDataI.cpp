#include "LinkViewDataI.h"

using namespace xce::personalinfo;

//LinkViewDataI
//LinkViewReader服务缓存的数据类型，也是该服务的Preloader传输的数据类型
//--------------------------------------------------------------
void LinkViewDataI::setProps(const MyUtil::Str2StrMap& props) {
  IceUtil::Mutex::Lock lock(_mutex);
  for (MyUtil::Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it) {
    if (it->first == CPILINKSTATUS) {
      linkStatus =  boost::lexical_cast<int>((*it).second);
    } else if (it->first == CPILINK) {
      link = (*it).second;
    }
  }
}

MyUtil::Str2StrMap LinkViewDataI::getProps() {
  MyUtil::Str2StrMap result;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    result[CPIID.c_str()] = boost::lexical_cast<string>(userId);
    result[CPILINKSTATUS.c_str()] = boost::lexical_cast<string>(linkStatus);
    result[CPILINK.c_str()] = link;
  }
  return result;
}

void LinkViewDataI::parse(Ice::ObjectPtr& obj) {
  LinkViewDataPtr data = LinkViewDataPtr::dynamicCast(obj);
  userId = data->userId;
  linkStatus = data->linkStatus;
  link = data->link;
}

void LinkViewDataI::parse(mysqlpp::Row& row) {
  userId = (int) row[CPIID.c_str()];
  linkStatus = (int) row[CPILINKSTATUS.c_str()];
  link = (std::string) row[CPILINK.c_str()];
}

//LinkData
//Link2UserIdReaderPreloader传输的数据类型
//--------------------------------------------------------------
LinkDataI::LinkDataI() {
}

LinkDataI::LinkDataI(const LinkViewDataIPtr linkViewData) {
  userId = linkViewData->userId;
  link = linkViewData->link;
}

void LinkDataI::setProps(const MyUtil::Str2StrMap& props) {
  IceUtil::Mutex::Lock lock(_mutex);
  for (MyUtil::Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it) {
    if (it->first == CPILINK) {
      link = it->second;
    }
  }
}

MyUtil::Str2StrMap LinkDataI::getProps() {
  MyUtil::Str2StrMap result;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    result[CPIID.c_str()] = boost::lexical_cast<string>(userId);
    result[CPILINK.c_str()] = link;
  }
  return result;
}

void LinkDataI::parse(Ice::ObjectPtr& obj) {
  LinkDataPtr data = LinkDataPtr::dynamicCast(obj);
  userId = data->userId;
  link = data->link;
}

void LinkDataI::parse(mysqlpp::Row& row) {
  userId = (int) row[CPIID.c_str()];
  link = (std::string) row[CPILINK.c_str()];
}

//Link2UserIdDataI
//Link2UserIdReader服务缓存的数据类型
//--------------------------------------------------------------
int Link2UserIdDataI::get(const string& link) {
  int result = 0;
  IceUtil::RWRecMutex::RLock lock(_mutex);
  MyUtil::Str2IntMap::iterator it = data.find(link);
  if(it != data.end()) {
    result = it->second;
  }
  return result;
}

void Link2UserIdDataI::add(const string& link, int userId) {
  IceUtil::RWRecMutex::WLock lock(_mutex);
  data[link] = userId;
}

void Link2UserIdDataI::remove(const string& link) {
  IceUtil::RWRecMutex::WLock lock(_mutex);
  data.erase(link);
}

int Link2UserIdDataI::count(const string& link) {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  return data.count(link);
}

int Link2UserIdDataI::size() {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  return data.size();
}

//LinkViewDataFactoryI
//--------------------------------------------------------------
Ice::ObjectPtr LinkViewDataFactoryI::create(Ice::Int userId, int wrflag) {
  LinkViewDataIPtr result = new LinkViewDataI;
  com::xiaonei::xce::Statement sql;
  sql << "SELECT " << CPIID << ", " << CPILINKSTATUS << ", " << CPILINK << " FROM " << TABLE_PERSONAL << " WHERE id = '" << userId << "'";
  LinkViewDataResultHandlerI handler(result);
  size_t count = com::xiaonei::xce::QueryRunner(DB_PERSONAL, wrflag).query(sql, handler);
  if (count == 0 | userId != result->userId) {
    MCE_WARN("LinkViewDataFactoryI::create, Exception NoSuchLinkViewData, userId = " << userId);
    return NULL;
  }
  return result;
}

Ice::ObjectPtr LinkViewDataFactoryI::create(Ice::Int userId) {
  return create(userId, com::xiaonei::xce::CDbRServer);
}

//LinkDataFactoryI
//--------------------------------------------------------------
Ice::ObjectPtr LinkDataFactoryI::create(Ice::Int userId) {
  LinkDataIPtr result = new LinkDataI;
  com::xiaonei::xce::Statement sql;
  sql << "SELECT " << CPIID << ", " << CPILINK << " FROM " << TABLE_PERSONAL << " WHERE id = '" << userId << "'";
  LinkDataResultHandlerI handler(result);
  size_t count = com::xiaonei::xce::QueryRunner(DB_PERSONAL, com::xiaonei::xce::CDbRServer).query(sql, handler);
  if (count == 0 | userId != result->userId) {
    MCE_WARN("LinkDataFactoryI::create, Exception NoSuchLinkData, userId = " << userId);
    return NULL;
  }
  return result;
}

//Link2UserIdDataFactoryI
//--------------------------------------------------------------
Ice::ObjectPtr Link2UserIdDataFactoryI::create(Ice::Int bucketId) {
  Link2UserIdDataIPtr result = new Link2UserIdDataI;
  return result;
}

//LinkViewDataResultHandlerI
//--------------------------------------------------------------
LinkViewDataResultHandlerI::LinkViewDataResultHandlerI(const LinkViewDataIPtr& result) :
    _result(result) {
}
bool LinkViewDataResultHandlerI::handle(mysqlpp::Row& row) const {
  _result->userId = (int) row[CPIID.c_str()];
  _result->linkStatus = (int) row[CPILINKSTATUS.c_str()];
  _result->link = (std::string) row[CPILINK.c_str()];
  return true;
}

//LinkDataResultHandlerI
//--------------------------------------------------------------
LinkDataResultHandlerI::LinkDataResultHandlerI(const LinkDataIPtr& result) :
    _result(result) {
}
bool LinkDataResultHandlerI::handle(mysqlpp::Row& row) const {
  _result->userId = (int) row[CPIID.c_str()];
  _result->link = (std::string) row[CPILINK.c_str()];
  return true;
}

//分桶存储的哈希函数
//---------------------------------------------------------------
int BucketTool::getBucketId(const string& link) {
  unsigned long h = 0, g;
  const char *arKey = link.c_str();
  const char *arEnd = arKey + link.length();
  while(arKey < arEnd) {
    h = (h << 4) + *arKey++;
    if ((g = (h & 0xF0000000))) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h % MAX_BUCKET;
}
