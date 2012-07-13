#ifndef _SG_RCD_MESSAGE_SERVICE_ADAPTER_H_
#define _SG_RCD_MESSAGE_SERVICE_ADAPTER_H_

#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "SocialGraphRcdMessageService.h"


namespace xce {
namespace socialgraph {

class RcdMessageServiceAdapter : public MyUtil::ReplicatedClusterAdapterI<RcdMessageServicePrx>, public MyUtil::Singleton<RcdMessageServiceAdapter> {
 public:
  RcdMessageServiceAdapter() :
		MyUtil::ReplicatedClusterAdapterI<RcdMessageServicePrx>("ControllerSocialGraphRcdMessageService", 120, 1000, new MyUtil::SocialGraphChannel()) {

  }
  
  void AddSgBlock(Ice::Long source, Ice::Long block, const std::string& biz = "NON");
  void RemoveRcdCache(Ice::Long source, Ice::Long target, const std::string& biz = "NON");
  void AddSgBlockAndRemoveRcdCache(Ice::Long source, Ice::Long block, const std::string& biz = "NON");
  void RemoveBizCache(Ice::Long source, Ice::Long block, const std::string& biz);
   
  // buddy core logic use
  void BecomeFriends(int host, int guest);
  void Block(int host, int guest);
 
};


}}

#endif

