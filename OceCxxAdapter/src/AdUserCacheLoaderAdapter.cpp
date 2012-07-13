#include "AdUserCacheLoaderAdapter.h"
namespace xce{
namespace ad{

AdUserCacheLoaderPrx AdUserCacheLoaderAdapter::getManager(int id) {
  return locate<AdUserCacheLoaderPrx> (_managers, "M", id, TWO_WAY);
}

AdUserCacheLoaderPrx AdUserCacheLoaderAdapter::getManagerOneway(int id) {
  return locate<AdUserCacheLoaderPrx> (_managersOneway, "M", id, ONE_WAY);
}

void AdUserCacheLoaderAdapter::LoadByIdSeq(const MyUtil::IntSeq& ids){
  if(!ids.empty()){
    getManagerOneway(ids.at(0))->LoadByIdSeq(ids);
  }
}

void AdUserCacheLoaderAdapter::LoadById(int userid){
  getManagerOneway(userid)->LoadById(userid);
}

void AdUserCacheLoaderAdapter::ReloadUserInfoByIdSeq(const MyUtil::IntSeq& ids){
  if(!ids.empty()){
    getManagerOneway(ids.at(0))->ReloadUserInfoByIdSeq(ids);
  }
}

void AdUserCacheLoaderAdapter::ReloadUserInfoById(int userid){
  getManagerOneway(userid)->ReloadUserInfoById(userid);
}

string AdUserCacheLoaderAdapter::GetById(int userid){
  return getManager(userid)->GetById(userid);
}

}
}
