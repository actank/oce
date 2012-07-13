#include "GoogleAdHandler.h"

namespace xce {
namespace ad {

bool GoogleAdHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  int uid = para.userProfile_.id();
  MCE_DEBUG("GoogleAdHandler::handle() --> uid = " << uid);
  string area = para.userProfile_.ip_area();
  size_t pos = area.find("0086");
  if(pos == string::npos) {
    AdGroupPtr group = para.adzone_->GetGoogleGroups();
    if(group){
      SelectedAdGroupPtr sp = new SelectedAdGroup(group);
      sp->pos_ = 1;
      para.zone_selected_groups_.insert(sp);
      para.total_selected_groups_.insert(sp);
      MCE_DEBUG("GoogleAdHandler::handle() --> uid " << uid << " not in china, got google ad " << group->group_id());
    } else {
      MCE_DEBUG("GoogleAdHandler::handle() --> uid " << uid << " not in china, no google ad ");
    }
  } else {
    MCE_DEBUG("GoogleAdHandler::handle() --> uid " << uid << " in china, no google ad");
  }
  return true;
}

}
}
