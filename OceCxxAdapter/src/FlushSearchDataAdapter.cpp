#include "FlushSearchDataAdapter.h"

void FlushDataAdapterFactory::loadFlushSearchDataAdapter()  {
  instances_["ControllerFlushBlogData"] = boost::shared_ptr<FlushSearchDataAdapter>(new FlushSearchDataAdapter("ControllerFlushBlogData"));
  instances_["ControllerFlushShareData"] = boost::shared_ptr<FlushSearchDataAdapter>(new FlushSearchDataAdapter("ControllerFlushShareData"));
  instances_["ControllerFlushStateData"] = boost::shared_ptr<FlushSearchDataAdapter>(new FlushSearchDataAdapter("ControllerFlushStateData"));
  instances_["ControllerFlushLBSData"] = boost::shared_ptr<FlushSearchDataAdapter>(new FlushSearchDataAdapter("ControllerFlushLBSData"));
}


boost::shared_ptr<FlushSearchDataAdapter> FlushDataAdapterFactory::getNamed(string name)  {
  return instances_.find(name)->second;
}

map<string, boost::shared_ptr<FlushSearchDataAdapter> > FlushDataAdapterFactory::instances_ = map<string, boost::shared_ptr<FlushSearchDataAdapter> >();

FlushSearchDataAdapter::FlushSearchDataAdapter(const string& business_type): MyUtil::ReplicatedClusterAdapterI<IndexUpdateLogicPrx>(business_type, 120, 300, new MyUtil::AddressbookChannel), business_type_(business_type)  {
}


FlushSearchDataAdapter::~FlushSearchDataAdapter()  {
}

bool FlushSearchDataAdapter::update(IndexUpdateData info)
{
  IndexUpdateLogicPrx proxy = getProxy(0); 
  try  {
    MCE_DEBUG("FlushDataAdapter proxy is "<<proxy); 
    proxy->update(info);
  } catch (Ice::Exception& e) {
    MCE_WARN("FlushSearchData::update Ice::Exception " << e);
    MCE_WARN("FlushSearchData Proxy is "<<proxy);
  } catch (std::exception& e) {
    MCE_WARN("FlushSearchData::update std::exception " << e.what());
    MCE_WARN("ContaienrIndexAdapter Proxy is "<<proxy);
  } catch (...) {
    MCE_WARN("FlushSearchData::update Unknown exception");
    MCE_WARN("FlushSearchData Proxy is "<<proxy);
  }
  return true;
}

void FlushSearchDataAdapter::setValid(bool new_state)  {
 return ;
}

void FlushSearchDataAdapter::setRecovery(bool new_state)  {

 return;
}

bool FlushSearchDataAdapter::isRecovery()  {
 return true;
}

bool FlushSearchDataAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  return true;
}

