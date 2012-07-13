#include  <map>
#include <vector>
#include "feed/rfeed_adapter.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedxml.h"
#include "ctemplate/template.h"
#include "ctemplate/template_dictionary.h"
#include "cwf/site.h"
#include "feed/expandfeed.h"
#include "base/logging.h"

namespace xce {

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

  std::vector<LogicAction*> sub_actions_;
  std::vector<std::string> sub_action_names_;
  LogicAction* parent_action_; 
  std::string parent_action_name_;

  bool BuildSubDict() {
    bool ret = true;
    for (std::vector<LogicAction*>::iterator it = sub_actions_.begin(); it != sub_actions_.end(); it++) {
      if (it == NULL || !(*it)->BuildDictionary()) {
        ret = false; 
      }
    }    

    return false;
  } 

public:
  LogicAction(std::vector<std::string> logic_parts, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin):logic_parts_(logic_parts), feed_(feed), fx_(fx), dict_(dict), u_(u), source_(source), owner_(owner), likeinfo_(likeinfo), template_file_(template_file), index_(index), is_admin_(is_admin), parent_action_(NULL), sec_(NULL) { }

  const std::string & section_name() {
    return section_name_;
  }

  const std::string &  parent_action_name() {
    return parent_action_name_;
  } 

  const std::vector<std::string> & sub_action_names() {
    return sub_action_names_;
  }

  void set_parent_action(LogicAction * p_action) {
    parent_action_ = p_action;
  }

  void AddSubActions(LogicAction * s_action) {
    sub_actions_.push_back(s_action);
  }

  virtual bool BuildDictionary() = 0;  

  static std::map<std::string, LogicAction*> ParseLogicStr(const std::string & logic_str, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin) {

    std::map<std::string, LogicAction*> ret_map;

    if (logic_str.empty()) {
      return ret_map;
    }

    std::vector<std::string> logic_str_vec;
    boost::split(logic_str_vec, logic_str, boost::is_any_of("@"), boost::token_compress_off);

    for (std::vector<std::string>::iterator it = logic_str_vec.begin(); it != logic_str_vec.end(); it++) {
      std::string logic = *it;
      std::vector<std::string> logic_parts;
      boost::split(logic_parts, logic, boost::is_any_of("#"), boost::token_compress_off);
      
      std::string logic_type_str = logic_parts[0];
      int logic_type = -1;
      try {
        logic_type = boost::lexical_cast<int>(logic_type_str);
      } catch (boost::bad_lexical_cast& e) {
        LOG_F(WARNING) << e.what();
        ret_map.resize(0);
        return ret_map;
      } 
 
      switch (logic_type) {
        CONDITION:
          if (logic_parts.size() != ConditionAction::LOGIC_VEC_SIZE) {
            LOG_F(WARNING) << "feed:" << feed->feed << " logic_type:" << logic_type << " size:" << logic_parts.size() << " logic:" << logic << "|"; 
            ret_map.resize(0);
            return ret_map;
          }
          LogicAction* logic_ptr = new ConditionAction(logic_parts, feed, fx, dict, u, source, owner, likeinfo, template_file, index, isAdmin);
          ret_map.insert(make_pair(logic_ptr->section_name(), logic_ptr));
         
          break; 

        TOW_WAY_CONDITION:
          break; 
        LOOP:
          break; 
        HASSIMILAR:
          break; 
        default:
          break; 
      } 
  
    }
   
    for (std::map<std::string, LogicAction*>::iterator map_it = ret_map.begin(); map_it != ret_map.end(); map_it++) {
      LogicAction * parent_logic_ptr = map_it->second;
      if (parent_loigc_ptr ==  null) {
        LOG_F(WARNING) << "feed:" << feed->feed << " logic_type:" << " no parent logic:" << map_it->first;
        ret_map.resize(0);
        return ret_map;
      }
      const std::vector<std::string> & sub_logic_vec = map_it->second->sub_action_names();
      for(const std::vector<std::string>::iterator vec_it = sub_logic_vec.begin(); vec_it != sub_logic_vec.end(); vec_it++) {
        std::string sub_logic_name = *vec_it;
        std::map<std::string, LogicAction*>::iterator sub_logic_map_it = ret_map.find(sub_logic_name);
        if (sub_logic_map_it != ret_map.end()) {
          LogicAction * sub_logic_ptr = sub_logic_map_it->second;
          sub_logic_ptr->set_parent_action(parent_logic_ptr);
          parent_logic_prt->AddSubActions(sub_logic_ptr); 
        } else {
          LOG_F(WARNING) << "feed:" << feed->feed << " logic_type:" << logic_type << " no sub logic:" << sub_logic_name;
          ret_map.resize(0);
          return ret_map;
        }
         
      } 
    }  

    for(std::map<std::string, LogicAction*>::iterator map_it = ret_map.begin(); map_it != ret_map.end(); map_it++) {
      LogicAction * logic_ptr = map_it->second;
      std::string p_name = logic_ptr->parent_action_name();
      if (!p_name.empty()) {
        ret_map.erase(map_it);
      }
    }
      

    return ret_map; 

  }
      
};


class ConditionAction: public LogicAction {
private:
  std::vector<std::string> condition_vars_;
  std::vector<std::string> section_vars_;
  
  
    
public:
 
  const int LOGIC_VEC_SIZE = 6;  

  ConditionAction(std::vector<std::string> logic_parts, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin): LogicAction(logic_parts, feed, fx, dict, u, source, owner, likeinfo, template_file, index, is_admin) { 
 
    section_name_ = logic_parts[1];
    parent_action_name_ = logic_parts[2];

    boost::split(condition_vars_, logic_parts[3], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(section_vars_, logic_parts[4], boost::is_any_of("$"), boost::token_compress_off);
    boost::split(sub_action_names_, logic_parts[5], boost::is_any_of("$"), boost::token_compress_off);

  }

  void AddConditionVars(const std::vector<std::string> & condition_vars) {
    condition_vars_ = condition_vars;
  } 

  void AddSectionVars(const std::vector<std::string> & section_vars) {
    section_vars_ = section_vars;
  }

  virtual bool BuildDictionary() {
    bool ret = true;
    bool flag = true;
    for (std::vector<std::string>::iterator it = condition_vars_.begin(); it != condition_vars_.end(); it++) {
      std::string condition_value = fx_->find(it->c_str());
      if (condition_value.empty()) {
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

}
