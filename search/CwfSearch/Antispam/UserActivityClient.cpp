#include <Ice/Ice.h>
#include "OzeinFilterAdapter.h"
#include "UserActivityClient.h"
#include "FilterQuery.h"
#include "FilterAnswer.h"
#include "base/logging.h"

using namespace MyUtil;
using namespace com::xiaonei::ozein::activity::generation;
using namespace std;

UserActivityPrx UserActivityClient::getProxy(int id) {
	return locate<UserActivityPrx> (_managers, "Anti_Spam_Activity", id, TWO_WAY);
}
int UserActivityClient::checkActivity(int fromid, int toid, int type, const string& content){
//  LOG(INFO) << "UserActivityClient fromid:" << fromid << " toid:" << toid << " type:" << type << " content:" << content;
  UserActivityPrx prx = getProxy(0);
//  LOG(INFO) << "UserActivityClient prx:" << prx->ice_getConnection();
  int flag;
  try {
    flag = prx->checkActivity(fromid,toid,type,content);
  } catch (exception& e) {
    LOG(INFO) << "UserActivityClient " << e.what();
  }
	return flag;
}

