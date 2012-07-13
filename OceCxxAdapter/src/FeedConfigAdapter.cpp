#include "FeedConfigAdapter.h"

using namespace xce::feed;
using namespace MyUtil;

/*void
FeedDispatcherNAdapter::deliverSupply(int supplier, int accepter, const Ice::Context& ctx){
	_managerOneway->deliverSupply(supplier, accepter, ctx);
}
*/

void ConfigManagerAdapter::SetSendConfig(int uid, int type, int appid, bool newsconfig){
  getManager(0)->SetSendConfig(uid, type, appid, newsconfig);
}
bool ConfigManagerAdapter::GetSendConfig(int uid, int type,int appid){
  return getManager(0)->GetSendConfig(uid,  type , appid);
}

void ConfigManagerAdapter::SetRecvConfig(int uid, int type, int appid, bool newsconfig){
  getManager(0)->SetRecvConfig(uid, type, appid, newsconfig);
}
MyUtil::IntArray ConfigManagerAdapter::FilterRecvConfig(MyUtil::IntArray target,int type,int appid){
  return   getManager(0)->FilterRecvConfig(target,  type , appid);
}


UserConfigDict ConfigManagerAdapter::GetUserBlockedAppDict(int uid)
{
   return getManager(0)->GetUserBlockedAppDict(uid); 
}  

ConfigManagerPrx ConfigManagerAdapter::getManager(int id) {
	return locate<ConfigManagerPrx> (_managers, "M", id, TWO_WAY);
}

ConfigManagerPrx ConfigManagerAdapter::getManagerOneway(int id) {
	return locate<ConfigManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

