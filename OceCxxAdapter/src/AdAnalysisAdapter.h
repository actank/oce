#ifndef __ADANALYSIS_ADAPTER_H__
#define __ADANALYSIS_ADAPTER_H__

#include "AdapterI.h"
#include "AdAnalysis.h"

namespace xce{
namespace ad{
  using namespace MyUtil;
  class AdAnalysisAdapter : public AdapterI, public AdapterISingleton<MyUtil::AdChannel, AdAnalysisAdapter>{
  public:
    void Pv(const AdPvParameterSeq& paraseq);
    //void Click(Ice::Long group_id, Ice::Long creative_id, Ice::Int click_price, Ice::Long bit_unit_id, int userid, const string& ip, const string& click_url, const string& ubc, const string& to_url, const string& pv_time);
    void Click(const AdClickParameterPtr& para);

    void Pv2(const AdPvParameterSeq& paraseq, const MyUtil::StrSeq& append_str);
    void Click2(const AdClickParameterPtr& para, const string& append_str);

    void ReloadGroupById(Ice::Long groupid);
    void ReloadMemberById(Ice::Long memberid);
    void ReloadCampaignById(Ice::Long planid);
    void ReloadCreativeById(Ice::Long creativeid);

    int getCluster(){
      return _cluster;
    }
  protected:
    virtual string name(){
      return "AdAnalysis";
    }
    virtual string endpoints(){
      return "@AdAnalysis";
    }
    virtual size_t cluster(){
      return 1;
    }
    AdAnalysisManagerPrx getManagerOneway();
    AdAnalysisManagerPrx getManager();
  private:
    vector<AdAnalysisManagerPrx> oneway_proxy_;
    vector<AdAnalysisManagerPrx> twoway_proxy_;
  };
};
};
#endif
