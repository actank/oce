#include "AdLogSAdapter.h"
namespace xce{
namespace ad{
AdLogManagerPrx AdLogAdapter::getManagerOneway(int index){
	return locate<AdLogManagerPrx> (oneway_proxy_, "M", index, ONE_WAY, 200);
}

AdLogManagerPrx AdLogAdapter::getManager(int index){
	return locate<AdLogManagerPrx> (twoway_proxy_, "M", index, TWO_WAY, 200);
}

void AdLogAdapter::PvBatch(const ::MyUtil::StrSeq& logs) {
  index_= (index_+1)%3;
  getManagerOneway(index_)->PvBatch(logs);
}

void AdLogAdapter::ClickForBp(const string& log){
  index_= (index_+1)%3;
  getManagerOneway(index_)->ClickForBp(log);
}
void AdLogAdapter::ClickForCharge(const string& log){
  getManagerOneway(0)->ClickForCharge(log);
}
void AdLogAdapter::PvForCharge(const ::MyUtil::StrSeq& logs){
  getManagerOneway(3)->PvForCharge(logs);
}
};
};
