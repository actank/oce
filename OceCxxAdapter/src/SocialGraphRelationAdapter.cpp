#include "SocialGraphRelationAdapter.h"

namespace xce {
namespace socialgraph {

void SgRelationAdapter::reload(int user_id) {
  getProxy(user_id)->reload(user_id);
}

void SgRelationAdapter::deleteEmotionStateCache(int user_id) {
  getProxy(user_id)->deleteEmotionStateCache(user_id);
}

EmotionState SgRelationAdapter::getEmotionState(int user_id) {
  return getProxy(user_id)->getEmotionState(user_id);
}
void SgRelationAdapter::setEmotionState(int user_id, const EmotionState& stat) {
  getProxy(user_id)->setEmotionState(user_id, stat);
}

}}


