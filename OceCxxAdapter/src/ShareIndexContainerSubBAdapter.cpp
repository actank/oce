#include "ShareIndexContainerSubBAdapter.h"
const int PROXY_NUM = 2;
ShareIndexContainerSubBAdapter::ShareIndexContainerSubBAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

ShareIndexContainerSubBAdapter::~ShareIndexContainerSubBAdapter()
{
}

bool ShareIndexContainerSubBAdapter::update(xce::sharesearch::ShareIndexInfoSeq shareinfos)
{
  std::cout<<"enter in adapter SubB..."<<std::endl; 
  //Prx proxy;
  Prx proxy;
  proxy = locate<com::xiaonei::sharesearch::server::ShareIndexContainerPrx>(/*managers_,*/ "ShareIndexContainer_SubB", MyUtil::TWO_WAY); 
  MCE_INFO(proxy);  
  proxy->update(shareinfos);
  
  //Prx proxy = locate<com::xiaonei::shareSearch::server::ShareIndexContainerPrx>(/* managers_,*/ "ShareIndexContainer_SubA", MyUtil::TWO_WAY); 
  //MCE_INFO(proxy);
  //proxy->update(shareinfos);
  
  
  
  return true;//proxy->update(shareinfos);
}

bool ShareIndexContainerSubBAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return ShareIndexContainerSubBAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


ShareIndexContainerSubBAdapter::Prx ShareIndexContainerSubBAdapter::getManagerOneway(int id) {
  return locate<ShareIndexContainerSubBAdapter::Prx> (this->managers_oneway_, "ShareIndexContainer_SubB",
      id, MyUtil::ONE_WAY, this->timeout_);
}
