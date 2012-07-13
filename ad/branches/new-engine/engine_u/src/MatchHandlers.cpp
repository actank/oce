#include "MatchHandlers.h"
#include "AdMatchManagerI.h"
#include "Log.h"
#include "Date.h"
#include "AdLogSAdapter.h"

using namespace xce::ad;

namespace xce {
namespace ad {

bool RotationHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  //查找轮播广告
  //轮播广告支持不止一个，需要支持指定个数的函数接口（优先级低）
  if (para.PickCount() > 0) {
    AdGroupPtr rotate_group_ptr = para.adzone_->GetRotateGroups(para.rotate_index_, para.userProfile_);

    if(rotate_group_ptr){
      set<AdGroupPtr> rotate_set;
      rotate_set.insert(rotate_group_ptr);
      Filter::instance().FilterCompete(para.userProfile_.id(), rotate_set, para.zone_selected_groups_);
      if(!rotate_set.empty()){

        //update selected
        SelectedAdGroupPtr p = new SelectedAdGroup(rotate_group_ptr);
        p->pos_ = 1;
//        para.zone_selected_groups_.insert(rotate_set.begin(), rotate_set.end());
//        para.total_selected_groups_.insert(rotate_set.begin(), rotate_set.end());
        para.zone_selected_groups_.insert(p);
        para.total_selected_groups_.insert(p);
      } else{
      }
    } else{
      MCE_INFO("[MAIN_PROCESS] RotationHandler::handle --> Get ROTATE ad from AdZone : failed");
      if ((para.adzone_->member_type() == 0) && ((para.adzone_->GetRotateSize()- para.rotate_index_) <= para.adzone_->self_rotation_number())) {
        MCE_INFO("AdMatchNManagerI::GetAds --> need ad from AdEngine/Tw: uid = " << para.userProfile_.id() 
            << ", zone_id = " << para.adzone_->id() << ", rotate_index = " << para.rotate_index_);
        AdInfoSeq ad_info_seq;
        AdInfo ad_info;
        ad_info.groupid = -1;
        ad_info_seq.push_back(ad_info);
        result[para.adzone_->id()] = ad_info_seq;
        // ans[9999963] = AdInfoSeq(); // old code, to delete
        // continue;
        return false;
      }
    }
  }
  STATISTICS("RotationHandler::handle --> time = " << ts.getTime());
  return true;
}

bool DefaultHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  //MCE_DEBUG("[MAIN_PROCESS] DefaultHandler::handle ==>NO DEFAULT  line:"<<__LINE__<<",adzone:"<< para.adzone_->id())
  TimeStat ts;
  if (para.PickCount() > 0) {
    set<AdGroupPtr> default_groups;
    if(100000000072 != para.adzone_->id() && 100000000070 != para.adzone_->id()
      && 100000000063 != para.adzone_->id() && 100000000001 != para.adzone_->id()) {
      AdZonePtr adzone = AdZonePool::instance().GetAdZone(100000000072);
   //   MCE_DEBUG("[MAIN_PROCESS] DefaultHandler::handle ==>NO DEFAULT  line:"<<__LINE__<<",adzone:" << adzone->id())
      if(adzone) {
        default_groups = adzone->GetDefaultGroups(para.PickCount());
    //    MCE_DEBUG("[MAIN_PROCESS] DefaultHandler::handle ==>NO DEFAULT  line:"<<__LINE__)
      }
    } else {
      default_groups = para.adzone_->GetDefaultGroups(para.PickCount());
     // MCE_DEBUG("[MAIN_PROCESS] DefaultHandler::handle ==>NO DEFAULT  line:"<<__LINE__)
    }
    if (default_groups.empty()) {
      MCE_INFO("[MAIN_PROCESS] DefaultHandler::handle ==>NO DEFAULT ads  is got, needed ad count = "
          << para.PickCount());
    } else {
      MCE_INFO("[MAIN_PROCESS] DefaultHandler::handle ==>Get DEFAULT ad from AdZone : get default_groups.size() = "
          << default_groups.size() << ", needed ad count = " << para.PickCount());
    }

    for(set<AdGroupPtr>::iterator it = default_groups.begin(); it != default_groups.end(); ++it) {
      SelectedAdGroupPtr p = new SelectedAdGroup(*it);
      p->pos_ = para.zone_selected_groups_.size() + 1;  //cost = 0
      para.zone_selected_groups_.insert(p);
      para.total_selected_groups_.insert(p);
    }

//    para.zone_selected_groups_.insert(default_groups.begin(), default_groups.end());
//    para.total_selected_groups_.insert(default_groups.begin(), default_groups.end());
  }

  STATISTICS("DefaultHandler::handle --> time = " << ts.getTime());
  return true;
}

static void Adjust(vector<SelectedAdGroupPtr>& groups_out, set<SelectedAdGroupPtr>& groups_in) {
  //调整
  vector<int> priority_pos = EngineConfig::instance().priority_pos();
  for(set<SelectedAdGroupPtr>::iterator sit = groups_in.begin();
      sit != groups_in.end(); ++sit) {
    int pos = priority_pos[((*sit)->pos_-1)%priority_pos.size()];
    //调整展示位置，目前是2,3位优先
    groups_out[(pos-1)%groups_out.size()] = (*sit);
  }
}

static void AdjustForInner(vector<SelectedAdGroupPtr>& groups_out, set<SelectedAdGroupPtr>& groups_in) {
  //调整
  vector<int> priority_pos = EngineConfig::instance().inner_priority_pos();
  for(set<SelectedAdGroupPtr>::iterator sit = groups_in.begin();
      sit != groups_in.end(); ++sit) {
    int pos = priority_pos[((*sit)->pos_-1)%priority_pos.size()];
    groups_out[(pos-1)%groups_out.size()] = (*sit);
  }
}

// 调整广告展示位置
bool PosAdjustHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  // 调整广告展示的位置
  vector<SelectedAdGroupPtr> zone_selected(para.zone_selected_groups_.begin(), para.zone_selected_groups_.end());
  MCE_DEBUG("ads num " <<  zone_selected.size());
  /*  if(para.adzone_->id() == 100000000001) {
      Adjust(zone_selected, para.zone_selected_groups_);
    } else if ((para.adzone_->id() == 100000000063) && para.has_re_) {
      AdjustForInner(zone_selected, para.zone_selected_groups_);
    }*/
   //调整后的
  para.final_selected_groups_.swap(zone_selected);
  STATISTICS("PosAdjustHandler::handle --> time = " << ts.getTime());
  return true;
}

//样式实验进行样式替换
bool StyleHandler::isBig( int widget_id){
	return widget_id == EngineConfig::instance().big_pic_id();
}
bool StyleHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  MCE_DEBUG("StyleHandler::handle() --> user_id = " << para.userProfile_.id()
            << ", adzone_id = " << para.adzone_->id()
            << ", para.final_selected_groups_.size() = " << para.final_selected_groups_.size());
  AdStylePtr style = para.style_;
  if( style ){
    MCE_DEBUG("style replace with styleId=" << style->style_id());
	  int abs_pos = 0;
	    vector<int> bigs = style->big_widgets();
	    vector<int> smalls = style->small_widgets();
	    for(vector<SelectedAdGroupPtr>::iterator sit = para.final_selected_groups_.begin();
	        sit != para.final_selected_groups_.end(); ++sit, ++abs_pos){
	    	AdGroupPtr group_ptr = (*sit)->group_;
        int widget_id = 0;
	    	map<Ice::Long, Ice::Long> creative2widget = group_ptr->creative2widget();
        if(!creative2widget.empty()) {
          (*sit)->selected_creative_id_ = creative2widget.begin()->first;
          widget_id = creative2widget.begin()->second;
        }
	    	int newId = 0;
	    	if( StyleHandler::isBig(widget_id) ){
	    		if( abs_pos < bigs.size() && bigs[abs_pos] > 0 )
	    		  newId = bigs[abs_pos];
	    	}else{
	    		if( abs_pos < smalls.size() && smalls[abs_pos] > 0)
	    		  newId = smalls[abs_pos];
	    	}
        MCE_DEBUG("adgroup_id " << group_ptr->group_id() << " widget replace: " << widget_id <<" --> " << newId);
	    	if( newId != 0 ){
          (*sit)->selected_widget_id_ = newId;
          (*sit)->extra_ = "style_test";
        }
	    }
  }
  STATISTICS("StyleHandler::handle --> time = " << ts.getTime());
  return true;
}

bool sortByPos(const SelectedAdGroupPtr& g1, const SelectedAdGroupPtr& g2) {
    return g1->pos_ < g2->pos_;
}
bool LogHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  TimeStat ts;
  TimeStat subts;
  AdInfoSeq ad_info_seq;

  ::MyUtil::StrSeq pv_log_seq;
  ::MyUtil::StrSeq engine_log_seq;

  UserProfile & profile = para.userProfile_;
  ostringstream log;
  ostringstream engine_log;
  MyUtil::LongSeq group_id_seq;
  vector<SelectedAdGroupPtr> zone_selected  =para.final_selected_groups_;
  std::sort(zone_selected.begin(), zone_selected.end(), sortByPos);
  for(vector<SelectedAdGroupPtr>::iterator sit = zone_selected.begin();
      sit != zone_selected.end(); ++sit){
    AdInfo ad_info;
    MyUtil::Date date(time(NULL));
    AdGroupPtr group_ptr = (*sit)->group_;
    group_id_seq.push_back(group_ptr->group_id());
    map<Ice::Long,Ice::Long> creative2widget = group_ptr->creative2widget();
    if(!group_ptr->creative_ids().empty()){
      ad_info.creativeid = *(group_ptr->creative_ids().begin());
    } else{
      ad_info.creativeid = 0;
    }
    ad_info.groupid = group_ptr->group_id();
    ad_info.updateTime = 0;
    LogParas log_para;
    log_para.cost_ = (*sit)->cost_;
    log_para.user_profile_ = para.userProfile_; //profile;
    log_para.pv_time_ = Date(time(NULL)).time();
    log_para.creative_id_ = ad_info.creativeid;
    log_para.max_price_ = group_ptr->max_price();
    log_para.zone_id_ = para.adzone_->id();
    log_para.ad_count_ = para.adzone_->bp_count();
    log_para.next_load_time_ = para.next_load_time_;
    log_para.refresh_idx_ = para.refresh_idx_;
    ostringstream append_info;
    if(!para.tag_.empty()) {
      append_info << para.tag_;
    } else {
      append_info << "rr_0";
    }
    //展示的绝对位置
    log_para.pos_ = (*sit)->pos_;
    log_para.demo_state_ = (*sit)->demo_state_;
    log_para.widget_id_ = (*sit)->selected_widget_id_;    
    append_info << '_' << log_para.widget_id_;
    log_para.append_info_ = append_info.str();
    ad_info.text = LogHelper::instance().MakeText(log_para);
    if(!(*sit)->extra_.empty()) {
      ad_info.updateTime = -1;
    } else {
      ad_info.updateTime = 0;
    }
    ad_info_seq.push_back(ad_info);

    //call AdLogAdapter
    Ice::Long zone_id = log_para.zone_id_;
    Ice::Long creativeid = log_para.creative_id_;
    string http = LogHelper::instance().MakeHttp(log_para);
    string ip = LogHelper::instance().MakeIp(profile.ip());

    string deal_referer = para.referer_;
    LogHelper::instance().Process(deal_referer);
    MCE_DEBUG("LogHandler::handle-->gid:"<<(*sit)->group_->group_id()<<",pos:" <<(*sit)->pos_);

    //int sort_type = EngineConfig::instance().sort_type();
    int sort_type = EngineConfig::instance().server_index();

    log << EngineConfig::instance().server_name() << "||" << ip << "||" << date.toDateTime() << "||\"GET " << http << " HTTP/1.0\"||200||undefined||undefined||undefined||2||" << sort_type << "||none||" << deal_referer << "||" << zone_id <<
        "^B^B^B^B^B^B^B||" << creativeid << "^B" << group_ptr->member_id() << "^B" << group_ptr->bid_unit_id()
        << "^B0^B" << group_ptr->max_price() << "^B" << deal_referer << "^B" << log_para.pos_ <<"^B" << log_para.demo_state_ << "^B2||4";
    if((!log_para.append_info_.empty()) && (log_para.append_info_.compare("-") != 0)) {
      log << "||" << log_para.append_info_;
    }

    pv_log_seq.push_back(log.str());
    log.str("");

    engine_log << "pvlog=1\tuserid=" << profile.id() << "\tage="<< profile.age()
        << "\tgender="<< profile.gender() << "\tstage=" << profile.stage()
        << "\tgrade=" << profile.grade() << "\tcurrent_area="
        <<  ((profile.current_area().empty()) ? "-" : profile.current_area())
        << "\tip_area=" << ((profile.ip_area().empty()) ? "-" : profile.ip_area())
        << "\tschool=" << profile.school() << "\tzone_id=" << para.adzone_->id()
        << "\tgroup_id=" << group_ptr->group_id() << "\tmax_price=" << group_ptr->max_price()
        << "\tapp_id=" << LogHelper::instance().GetAppId(para.referer_)
        << "\tmember_id=" << group_ptr->member_id() << "\tsystem_time=" << date.toDateTime();
    engine_log_seq.push_back(engine_log.str());
    engine_log.str("");
  }

  result[para.adzone_->id()] = ad_info_seq;

  try {
    subts.reset();
    AdLogAdapter::instance().PvBatch(pv_log_seq);
    STATISTICS("LogHandler::handle --> TIME_TEST write pv_log : "
        << subts.getTime());
  } catch (Ice::Exception& e) {
    MCE_WARN("LogHandler::handle AdLogAdapter.PvBatch ERR line:"
            << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LogHandler::handle AdLogAdapter.PvBatch ERR line:"
            << __LINE__ << " err:" << e.what());
  }

  try {
    subts.reset();
    AdLogAdapter::instance().EngineLogBatch(engine_log_seq);
    STATISTICS("LogHandler::handle --> TIME_TEST write engine_log : "
        << subts.getTime());
  } catch (Ice::Exception& e) {
    MCE_WARN("LogHandler::handle AdLogAdapter.EngineLogBatch ERR line:"
            << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LogHandler::handle AdLogAdapter.EngineLogBatch ERR line:"
            << __LINE__ << " err:" << e.what());
  }

  STATISTICS("LogHandler::handle --> time = " << ts.getTime());
  return true;
}

}
}

