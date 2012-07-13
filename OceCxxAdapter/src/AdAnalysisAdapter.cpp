#include "AdAnalysisAdapter.h"
namespace xce{
namespace ad{
AdAnalysisManagerPrx AdAnalysisAdapter::getManagerOneway(){
	return locate<AdAnalysisManagerPrx> (oneway_proxy_, "M", 0, ONE_WAY);
}

AdAnalysisManagerPrx AdAnalysisAdapter::getManager(){
	return locate<AdAnalysisManagerPrx> (twoway_proxy_, "M", 0, TWO_WAY);
}

void AdAnalysisAdapter::Pv(const AdPvParameterSeq& paraseq){
  getManagerOneway()->Pv(paraseq);
}

//void AdAnalysisAdapter::Click(Ice::Long group_id, Ice::Long creative_id, Ice::Int click_price, Ice::Long bit_unit_id, int userid, const string& ip, const string& click_url, const string& ubc, const string& to_url, const string& pv_time){
//  getManagerOneway()->Click(group_id, creative_id, click_price, bit_unit_id, userid, ip, click_url, ubc, to_url, pv_time);
//}

void AdAnalysisAdapter::Click(const AdClickParameterPtr& para){
  getManagerOneway()->Click(para);
}
void AdAnalysisAdapter::ReloadGroupById(Ice::Long groupid){
  getManagerOneway()->ReloadGroupById(groupid);
}
void AdAnalysisAdapter::ReloadMemberById(Ice::Long memberid){
  getManagerOneway()->ReloadMemberById(memberid);
}
void AdAnalysisAdapter::ReloadCampaignById(Ice::Long planid){
  getManagerOneway()->ReloadCampaignById(planid);
}
void AdAnalysisAdapter::ReloadCreativeById(Ice::Long creativeid){
  getManagerOneway()->ReloadCreativeById(creativeid);
}


void AdAnalysisAdapter::Pv2(const AdPvParameterSeq& paraseq, const MyUtil::StrSeq& append_str){
	getManagerOneway()->Pv2(paraseq,append_str);
}
void AdAnalysisAdapter::Click2(const AdClickParameterPtr& para, const string& append_str){
	getManagerOneway()->Click2(para,append_str);
}


};
};
