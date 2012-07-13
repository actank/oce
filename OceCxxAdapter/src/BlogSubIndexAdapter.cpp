#include "BlogSubIndexAdapter.h"
const int PROXY_NUM = 2;
BlogSubIndexAdapter::BlogSubIndexAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

BlogSubIndexAdapter::~BlogSubIndexAdapter()
{
}

bool BlogSubIndexAdapter::update(int mod, com::xiaonei::search5::logicserver::IndexUpdateDataSeq bloginfos)
{
  std::cout<<"enter in adapter..."<<std::endl; 
  //Prx proxy;
  Prx proxy;
  proxy = locate<com::xiaonei::search5::logicserver::RmiLogicPrx>(/*managers_,*/ "BlogUpdateLogicRmi01", MyUtil::TWO_WAY); 
  
  MCE_INFO(proxy);
  
  try  {
    proxy->update(mod, bloginfos);
  } catch (Ice::Exception& e) {
    MCE_INFO("BlogSubIndexAdapter::update Ice::Exception " << e);
  } catch (std::exception& e) {
    MCE_INFO("BlogSubIndexAdapter::update std::exception " << e.what());
  } catch (...) {
    MCE_INFO("BlogSubIndexAdapter::update Unknown exception");
  }

  
  return true;//proxy->update(shareinfos);
}

bool BlogSubIndexAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return BlogSubIndexAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


BlogSubIndexAdapter::Prx BlogSubIndexAdapter::getManagerOneway(int id) {
  return locate<BlogSubIndexAdapter::Prx> (this->managers_oneway_, "BlogUpdateLogicRmi01",
      id, MyUtil::ONE_WAY, this->timeout_);
}
