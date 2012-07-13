#include "AdLogAdapter.h"
namespace xce{
namespace ad{
AdLogManagerPrx AdLogAdapter::getManagerOneway(int server_index){
	return getProxy(server_index);
  //return locate<AdLogManagerPrx> (oneway_proxy_, "M", server_index, ONE_WAY);
}

AdLogManagerPrx AdLogAdapter::getManager(int server_index){
	return getProxy(server_index);
  //return locate<AdLogManagerPrx> (twoway_proxy_, "M", server_index, TWO_WAY);
}

void AdLogAdapter::Pv(const string& log){
  server_index_ = (server_index_ + 1) % getCluster();
  getManagerOneway(server_index_)->Pv(log);
}

void AdLogAdapter::PvBySeq(const MyUtil::StrSeq & logs){
  server_index_ = (server_index_ + 1) % getCluster();
  getManagerOneway(server_index_)->PvBySeq(logs);
}

void AdLogAdapter::ClickForBp(const string& log){
  server_index_ = (server_index_ + 1) % getCluster();
  getManagerOneway(server_index_)->ClickForBp(log);
}
void AdLogAdapter::ClickForCharge(const string& log){
  server_index_ = (server_index_ + 1) % getCluster();
  getManagerOneway(server_index_)->ClickForCharge(log);
}
};
};
