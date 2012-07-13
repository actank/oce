#include "rcd_message_service.h"

#include <sstream>
#include <boost/foreach.hpp>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"
#include "util/cpp/TimeCost.h"

//#include "socialgraph/socialgraphproto/item.pb.h"
#include "item.pb.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"

void MyUtil::initialize() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
  using namespace xce::socialgraph;
  using namespace xce::log;
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&RcdMessageServiceImpl::instance(), service.createIdentity("SGRMS", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SocialGraphRcdMessageService.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SocialGraphRcdMessageService.Interval", 5);

  static const std::string kControllerName = "ControllerSocialGraphRcdMessageService";

  ConnectionPoolManager::instance().initialize();

  // register to Controller Service
  ServerStateSubscriber::instance().initialize(kControllerName,  &RcdMessageServiceImpl::instance(), mod, interval, new MyUtil::SocialGraphChannel());
  ServerStateSubscriber::instance().setStatus(1);

  ActiveTrack::instance().initialize();

}

namespace xce {
namespace socialgraph {


void DbUtil::AddSgBlock(Ice::Long source, Ice::Long block, const std::string& biz) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  std::ostringstream otem;
  otem << "DbUtil::AddSgBlock(" << source << "," << block << "," << biz << ")";
  TimeCost tc = TimeCost::create(otem.str(), TimeCost::LOG_LEVEL_INFO);
  const static std::string kDbSourceName = "socialgraph_block";
  const static std::string table_name = "sg_block";
  Statement sql;
  sql << " INSERT INTO " << table_name
      << " (source_id, block_id, business)"
      << " VALUES (" << source << ", " << block
      << ", '" << biz << "')";
  try {
     mysqlpp::SimpleResult result = QueryRunner(kDbSourceName, CDbWServer).execute(sql);
  } catch (std::exception& e) {
    std::cerr << e.what();
    MCE_WARN("DbUtil::AddSgBlock " << e.what());
  } catch (...) {
    MCE_WARN("DbUtil::AddSgBlock ... exception");
  }

}

RcdMessageServiceImpl::RcdMessageServiceImpl() {

}

void RcdMessageServiceImpl::addSgBlock(::Ice::Long source, ::Ice::Long block, const ::std::string& biz, const ::Ice::Current& current) {
  MCE_INFO("RcdMessageServiceImpl::addSgBlock(" << source << ", " << block << ", " << biz << ")");
  if (source > 0) {
    DbUtil::AddSgBlock(source, block, biz);
  }
}


void RcdMessageServiceImpl::removeRcd(::Ice::Long source, ::Ice::Long target, const ::Ice::Current& current) {
  MCE_INFO("RcdMessageServiceImpl::removeRcd(" << source << ", " << target << ")");
  const static std::string kBiz = "RCD";
  BinaryData bin_data;
  bool fetch = BusinessCacheAdapter::GetInstance().Get(kBiz, source, bin_data);
  if (!fetch) {
    return;
  }
  
  Items old_items;
  if (!old_items.ParseFromArray(bin_data.data(), bin_data.size())) {
    return;
  }
  Items new_items;
  BOOST_FOREACH(const Item& it, old_items.items()) {
    if (it.id() != target) {
      Item* item = new_items.add_items();
      *item = it;
    }
  } 
  BinaryData new_bin_data;
  int size = new_items.ByteSize();
  std::string s;
  new_items.SerializeToString(&s);
  new_bin_data.reserve(size);
  new_bin_data.assign(s.data(), s.data() + size);
  BusinessCacheAdapter::GetInstance().Set(kBiz, source, new_bin_data);
}

void RcdMessageServiceImpl::removeBizRcd(::Ice::Long source, ::Ice::Long target, const std::string& biz,const ::Ice::Current& current) {
  MCE_INFO("RcdMessageServiceImpl::removeRcd(" << source << ", " << target << "," << biz << ")");
  const static std::string kBiz = "RCD";
  BinaryData bin_data;
  bool fetch = BusinessCacheAdapter::GetInstance().Get(kBiz, source, bin_data);
  if (!fetch) {
    return;
  }
  
  Items old_items;
  if (!old_items.ParseFromArray(bin_data.data(), bin_data.size())) {
    return;
  }
  Items new_items;
  BOOST_FOREACH(const Item& it, old_items.items()) {
    if (!it.has_message()) {
      continue;
    }
    if (it.id() != target || it.message() != biz) {
      Item* item = new_items.add_items();
      *item = it;
    }
  } 
  BinaryData new_bin_data;
  int size = new_items.ByteSize();
  std::string s;
  new_items.SerializeToString(&s);
  new_bin_data.reserve(size);
  new_bin_data.assign(s.data(), s.data() + size);
  BusinessCacheAdapter::GetInstance().Set(kBiz, source, new_bin_data);
}

void RcdMessageServiceImpl::removeBizCache(::Ice::Long source, ::Ice::Long target, const ::std::string& biz, const ::Ice::Current& current) {
  MCE_INFO("RcdMessageServiceImpl::removeBizCache(" << source << ", " << target << "," << biz << ")");
  BinaryData bin_data;
  bool fetch = BusinessCacheAdapter::GetInstance().Get(biz, source, bin_data);
  if (!fetch) {
    return;
  }
  
  Items old_items;
  if (!old_items.ParseFromArray(bin_data.data(), bin_data.size())) {
    return;
  }
  Items new_items;
  BOOST_FOREACH(const Item& it, old_items.items()) {
    if (it.id() != target) {
      Item* item = new_items.add_items();
      *item = it;
    }
  } 
  BinaryData new_bin_data;
  int size = new_items.ByteSize();
	new_bin_data.resize(size);
	new_items.SerializeToArray(new_bin_data.data(), size);	
  BusinessCacheAdapter::GetInstance().Set(biz, source, new_bin_data);
}

}}

//#include <iostream>
//int main(int argc, char* argv[]) {
//  using namespace MyUtil;
//  using namespace com::xiaonei::xce;
//  using namespace xce::serverstate;
//  using namespace xce::socialgraph;
//  ConnectionPoolManager::instance().initialize();
//  DbUtil::AddSgBlock(123L, 456L, "FFW");
//  DbUtil::AddSgBlock(123L, 456789L);
//  std::cout << "hello world" << std::endl;
//  SetData();
//  GetData();
//  Remove(5);
//  GetData();
//  Remove2(1, "RCD");
//  Remove2(2, "RCD");
//  Remove2(3, "RCD");
//  GetData();
//  return 0;
//}

