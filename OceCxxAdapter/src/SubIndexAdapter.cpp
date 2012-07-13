#include "SubIndexAdapter.h"

SubIndexAdapter::~SubIndexAdapter()
{
}


bool SubIndexAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  return true;
}

void SubIndexAdapterFactory::loadSubIndexAdapter()  {
  try  {
  SubIndexAdapter::instance().setName("StateContainerLogic01");

  SubIndexAdapter::instance().setEndpoints("@StateContainerLogic01.StateContainerLogic01.StateContainerLogic01");

  SubIndexAdapter::instance().setProxy();
  
  _instances["State"] = SubIndexAdapter::instance().getProxy();
 
  SubIndexAdapter::instance().setName("BlogContainerLogic01");

  SubIndexAdapter::instance().setEndpoints("@BlogContainerLogic01.BlogContainerLogic01.BlogContainerLogic01");

  SubIndexAdapter::instance().setProxy();
  
  _instances["Blog"] = SubIndexAdapter::instance().getProxy();
 
 SubIndexAdapter::instance().setName("ShareContainerLogic01");

  SubIndexAdapter::instance().setEndpoints("@ShareContainerLogic01.ShareContainerLogic01.ShareContainerLogic01");

  SubIndexAdapter::instance().setProxy();
  
  _instances["Share"] = SubIndexAdapter::instance().getProxy();

  SubIndexAdapter::instance().setName("LBSContainerLogic01");

  SubIndexAdapter::instance().setEndpoints("@LBSContainerLogic01.LBSContainerLogic01.LBSContainerLogic01");

  SubIndexAdapter::instance().setProxy();
  
  _instances["LBS"] = SubIndexAdapter::instance().getProxy();

   MCE_INFO("SubIndexAdapter instance size is "<<_instances.size()); 
  }  catch (Ice::Exception& e) {
    MCE_WARN("SubIndexAdapter::update Ice::Exception " << e);
  } catch (std::exception& e) {
    MCE_WARN("SubIndexAdapter::update std::exception " << e.what());
  } catch (...) {
    MCE_WARN("SubIndexAdapter::update Unknown exception");
  }
}

RmiLogicPrx SubIndexAdapterFactory::getNamed(const string& name)  {
  if(_instances.empty() || _instances.find(name) == _instances.end())  {
    return NULL;
  }
  return _instances[name];
}

map<string, RmiLogicPrx> SubIndexAdapterFactory::_instances = map<string, RmiLogicPrx>();

