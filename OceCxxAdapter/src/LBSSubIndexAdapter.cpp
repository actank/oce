#include "LBSSubIndexAdapter.h"
const int PROXY_NUM = 2;
LBSSubIndexAdapter::LBSSubIndexAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

LBSSubIndexAdapter::~LBSSubIndexAdapter()
{
}

bool LBSSubIndexAdapter::update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq bloginfos)
{
  MCE_INFO("enter in adapter..."); 
  //Prx proxy;
  Prx proxy;
  proxy = locate<com::xiaonei::search5::logicserver::RmiLogicPrx>(/*managers_,*/"LBSUpdateLogicRmi01", MyUtil::TWO_WAY); 
   

  MCE_INFO(proxy);
  
  try  {
    proxy->update(mod, bloginfos);
  } catch (Ice::Exception& e) {
    MCE_INFO("LBSSubIndexAdapter::update Ice::Exception " << e);
  } catch (std::exception& e) {
    MCE_INFO("LBSSubIndexAdapter::update std::exception " << e.what());
  } catch (...) {
    MCE_INFO("LBSSubIndexAdapter::update Unknown exception");
  }

  
  return true;//proxy->update(shareinfos);
}

bool LBSSubIndexAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return LBSSubIndexAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


LBSSubIndexAdapter::Prx LBSSubIndexAdapter::getManagerOneway(int id) {
  return locate<LBSSubIndexAdapter::Prx> (this->managers_oneway_, "LBSUpdateLogicRmi01",
      id, MyUtil::ONE_WAY, this->timeout_);
}
