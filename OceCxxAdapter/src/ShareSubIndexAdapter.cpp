#include "ShareSubIndexAdapter.h"
const int PROXY_NUM = 2;
ShareSubIndexAdapter::ShareSubIndexAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

ShareSubIndexAdapter::~ShareSubIndexAdapter()
{
}

bool ShareSubIndexAdapter::update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq shareinfos)
{
  std::cout<<"enter in adapter..."<<std::endl; 
  //Prx proxy;
  std::cout<<"proxy !!!"<<std::endl; 
  Prx proxy;
  proxy = locate<com::xiaonei::search5::logicserver::RmiLogicPrx>(/*managers_,*/ "ShareUpdateLogicRmi01", MyUtil::TWO_WAY); 
  std::cout<<"proxy is "<<proxy<<std::endl; 
  MCE_INFO(proxy);
  
  try  {
    proxy->update(mod, shareinfos);
  } catch (Ice::Exception& e) {
    MCE_INFO("ShareSubIndexAdapter::update Ice::Exception " << e);
  } catch (std::exception& e) {
    MCE_INFO("ShareSubIndexAdapter::update std::exception " << e.what());
  } catch (...) {
    MCE_INFO("ShareSubIndexAdapter::update Unknown exception");
  }

  
  return true;//proxy->update(shareinfos);
}

bool ShareSubIndexAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return ShareSubIndexAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


ShareSubIndexAdapter::Prx ShareSubIndexAdapter::getManagerOneway(int id) {
  return locate<ShareSubIndexAdapter::Prx> (this->managers_oneway_, "ShareUpdateLogicRmi01",
      id, MyUtil::ONE_WAY, this->timeout_);
}
