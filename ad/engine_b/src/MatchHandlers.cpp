#include "MatchHandlers.h"
#include "AdMatchManagerI.h"
#include "Date.h"
#include "AdLogAdapter.h"
#include "NielsonFilter.h"

using namespace xce::ad;

namespace xce {
namespace ad {

bool RotationHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  if (100000000073 == para.adzone_->id()){
    AdInfoSeq ad_info_seq;
    AdInfo ad_info;
    ad_info.groupid = -3;
    ad_info_seq.push_back(ad_info);
    result[para.adzone_->id()] = ad_info_seq; 
    return false;
  }
  /*
  去除72、73限制 
  if(100000000073 == para.adzone_->id() && ENGINE_U == para.remain_state_) {
    AdInfoSeq ad_info_seq;
    AdInfo ad_info;
    //ad_info.groupid = -1;
    ad_info.groupid = 0;
    ad_info_seq.push_back(ad_info);
    result[para.adzone_->id()] = ad_info_seq;
    //MCE_DEBUG("[REMAIN] RotationHandler::handle() --> user " << para.userProfile_.id() << ", adzone_id = " << para.adzone_->id() << ", flag = ENGINE_U, need ad from AdEngineU/Tw");
    return false; 
  }
  */
  //查找轮播广告
  if (para.PickCount() > 0) {
    set<AdGroupPtr> rotate_set;

    if(para.adzone_->GetRotateGroups(para.rotate_index_, para.userProfile_, rotate_set) && !rotate_set.empty()){
      Filter::instance().FilterCompete(para.userProfile_.id(), rotate_set, para.total_selected_groups_);
      if(!rotate_set.empty()){
        //update selected
        int pos = 1;
        for(set<AdGroupPtr>::iterator git = rotate_set.begin(); git != rotate_set.end(); ++git) {
          SelectedAdGroupPtr p = new SelectedAdGroup(*git);
          p->pos_ = pos++;
          para.zone_selected_groups_.insert(p);
          para.total_selected_groups_.insert(p);
        }
      } else{
      }
      //MCE_DEBUG("RotationHandler::handle() --> user " << para.userProfile_.id() << ", adzone_id = " << para.adzone_->id() << ", got group number = " << rotate_set.size());
    } else{
      //MCE_DEBUG("RotationHandler::handle() --> user " << para.userProfile_.id() << ", adzone_id = " << para.adzone_->id() << ", no rotate group, para.adzone_->GetRotateSize() = " << para.adzone_->GetRotateSize() << ", para.rotate_index_ = " << para.rotate_index_ << ", para.adzone_->self_rotation_number() = " << para.adzone_->self_rotation_number());
      if ((para.adzone_->member_type() == 0) && ((para.adzone_->GetRotateSize() - para.rotate_index_) <= para.adzone_->self_rotation_number())) {
        MCE_INFO("AdMatchNManagerI::GetAds --> need ad from AdEngine/Tw: uid = " << para.userProfile_.id() 
            << ", zone_id = " << para.adzone_->id() << ", rotate_index = " << para.rotate_index_);
        AdInfoSeq ad_info_seq;
        AdInfo ad_info;
        ad_info.groupid = -1;
        ad_info_seq.push_back(ad_info);
        result[para.adzone_->id()] = ad_info_seq;
        if(100000000072 == para.adzone_->id()) {
          //MCE_INFO("[REMAIN] RotationHandler::handle() --> user " << para.userProfile_.id() << ", set flag ENGINE_U");
          para.remain_state_ = ENGINE_U;
        }
        //MCE_INFO("[REMAIN] RotationHandler::handle() --> user " << para.userProfile_.id() << ", adzone_id = " << para.adzone_->id() << ", need ad from AdEngine/Tw");
        return false;
      }
    }
  }
  return true;
}

bool DefaultHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  if (para.PickCount() > 0) {
    if(100000000070 == para.adzone_->id()) {//日志页，请求自助
      AdInfoSeq ad_info_seq;
      AdInfo ad_info;
      ad_info.groupid = -2;
      ad_info_seq.push_back(ad_info);
      result[para.adzone_->id()] = ad_info_seq;
      return false;
    } else if(100000000073 == para.adzone_->id() && DEFAULT == para.remain_state_) {
      AdInfoSeq ad_info_seq;
      AdInfo ad_info;
      ad_info.groupid = -2;
      ad_info_seq.push_back(ad_info);
      //抹去72的默认
      result[100000000072] = ad_info_seq;
      para.default_log72_.clear();
      ad_info_seq.clear();
      ad_info.groupid = 0;
      ad_info_seq.push_back(ad_info);
      result[para.adzone_->id()] = ad_info_seq;
      return false;
    } else if(100000000072 == para.adzone_->id()) {
      para.remain_state_ = DEFAULT;
      AdInfoSeq ad_info_seq;
      AdInfo ad_info;
      ad_info.groupid = -3;
      ad_info_seq.push_back(ad_info);
      result[para.adzone_->id()] = ad_info_seq;
      MCE_DEBUG("DefaultHandler::handle() --> only 72 need call engine_u");
      return false;
    } else if(0 == para.adzone_->member_type()) {//允许展示自助的广告位
      AdInfoSeq ad_info_seq;
      AdInfo ad_info;
      ad_info.groupid = -3;
      ad_info_seq.push_back(ad_info);
      result[para.adzone_->id()] = ad_info_seq;
      return false;
    } 
    set<AdGroupPtr> default_groups;
    default_groups = para.adzone_->GetDefaultGroups(para.PickCount());

    for(set<AdGroupPtr>::iterator it = default_groups.begin(); it != default_groups.end(); ++it) {
      SelectedAdGroupPtr p = new SelectedAdGroup(*it);
      p->pos_ = para.zone_selected_groups_.size() + 1;  //cost = 0
      para.zone_selected_groups_.insert(p);
      para.total_selected_groups_.insert(p);
    }
  }

  return true;
}

bool LogHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  TimeStat subts;
  AdInfoSeq ad_info_seq;

  UserProfile & profile = para.userProfile_;
  ostringstream log;
  ostringstream engine_log;

  int abs_pos = 1;
  MyUtil::LongSeq group_id_seq;
  vector<SelectedAdGroupPtr> zone_selected(para.zone_selected_groups_.begin(), para.zone_selected_groups_.end());
  for(vector<SelectedAdGroupPtr>::iterator sit = zone_selected.begin();
      sit != zone_selected.end(); ++sit, ++abs_pos){
    AdInfo ad_info;
    MyUtil::Date date(time(NULL));
    AdGroupPtr group_ptr = (*sit)->group_;

    group_id_seq.push_back(group_ptr->group_id());

    if(!group_ptr->creative_ids().empty()){
      int next_creative_index = 0;
      if(para.adzone_->GetRotateSize()){
        next_creative_index = para.rotate_index_ / para.adzone_->GetRotateSize();
      }
      //ad_info.creativeid = group_ptr->GetNextCreative(next_creative_index);
      ad_info.creativeid = group_ptr->GetCreative(para.userProfile_.id(), next_creative_index);
    } else{
      ad_info.creativeid = 0;
    }
    ad_info.groupid = group_ptr->group_id();
    ad_info.updateTime = 0;
    LogParas log_para;
    log_para.cost_ = 0; //(*sit)->cost_;
    log_para.user_profile_ = para.userProfile_; //profile;
    log_para.pv_time_ = Date(time(NULL)).time();
    log_para.creative_id_ = ad_info.creativeid;
    log_para.max_price_ = group_ptr->max_price();
    log_para.zone_id_ = para.adzone_->id();
    log_para.ad_count_ = para.adzone_->brand_count();
    log_para.next_load_time_ = para.next_load_time_;
    log_para.refresh_idx_ = para.refresh_idx_;
    if(!(*sit)->append_info_.empty()) {
      log_para.append_info_ = (*sit)->append_info_;
    }
    //展示的绝对位置
    log_para.pos_ = abs_pos;
    ad_info.text = LogHelper::instance().MakeText(log_para);
    ad_info_seq.push_back(ad_info);

    //call AdLogAdapter
    Ice::Long zone_id = log_para.zone_id_;
    Ice::Long creativeid = log_para.creative_id_;
    string http = LogHelper::instance().MakeHttp(log_para);
    string ip = LogHelper::instance().MakeIp(profile.ip());

    string deal_referer = para.referer_;
    LogHelper::instance().Process(deal_referer);

    log << EngineConfig::instance().server_name() << "||" << ip << "||" << date.toDateTime() << "||\"GET " << http << " HTTP/1.0\"||200||undefined||undefined||undefined||2||1||none||" << deal_referer << "||" << zone_id <<
        "^B^B^B^B^B^B^B||" << creativeid << "^B" << group_ptr->member_id() << "^B" << group_ptr->bid_unit_id()
        << "^B0^B" << group_ptr->max_price() << "^B" << deal_referer << "^B" << abs_pos << "^B1^B2||4";
    if((!log_para.append_info_.empty()) && (log_para.append_info_.compare("-") != 0)) {
      log << "||" << log_para.append_info_;
    }

    if(100000000072 == para.adzone_->id() && para.remain_state_ == DEFAULT) {
      MCE_DEBUG("[REMAIN] LogHandler::handle() --> 72 default ad pv log: " << log.str());
      para.default_log72_.push_back(log.str());
    } else {
      MCE_DEBUG("[REMAIN] LogHandler::handle() --> normal pv log: " << log.str());
      para.pv_log_seq_.push_back(log.str());
    }
    log.str("");
  }

  result[para.adzone_->id()] = ad_info_seq;

/*
  try {
    subts.reset();
    AdLogAdapter::instance().PvBySeq(pv_log_seq);
  } catch (Ice::Exception& e) {
    MCE_WARN("LogHandler::handle AdLogAdapter.PvBySeq ERR line:"
            << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LogHandler::handle AdLogAdapter.PvBySeq ERR line:"
            << __LINE__ << " err:" << e.what());
  }
*/
  return true;
}
 
bool OnlyFirstHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  int uid = para.userProfile_.id();
  //MCE_DEBUG("OnlyFirstHandler::handle() --> uid = " << uid << ", First = " << para.first_);
  if(para.first_) {
    AdGroupPtr group = para.adzone_->only_first(para.userProfile_);
    if(group) {// && group->bid_unit()->Match()
      //MCE_DEBUG("OnlyFirstHandler::handle() --> uid = " << uid << ", got group = " << group->group_id());
      SelectedAdGroupPtr sp = new SelectedAdGroup(group);
      sp->pos_ = 1;
      para.zone_selected_groups_.insert(sp);
      para.total_selected_groups_.insert(sp);
    }
    return true;
  } else {
    return true;
  }
}

}
}

