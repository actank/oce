#include "ShareIndexContainerAdapter.h"
const int PROXY_NUM = 2;
ShareIndexContainerAdapter::ShareIndexContainerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

ShareIndexContainerAdapter::~ShareIndexContainerAdapter()
{
}

bool ShareIndexContainerAdapter::update(xce::sharesearch::ShareIndexInfoSeq shareinfos)
{
  std::cout<<"enter in adapter..."<<std::endl; 
  //Prx proxy;
  Prx proxy[PROXY_NUM];
  setName("ShareIndexContainer_SubA");
  setEndPoints("@ShareIndexContainer_SubA.ShareIndexContainer_SubA.ShareIndexContainer_SubA");
  proxy[0] = locate<com::xiaonei::sharesearch::server::ShareIndexContainerPrx>(/*managers_,*/ "ShareIndexContainer_SubA", MyUtil::TWO_WAY); 
  
  MCE_INFO(proxy[0]);
  setName("ShareIndexContainer_SubB");
  setEndPoints("@ShareIndexContainer_SubB.ShareIndexContainer_SubB.ShareIndexContainer_SubB"); 
  proxy[1] = locate<com::xiaonei::sharesearch::server::ShareIndexContainerPrx>(/*managers_,*/ "ShareIndexContainer_SubB", MyUtil::TWO_WAY); 
  
  //std::cout<<proxies.size()<<std::endl; 
  //int rand_index = (int)(rand() % proxies.size());
  MCE_INFO(proxy[1]);
  proxy[0]->update(shareinfos);
  proxy[1]->update(shareinfos);
  
  //Prx proxy = locate<com::xiaonei::shareSearch::server::ShareIndexContainerPrx>(/* managers_,*/ "ShareIndexContainer_SubA", MyUtil::TWO_WAY); 
  //MCE_INFO(proxy);
  //proxy->update(shareinfos);
  
  
  
  return true;//proxy->update(shareinfos);
}

bool ShareIndexContainerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return ShareIndexContainerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


ShareIndexContainerAdapter::Prx ShareIndexContainerAdapter::getManagerOneway(int id) {
  return locate<ShareIndexContainerAdapter::Prx> (this->managers_oneway_, "ShareIndexContainer_SubA",
      id, MyUtil::ONE_WAY, this->timeout_);
}
