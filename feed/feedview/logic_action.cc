#include "feed/feedview/logic_action.h"

using namespace xce;

 std::map<std::string, boost::shared_ptr<LogicAction> > LogicAction::ParseLogicStr(const std::string & logic_str, const feed::FeedData * feed, FeedXml * fx, ctemplate::TemplateDictionary * dict, cwf::User * u, WeiredSource source, int owner, const ilikeit::LikeInfo * likeinfo,  const std::string & template_file, int index, bool is_admin) {

    std::map<std::string, boost::shared_ptr<LogicAction> > ret_map;

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
        ret_map.clear();
        return ret_map;
      } 

      LogicType lt = (LogicType)logic_type;
      switch (lt) {
        case CONDITION:
        {
          if (logic_parts.size() != ConditionAction::LOGIC_VEC_SIZE) {
            if (feed) {
              LOG_F(WARNING) << "feed:" << feed->feed;
            } 
            LOG_F(WARNING) << "LogicAction::ParseLogicStr --> logic_type:" << logic_type << " size:" << logic_parts.size() << " logic:" << logic << "|"; 
            ret_map.clear();
            return ret_map;
          }
          boost::shared_ptr<LogicAction> logic_ptr(new ConditionAction(logic_parts, feed, fx, dict, u, source, owner, likeinfo, template_file, index, is_admin));
          ret_map.insert(make_pair(logic_ptr->section_name(), logic_ptr));
         
          break; 
        }
        case TWO_WAY_CONDITION:
        {
          if (logic_parts.size() != TwoWayConditionAction::LOGIC_VEC_SIZE) {
            if (feed) {
              LOG_F(WARNING) << "feed:" << feed->feed;
            } 
            LOG_F(WARNING) << "TwoWayLogicAction::ParseLogicStr --> logic_type:" << logic_type << " size:" << logic_parts.size() << " logic:" << logic << "|"; 
            ret_map.clear();
            return ret_map;
          }
          boost::shared_ptr<LogicAction> logic_ptr(new TwoWayConditionAction(logic_parts, feed, fx, dict, u, source, owner, likeinfo, template_file, index, is_admin));
          ret_map.insert(make_pair(logic_ptr->section_name(), logic_ptr));
         
          break; 
        }
        case LOOP:
          break; 
        case HASSIMILAR:
          break; 
        default:
          break; 
      } 
  
    }


    //all_actions = ret_map;
   
    for (std::map<std::string, boost::shared_ptr<LogicAction> >::iterator map_it = ret_map.begin(); map_it != ret_map.end(); map_it++) {
      boost::shared_ptr<LogicAction> parent_logic_ptr = map_it->second;
      if (parent_logic_ptr.get() ==  NULL) {
        if (feed) {
          LOG_F(WARNING) << "feed:" << feed->feed; 
        }
        LOG_F(WARNING) << "LogicAction::ParseLogicStr -->  no parent logic:" << map_it->first;
        ret_map.clear();
        return ret_map;
      }
     
      LOG(INFO) << "ParseSubAction name:" << map_it->first;
      if (!parent_logic_ptr->ParseSubAction(ret_map)) {
        ret_map.clear();
        return ret_map;
      }

/*    const std::vector<std::string> & sub_logic_vec = map_it->second->sub_action_names();
      for(std::vector<std::string>::const_iterator vec_it = sub_logic_vec.begin(); vec_it != sub_logic_vec.end(); vec_it++) {
        std::string sub_logic_name = *vec_it;
        std::map<std::string, boost::shared_ptr<LogicAction> >::iterator sub_logic_map_it = ret_map.find(sub_logic_name);
        if (sub_logic_map_it != ret_map.end()) {
          boost::shared_ptr<LogicAction> sub_logic_ptr = sub_logic_map_it->second;
          if (sub_logic_ptr.get() == NULL) {
            if(feed) {
              LOG_F(WARNING) << "feed:" << feed->feed ;
            }
            LOG_F(WARNING) << "LogicAction::ParseLogicStr --> sub logic is NULL !" << sub_logic_name; 
            ret_map.clear();
            return ret_map;
          }

          sub_logic_ptr->set_parent_action(parent_logic_ptr);
          parent_logic_ptr->AddSubActions(sub_logic_ptr); 
        } else {
          if(feed) {
            LOG_F(WARNING) << "feed:" << feed->feed ;
          }
          LOG_F(WARNING) << "LogicAction::ParseLogicStr --> no logic !" << sub_logic_name; 
          ret_map.clear();
          return ret_map;
        }
         
      } */ 
       
    }  

    for(std::map<std::string, boost::shared_ptr<LogicAction> >::iterator map_it = ret_map.begin(); map_it != ret_map.end(); map_it++) {
      boost::shared_ptr<LogicAction> logic_ptr = map_it->second;
      std::string p_name = logic_ptr->parent_action_name();
      if (!p_name.empty()) {
        ret_map.erase(map_it);
      }
    }
      

    return ret_map; 

  }
