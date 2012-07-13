#include "ShareIndexContainerSubAAdapter.h"
const int PROXY_NUM = 2;
ShareIndexContainerSubAAdapter::ShareIndexContainerSubAAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

ShareIndexContainerSubAAdapter::~ShareIndexContainerSubAAdapter()
{
}

bool ShareIndexContainerSubAAdapter::update(xce::sharesearch::ShareIndexInfoSeq shareinfos)
{
  std::cout<<"enter in adapter..."<<std::endl; 
  //Prx proxy;
  Prx proxy;
  proxy = locate<com::xiaonei::sharesearch::server::ShareIndexContainerPrx>(/*managers_,*/ "ShareIndexContainer_SubA", MyUtil::TWO_WAY); 
  
  MCE_INFO(proxy);
  
  //std::cout<<proxies.size()<<std::endl; 
  //int rand_index = (int)(rand() % proxies.size());
  proxy->update(shareinfos);
  
  //Prx proxy = locate<com::xiaonei::shareSearch::server::ShareIndexContainerPrx>(/* managers_,*/ "ShareIndexContainer_SubA", MyUtil::TWO_WAY); 
  //MCE_INFO(proxy);
  //proxy->update(shareinfos);
  
  
  
  return true;//proxy->update(shareinfos);
}

bool ShareIndexContainerSubAAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return ShareIndexContainerSubAAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


ShareIndexContainerSubAAdapter::Prx ShareIndexContainerSubAAdapter::getManagerOneway(int id) {
  return locate<ShareIndexContainerSubAAdapter::Prx> (this->managers_oneway_, "ShareIndexContainer_SubA",
      id, MyUtil::ONE_WAY, this->timeout_);
}
