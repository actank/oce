#include "StateSubIndexAdapter.h"
const int PROXY_NUM = 2;
StateSubIndexAdapter::StateSubIndexAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

StateSubIndexAdapter::~StateSubIndexAdapter()
{
}

bool StateSubIndexAdapter::update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq stateinfos)
{
  std::cout<<"enter in adapter..."<<std::endl; 
  //Prx proxy;
  std::cout<<"proxy !!!"<<std::endl; 
  Prx proxy;
  proxy = locate<com::xiaonei::search5::logicserver::RmiLogicPrx>(/*managers_,*/ "StateUpdateLogicRmi01", MyUtil::TWO_WAY); 
  std::cout<<"proxy is "<<proxy<<std::endl; 
  MCE_INFO(proxy);
  
  try  {
    proxy->update(mod, stateinfos);
  } catch (Ice::Exception& e) {
    MCE_INFO("StateSubIndexAdapter::update Ice::Exception " << e);
  } catch (std::exception& e) {
    MCE_INFO("StateSubIndexAdapter::update std::exception " << e.what());
  } catch (...) {
    MCE_INFO("StateSubIndexAdapter::update Unknown exception");
  }

  
  return true;//proxy->update(shareinfos);
}

bool StateSubIndexAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return StateSubIndexAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


StateSubIndexAdapter::Prx StateSubIndexAdapter::getManagerOneway(int id) {
  return locate<StateSubIndexAdapter::Prx> (this->managers_oneway_, "StateUpdateLogicRmi01",
      id, MyUtil::ONE_WAY, this->timeout_);
}
