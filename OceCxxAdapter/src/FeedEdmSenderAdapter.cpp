#include "FeedEdmSenderAdapter.h"
namespace xce {
namespace edm {

FeedEdmSenderPrx FeedEdmSenderAdapter::getManager(int id) {
  return locate<FeedEdmSenderPrx> (_managers, "FEDM", id, MyUtil::TWO_WAY);
}

FeedEdmSenderPrx FeedEdmSenderAdapter::getManagerOneway(int id) {
  return locate<FeedEdmSenderPrx> (_managersOneway, "FEDM", id, MyUtil::ONE_WAY);
}

void FeedEdmSenderAdapter::Receive(Ice::Int user_id, const MyUtil::Str2StrMap& params) {
  getManagerOneway(user_id)->Receive(user_id, params); 
}
void FeedEdmSenderAdapter::ReceiveForPhone(Ice::Int user_id, const MyUtil::Str2StrMap& params) {
  getManagerOneway(user_id)->ReceiveForPhone(user_id, params); 
}
void FeedEdmSenderAdapter::NoCheckReceive(Ice::Int user_id, const MyUtil::Str2StrMap& params) {
  getManagerOneway(user_id)->NoCheckReceive(user_id, params); 
}
void FeedEdmSenderAdapter::ClearFrequencyCacheByUser(int userid){
  getManagerOneway(userid)->ClearFrequencyCacheByUser(userid); 
}
void FeedEdmSenderAdapter::FlushCache() {
  for(size_t i = 0; i < cluster(); i++) {
    getManagerOneway(i)->FlushCache();
  }
}
/*
void FeedEdmSenderAdapter::SetSendingFlag(int type, int edm_id) {
  getManagerOneway(edm_id);
  for (std::vector<FeedEdmSenderPrx>::iterator ite = _managersOneway.begin(); ite !=  _managersOneway.end(); ++ite) {
    (*ite)->SetSendingFlag(type,edm_id);
  }
}

void FeedEdmSenderAdapter::SetSuspendFlag(Ice::Int type, Ice::Int edm_id) {
  getManagerOneway(edm_id);
  for (std::vector<FeedEdmSenderPrx>::iterator ite = _managersOneway.begin(); ite !=  _managersOneway.end(); ++ite) {
    (*ite)->SetSuspendFlag(type,edm_id);
  }
}

void FeedEdmSenderAdapter::IncreaseConsume(int type, int id) {
  getManagerOneway(id);
  for (std::vector<FeedEdmSenderPrx>::iterator ite = _managersOneway.begin(); ite !=  _managersOneway.end(); ++ite) {
    (*ite)->IncreaseConsume(type,id);
  }
}
void FeedEdmSenderAdapter::IncreaseConsumeInMinite(int id) {
  getManagerOneway(id);
  for (std::vector<FeedEdmSenderPrx>::iterator ite = _managersOneway.begin(); ite !=  _managersOneway.end(); ++ite) {
    (*ite)->IncreaseConsumeInMinite(id);
  }
}
*/
}
}
