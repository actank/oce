#include "AdMonitorI.h"
#include "AdStruct.h"


namespace xce {
namespace ad {
using namespace std;

::std::string AdMonitorI::MemberStatus(::Ice::Long id, const ::Ice::Current&) {
  string result;
  ostringstream result_oss;
  if (id) {
    AdMemberPtr member = AdMemberPool::instance().GetAdMember(id);
    result_oss << "type:\tmember\nid:\t" << id << "\navailable:\t" << member->Available()
        << "\ndelete_flag:\t" << member->delete_flag()
        << "\nma_online_status:\t" << member->ma_online_status()
        << "\nno_exceed_max:\t" << member->no_exceed_max()
        << "\nam_online_status:\t" << member->am_online_status()
        << "\nhave_money:\t" << member->have_money()
        << "\nvalidity:\t" << member->validity()
        << "\nis_formal:\t" << member->is_formal() << endl;
  } else {
    result_oss << "No member id: " << id;
  }
  result = result_oss.str();
  MCE_DEBUG("[MONITOR_DEBUG] : Result = " << result);
  return result;
}

::std::string AdMonitorI::CampaignStatus(::Ice::Long id, const ::Ice::Current&) {
  string result;
  ostringstream result_oss;
  if (id) {
    AdCampaignPtr p = AdCampaignPool::instance().GetAdCampaign(id);
    result_oss << "type:\tcampaign\nid:\t" << id << "\navailable:\t" << p->Available()
        << "\ndelete_flag:\t" << p->delete_flag()
        << "\naudit_status:\t" << p->audit_status()
        << "\nam_status:\t" << p->am_status()
        << "\nmember_status:\t" << p->member_status()
        << "\nsys_status:\t" << p->sys_status();
    time_t rawtime = p->begin_time();
    struct tm * timeinfo = localtime(&rawtime);
    char* time = asctime (timeinfo);
    result_oss << "\nbegin_time:\t" << time;
    rawtime = p->end_time();
    timeinfo = localtime(&rawtime);
    time = asctime (timeinfo);
    result_oss << "\nend_time:\t" << time;
  } else {
    result_oss << "No campaign id: " << id;
  }
  result = result_oss.str();
  MCE_DEBUG("[MONITOR_DEBUG] : Result = " << result);
  return result;
}

::std::string AdMonitorI::GroupStatus(::Ice::Long id, const ::Ice::Current&) {
  string result;
  ostringstream result_oss;
  if (id) {
    AdGroupPtr p = AdGroupPool::instance().GetAdGroup(id);
    result_oss << "type:\tgroup\nid:\t" << id << "\navailable:\t" << p->Available()
        << "\nmember_status:\t" << p->member_status()
        << "\nam_status:\t" << p->am_status()
        << "\naudit_status:\t" << p->audit_status()
        << "\ndelete_flag:\t" << p->delete_flag()
        << "\ndisplay_type:\t" << p->display_type()
        << "\nmax_price:\t" << p->max_price()
        << "\nrotation_number:\t" << p->rotation_number();
  } else {
    result_oss << "No group id: " << id;
  }
  result = result_oss.str();
  MCE_DEBUG("[MONITOR_DEBUG] : Result = " << result);
  return result;
}

::std::string AdMonitorI::CompeteRef(::Ice::Long campaign_id, const ::Ice::Current&) {
  string result;
  ostringstream result_oss;
  if (campaign_id) {
    vector<Ice::Long> competes = CompeteRef::instance().Get(campaign_id);
    for(vector<Ice::Long>::iterator it = competes.begin(); it != competes.end(); ++it) {
      result_oss << "campaign_id:\t" << (*it) << endl;
    }
  } else {
    result_oss << "No CompeteRef of this campaign id: " << campaign_id;
  }
  result = result_oss.str();
  MCE_DEBUG("[MONITOR_DEBUG] : Result = " << result);
  return result;
}

::std::string AdMonitorI::RotationCache(::Ice::Long id, const ::Ice::Current&) {
  string result;
  ostringstream result_oss;
  if (id) {
    vector<AdGroupPtr> rotations = AdZonePool::instance().GetAdZone(id)->rotate_list();
    for(vector<AdGroupPtr>::iterator it = rotations.begin(); it != rotations.end(); ++it) {
      result_oss << "group_id:\t" << (*it)->group_id() << endl;
    }
  } else {
    result_oss << "No rotationcache of this adzone id: " << id;
  }
  result = result_oss.str();
  MCE_DEBUG("[MONITOR_DEBUG] : Result = " << result);
  return result;
}

}
}
