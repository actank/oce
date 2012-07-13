#ifndef __LOGICACTION_H__
#define __LOGICACTION_H__
#include <map>
#include <vector>
#include "feed/rfeed_adapter.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedview/feedxml.h"
#include "ctemplate/template.h"
#include "ctemplate/template_dictionary.h"
#include "feed/cwf_feed/site.h"
#include "feed/feedview/expandfeed.h"
#include "feed/base_feed/logging.h"
#include "boost/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"

namespace xce {

enum LogicType { CONDITION, TWO_WAY_CONDITION, LOOP, HASSIMILAR};

class LogicAction {

protected:
  std::string section_name_;
  ctemplate::TemplateDictionary * sec_;
  std::vector<std::string> logic_parts_;

  const feed::FeedData* feed_;
  int index_;
  FeedXml* fx_;
  ctemplate::TemplateDictionary* dict_;
  cwf::User* u_;
  WeiredSource source_;
  int owner_;
  const ilikeit::LikeInfo* likeinfo_;
  const std::string& template_file_;
  const bool is_admin_; 

  std::vector<boost::shared_ptr<LogicAction> > sub_actions_;
  std::vector<std::string> sub_action_names_;
  //boost::shared_ptr<LogicAction> parent_action_; 
  std::string parent_action_name_;

  bool BuildSubDict() {
    bool ret = true;
    for (std::vector< boost::shared_ptr<LogicAction> >::iterator it = sub_actions_.begin(); it != sub_actions_.end(); it++) {
      if ((*it).get()) {
        (*it)->set_dict(sec_);   ///////////////////
      } else {
        if (feed_) {
          LOG_F(WARNING) << "LogicAction::BuildSubDict --> feed:" << feed_->feed;
        }
        LOG_F(WARNING) << "LogicAction::BuildSubDict --> parent section_name_:" << section_name_ << " sub LogicAction null !";
        return false;
      }

      if (!(*it)->BuildDict()) {
        return false;
      }    

    }

    return true;
  }

  void set_dict(ctemplate::TemplateDictionary * dictionary) {
    dict_ = dictionary;
  } 




  virtual bool ParseSubAction(std::map<std::string, boost::shared_ptr<LogicAction> > & action_map) {
    LOG(INFO) << "LogicAction::ParseSubAction";
    const std::vector<std::string> & sub_logic_vec = sub_action_names();
    for(std::vector<std::string>::const_iterator vec_it = sub_logic_vec.begin(); vec_it != sub_logic_vec.end(); vec_it++) {
      std::string sub_logic_name = *vec_it;
      std::map<std::string, boost::shared_ptr<LogicAction> >::iterator sub_logic_map_it = action_map.find(sub_logic_name);
      if (sub_logic_map_it != action_map.end()) {
        boost::shared_ptr<LogicAction> sub_logic_ptr = sub_logic_map_it->second;
        if (sub_logic_ptr.get() == NULL) {
          LOG_F(WARNING) << "LogicAction::ParseSubAction --> sub action is NULL !" << sub_logic_name;
          action_map.clear();
          return false;
        }

        //boost::shared_pr<LogicAction> parent_logic_ptr = shared_from_this();
        //sub_logic_ptr->set_parent_action();
        AddSubActions(sub_logic_ptr);
      } else {
        LOG_F(WARNING) << "LogicAction::ParseSubAction --> no logic !" << sub_logic_name;
        action_map.clear();
        return false;
      }
    }
    return true;
    
  }

public:
  LogicAction(const std::vector<std::string> & logic_parts, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin):logic_parts_(logic_parts), feed_(feed), fx_(fx), dict_(dict), u_(u), source_(source), owner_(owner), likeinfo_(likeinfo), template_file_(template_file), index_(index), is_admin_(is_admin), sec_(NULL) { }


  static std::map<std::string, boost::shared_ptr<LogicAction> > ParseLogicStr(const std::string & logic_str, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin); 

  static const int LOGIC_VEC_SIZE = 6;  

  const std::string & section_name() {
    return section_name_;
  }

  const std::string &  parent_action_name() {
    return parent_action_name_;
  } 

  const std::vector<std::string> & sub_action_names() {
    return sub_action_names_;
  }
/*
  void set_parent_action(boost::shared_ptr<LogicAction> p_action) {
    parent_action_ = p_action;
  }
*/
  void AddSubActions(boost::shared_ptr<LogicAction> s_action) {
    sub_actions_.push_back(s_action);

  }

  virtual bool BuildDict() = 0; 


      
};


class ConditionAction: public LogicAction {
private:
  std::vector<std::string> condition_vars_;
  std::vector<std::string> section_vars_;
  
  
    
public:
 

  ConditionAction(const std::vector<std::string> & logic_parts, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin): LogicAction(logic_parts, feed, fx, dict, u, source, owner, likeinfo, template_file, index, is_admin) { 
 
    section_name_ = logic_parts[1];
    parent_action_name_ = logic_parts[2];

    boost::split(condition_vars_, logic_parts[3], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(section_vars_, logic_parts[4], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(sub_action_names_, logic_parts[5], boost::is_any_of("$"), boost::token_compress_off);

  }

/*
  void AddConditionVars(const std::vector<std::string> & condition_vars) {
    condition_vars_ = condition_vars;
  } 

  void AddSectionVars(const std::vector<std::string> & section_vars) {
    section_vars_ = section_vars;
  }
*/

  virtual bool BuildDict() {
    bool ret = true;
    bool flag = true;
   
    if (!dict_) {
      LOG_F(WARNING) << "ConditionAction::BuildDict --> dict_ is null !";
      return false;
    }


    if (section_name_.empty() || 0 == condition_vars_.size()) {
      LOG_F(WARNING) << "ConditionAction::BuildDict --> section_name_ is empty or condition_vars_ size 0 !";
      return false;
    }

    for (std::vector<std::string>::iterator it = condition_vars_.begin(); it != condition_vars_.end(); it++) {
      std::string condition_value = fx_->find(it->c_str());
      if (condition_value.empty()) {
        LOG_F(WARNING) << "ConditionAction::BuildDict --> condition_var:" << *it << " value is empty!";
        flag = false;
        break;
      }
    } 
  
    if (flag) {
      sec_ = dict_->AddSectionDictionary(section_name_);

      for (std::vector<std::string>::iterator it = section_vars_.begin(); it != section_vars_.end(); it++) {
        std::string var_name = *it;
        std::string var_value = fx_->find(var_name.c_str());
        sec_->SetValue(var_name, var_value);
      }      
      
    } 

        
    if (!BuildSubDict()) {
      ret = false; 
    } 
    return ret; 
  }

};



class TwoWayConditionAction: public LogicAction {
private:
  std::vector<std::string> condition_vars_;
  std::vector<std::string> section_vars_;
  
  
  std::vector<boost::shared_ptr<LogicAction> > sub_actions2_;
  std::vector<std::string> sub_action_names2_;
  std::vector<std::string> section_vars2_;

   
  void AddSubActions2(boost::shared_ptr<LogicAction> s_action) {
    sub_actions2_.push_back(s_action);

  }

  const std::vector<std::string> & sub_action_names2() {
    return sub_action_names2_;
  }

  bool BuildSubDict2() {
    bool ret = true;
    for (std::vector< boost::shared_ptr<LogicAction> >::iterator it = sub_actions2_.begin(); it != sub_actions2_.end(); it++) {
      if (!((*it).get()) || !((*it)->BuildDict())) {
        if (feed_) {
          LOG_F(WARNING) << "TwoWayConditionAction::BuildSubDict2 --> feed:" << feed_->feed;
        }
        LOG_F(WARNING) << "TwoWayConditionAction::BuildSubDict2 --> parent section_name_:" << section_name_ << " sub section_name null or build sub dict error !";
        ret = false; 
      }
    }    

    return false;
  } 
    
  virtual bool ParseSubAction(std::map<std::string, boost::shared_ptr<LogicAction> > & action_map) {  // override
    LOG(INFO) << "TwoWayConditionAction::ParseSubAction";
    if (!LogicAction::ParseSubAction(action_map)) {
      return false; 
    }

    const std::vector<std::string> & sub_logic_vec2 = sub_action_names2();
    for(std::vector<std::string>::const_iterator vec_it2 = sub_logic_vec2.begin(); vec_it2 != sub_logic_vec2.end(); vec_it2++) {
      std::string sub_logic_name2 = *vec_it2;
      std::map<std::string, boost::shared_ptr<LogicAction> >::iterator sub_logic_map_it2 = action_map.find(sub_logic_name2);
      if (sub_logic_map_it2 != action_map.end()) {
        boost::shared_ptr<LogicAction> sub_logic_ptr2 = sub_logic_map_it2->second;
        if (sub_logic_ptr2.get() == NULL) {
          LOG_F(WARNING) << "TwoWayConditionAction::ParseSubAction --> sub action is NULL !" << sub_logic_name2;
          action_map.clear();
          return false;
        }

        //boost::shared_pr<LogicAction> parent_logic_ptr = shared_from_this();
        //sub_logic_ptr->set_parent_action();
        AddSubActions2(sub_logic_ptr2);
      } else {
        LOG_F(WARNING) << "TwoWayConditionAction::ParseSubAction --> no logic !" << sub_logic_name2;
        action_map.clear();
        return false;
      }
    }
    return true;
    
  }

public:
 

  TwoWayConditionAction(const std::vector<std::string> & logic_parts, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin): LogicAction(logic_parts, feed, fx, dict, u, source, owner, likeinfo, template_file, index, is_admin) { 
 
    section_name_ = logic_parts[1];
    parent_action_name_ = logic_parts[2];

    boost::split(condition_vars_, logic_parts[3], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(section_vars_, logic_parts[4], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(sub_action_names_, logic_parts[5], boost::is_any_of("$"), boost::token_compress_off);

   
    boost::split(section_vars2_, logic_parts[6], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(sub_action_names2_, logic_parts[7], boost::is_any_of("$"), boost::token_compress_off);

  }

  static const int LOGIC_VEC_SIZE = 8;  
/*
  void AddConditionVars(const std::vector<std::string> & condition_vars) {
    condition_vars_ = condition_vars;
  } 

  void AddSectionVars(const std::vector<std::string> & section_vars) {
    section_vars_ = section_vars;
  }
*/

  virtual bool BuildDict() {
    bool ret = true;
    bool flag = true;

    if (!dict_) {
      LOG_F(WARNING) << "TwoWayConditionAction::BuildDict --> dict_ is null !";
      return false;
    }

    if (section_name_.empty() || 0 == condition_vars_.size()) {
      LOG_F(WARNING) << "TwoWayConditionAction::BuildDict --> section_name_ is empty or condition_vars_ size 0 !";
      return false;
    }

    for (std::vector<std::string>::iterator it = condition_vars_.begin(); it != condition_vars_.end(); it++) {
      std::string condition_value = fx_->find(it->c_str());
      if (condition_value.empty()) {
        LOG_F(WARNING) << "TwoWayConditionAction::BuildDict --> condition_var:" << *it << " value is empty!";
        flag = false;
        break;
      }
    } 
  
    if (flag) {
      sec_ = dict_->AddSectionDictionary(section_name_);

      for (std::vector<std::string>::iterator it = section_vars_.begin(); it != section_vars_.end(); it++) {
        std::string var_name = *it;
        std::string var_value = fx_->find(var_name.c_str());
        sec_->SetValue(var_name, var_value);
      }      
      
    } else {
      
      std::string other_sec_name = section_name_ + "_NOT"; 
      sec_ = dict_->AddSectionDictionary(other_sec_name);

      for (std::vector<std::string>::iterator it = section_vars2_.begin(); it != section_vars2_.end(); it++) {
        std::string var_name = *it;
        std::string var_value = fx_->find(var_name.c_str());
        sec_->SetValue(var_name, var_value);
      }      
    } 

    if (flag) {
      if (!BuildSubDict()) {
        ret = false; 
      } 
    } else {
      if (!BuildSubDict2()) {
        ret = false;
      }
    }     
    return ret; 
  }

};


}

#endif

