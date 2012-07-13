#ifndef _SG_RELATION_ADAPTER_H_
#define _SG_RELATION_ADAPTER_H_

#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "SocialGraphRelation.h"

namespace xce {
namespace socialgraph {

class SgRelationAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SgRelationServicePrx>, public MyUtil::Singleton<SgRelationAdapter> {
 public:
  SgRelationAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SgRelationServicePrx>("ControllerSocialGraphRelationService", 120, 1000, new MyUtil::SocialGraphChannel()) {

  }
  void reload(int user_id);
  void deleteEmotionStateCache(int user_id);
  EmotionState getEmotionState(int user_id);
  void setEmotionState(int user_id, const EmotionState& stat); 
};


}}
#endif

