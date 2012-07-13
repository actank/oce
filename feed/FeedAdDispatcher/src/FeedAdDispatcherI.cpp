#include "FeedAdDispatcherI.h"
#include <boost/algorithm/string.hpp>
#include <vector>
#include "pugixml.hpp"
#include "FeedAssistantAdapter.h"
void ::MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::feed::FeedAdDispatcherI::instance(),
      service.createIdentity("FAD", ""));
}
namespace xce {
namespace feed {
using namespace pugi;
static const std::string kConfigureTableName = "feed_ad_configure";
FeedAdDispatcherI::FeedAdDispatcherI() {
}
void FeedAdDispatcherI::Dispatch(Ice::Int ad_id, const MyUtil::Str2StrMap& params ,const Ice::Current&) {
  AdConfigurePtr ad_configure = evict_map_.locate(ad_id,0);
  if (ad_configure == 0) {
    MCE_INFO("start get ad from db");
    ad_configure = _LoadConfigureFromDB(ad_id);
    if (ad_configure == 0) {
      MCE_WARN("couldn't find ad_configure:" <<  ad_id);
      return;
    } else {
      MCE_INFO("add to cache");
      evict_map_.add(ad_configure,ad_id); 
    }
  }
  MCE_INFO("start build seed");
  FeedSeedPtr feed_seed = _BuildSeed(ad_id, params, ad_configure); 
  if (feed_seed != 0) {
    try {
      MCE_INFO("xml=" << feed_seed->xml);
      FeedAssistantAdapter::instance().dispatch(feed_seed);  
    } catch (Ice::Exception& e) {
      MCE_WARN("dispatch ad:" << ad_id << "failed,err->" << e);
    }
  }
  MCE_INFO("to send ad feed, ad_id:" << ad_id); 
}
xce::feed::FeedSeedPtr FeedAdDispatcherI::_BuildSeed(Ice::Int ad_id, const MyUtil::Str2StrMap& params, const AdConfigurePtr& ad_ptr) {
  FeedSeedPtr feed_seed = new FeedSeed();
  int feed_type = ad_ptr->stype_;
  int feed_parent_type = ad_ptr->stype_;
  int type = feed_type&0x00ffffff;
  type |= 1<<28;
  type |= 1<<30;
  type |= 0<<26;
  feed_seed->miniMerge = ad_id;
  feed_seed->newsMerge = ad_id;
  feed_seed->source = ad_id;
  feed_seed->psource = ad_id;
  MyUtil::Str2StrMap::const_iterator ite = params.find("actor");
  if (ite != params.end()) {
    Ice::Int actor_id = boost::lexical_cast<Ice::Int>(ite->second);
    feed_seed->actor = actor_id;
  } else {
    MCE_WARN("couldn't find actor");
    return 0;
  }
  feed_seed->type = type;
  feed_seed->ptype = feed_parent_type;
  feed_seed->time = (int)time(NULL);
  ite = ad_ptr->extra_props_.find("baseWeight");
  if (ite != ad_ptr->extra_props_.end()) {
    Ice::Int base_weight = boost::lexical_cast<Ice::Int>(ite->second);
    feed_seed->baseWeight = base_weight;
  } else {
    MCE_WARN("couldn't find baseWeight of ad:" << ad_id);
    feed_seed->baseWeight = 21;
  }
  xml_document doc;
  xml_node f_node = doc.append_child();
  f_node.set_name("f");
  f_node.append_attribute("v") = "1.0";
  f_node.append_attribute("type") = "edmAd";
  xml_node time_node = f_node.append_child();
  time_node.set_name("time");
  timeval now;
  gettimeofday(&now,NULL);
  Ice::Long millisecond = now.tv_sec * 1000 + now.tv_usec/1000;
  std::string time_string = boost::lexical_cast<string>(millisecond);
  MCE_DEBUG("time:" << millisecond << ",string _time:" << time_string.c_str());
  time_node.append_child(node_pcdata).set_value(time_string.c_str());
  xml_node type_node = f_node.append_child();
  type_node.set_name("type");
  std::string type_string = boost::lexical_cast<string>(feed_seed->type);
  type_node.append_child(node_pcdata).set_value(type_string.c_str());
  xml_node from_node = f_node.append_child();
  from_node.set_name("from");
  xml_node id_node = from_node.append_child();
  id_node.set_name("id");
  std::string id_string = boost::lexical_cast<string>(feed_seed->actor);
  id_node.append_child(node_pcdata).set_value(id_string.c_str());
  xml_node edm_node = f_node.append_child();
  edm_node.set_name("edm");
  std::string logo;
  ite = ad_ptr->content_data_.find("logo");
  if (ite != ad_ptr->content_data_.end()) {
    logo = ite->second;
    xml_node logo_node = edm_node.append_child();
    logo_node.set_name("logo");
    logo_node.append_child(node_pcdata).set_value(logo.c_str());
  }
  std::string title;
  ite = ad_ptr->content_data_.find("title");
  if (ite != ad_ptr->content_data_.end()) {
    title = ite->second;
    xml_node title_node = edm_node.append_child();
    title_node.set_name("title");
    title_node.append_child(node_pcdata).set_value(title.c_str());
  } else {
    MCE_WARN("couldn't find title of ad:" << ad_id);
    return 0;
  }
  std::string title_link;
  ite = ad_ptr->content_data_.find("titleLink");
  if (ite != ad_ptr->content_data_.end()) {
    title_link = ite->second;
    xml_node title_link_node = edm_node.append_child();
    title_link_node.set_name("titleLink");
    title_link_node.append_child(node_pcdata).set_value(title_link.c_str());
  } else {
    MCE_WARN("couldn't find title_link of ad:" << ad_id);
    return 0;
  }
  std::string body;
  ite  = ad_ptr->content_data_.find("body");
  if (ite != ad_ptr->content_data_.end()) {
    body = ite->second;
    xml_node body_node = edm_node.append_child();
    body_node.set_name("body");
    body_node.append_child(node_pcdata).set_value(body.c_str());
  } else {
    MCE_WARN("couldn't find body of ad:" << ad_id);
    return 0;
  }
  std::string body_link;
  ite  = ad_ptr->content_data_.find("bodyLink");
  if (ite != ad_ptr->content_data_.end()) {
    body_link = ite->second;
    xml_node body_link_node = edm_node.append_child();
    body_link_node.set_name("bodyLink");
    body_link_node.append_child(node_pcdata).set_value(body_link.c_str());
  } else {
    MCE_WARN("couldn't find body_link of ad:" << ad_id);
    return 0;
  }
  xml_node media_node = edm_node.append_child();
  media_node.set_name("media");
  std::string media_src;
  ite = ad_ptr->content_data_.find("mediaSrc");
  if (ite != ad_ptr->content_data_.end()) {
    media_src = ite->second;
    xml_node media_src_node = media_node.append_child();
    media_src_node.set_name("src");
    media_src_node.append_child(node_pcdata).set_value(media_src.c_str());
  } else {
    MCE_WARN("couldn't find media_src of ad:" << ad_id);
    return 0;
  }
  std::string media_url;
  ite = ad_ptr->content_data_.find("mediaUrl");
  if (ite != ad_ptr->content_data_.end()) {
    media_url = ite->second;
    xml_node media_url_node = media_node.append_child();
    media_url_node.set_name("url");
    media_url_node.append_child(node_pcdata).set_value(media_url.c_str());
  } else {
    MCE_WARN("couldn't find media_url of ad:" << ad_id);
    return 0;
  }
  std::string interactive;
  ite  = ad_ptr->extra_props_.find("interactive");
  if (ite != ad_ptr->extra_props_.end()) {
    interactive = ite->second;
    xml_node interactive_node = edm_node.append_child();
    interactive_node.set_name("interactive");
    interactive_node.append_child(node_pcdata).set_value(interactive.c_str());
  }
  xml_node reply_node = edm_node.append_child();
  reply_node.set_name("reply");
  xml_node reply_type_node = reply_node.append_child();
  reply_type_node.set_name("type");
  std::string reply_type;
  ite = ad_ptr->extra_props_.find("replyType");
  if (ite != ad_ptr->extra_props_.end()) {
    reply_type = ite->second;
    reply_type_node.append_child(node_pcdata).set_value(reply_type.c_str());
  }
  xml_node outer_url_node = edm_node.append_child();
  outer_url_node.set_name("outerUrl");
  xml_node config_node = edm_node.append_child();
  config_node.set_name("config");
  config_node.append_child(node_pcdata).set_value(boost::lexical_cast<string>(ad_id).c_str());
  xml_node user_node = edm_node.append_child();
  user_node.set_name("user");
  xml_node user_id_node = user_node.append_child();
  user_id_node.set_name("id");
  ite = params.find("user_id");
  if (ite != params.end()) {
    user_id_node.append_child(node_pcdata).set_value(ite->second.c_str());
  } else {
    MCE_WARN("couldn't find userId!");
    return 0;
  }
  xml_node user_name_node = user_node.append_child();
  user_name_node.set_name("name");
  std::string user_name;
  ite = params.find("user_name");
  if (ite != params.end()) {
    user_name = ite->second;
    user_name_node.append_child(node_pcdata).set_value(user_name.c_str());
  }
  std::string expose_id;
  ite = params.find("expose_id");
  if (ite != params.end()) {
    expose_id = ite->second;
    xml_node expose_id_node = edm_node.append_child();
    expose_id_node.set_name("exposeId");
    expose_id_node.append_child(node_pcdata).set_value(expose_id.c_str());
  } 
  std::string expose_num;
  ite = params.find("expose_num");
  if (ite != params.end()) {
    expose_num = ite->second;
    xml_node expose_num_node = edm_node.append_child();
    expose_num_node.set_name("exposeNum");
    expose_num_node.append_child(node_pcdata).set_value(expose_num.c_str());
  }
  feed_seed->xml = doc.xml();
  std::ostringstream expression;
  expression << "u(" << feed_seed->actor << ")";
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("base_weight_id",boost::lexical_cast<string>(feed_seed->baseWeight).c_str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("expr",expression.str()));
  ite  = ad_ptr->extra_props_.find("publishMini");
  if (ite != ad_ptr->extra_props_.end()) {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_mini",ite->second));
  } else {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_mini","0"));
  }
  ite  = ad_ptr->extra_props_.find("publishNews");
  if (ite != ad_ptr->extra_props_.end()) {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_news",ite->second));
  } else {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_news","1"));
  }
  ite  = ad_ptr->extra_props_.find("sendConfig");
  if (ite != ad_ptr->extra_props_.end()) {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("send_config",ite->second));
  } else {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("send_config","0"));  
  }
  ite  = ad_ptr->extra_props_.find("updateTime");
  if (ite != ad_ptr->extra_props_.end()) {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("update_time",ite->second));
  } else {
    feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("update_time","1"));
  } 
  return feed_seed;
}
AdConfigurePtr FeedAdDispatcherI::_LoadConfigureFromDB(Ice::Int ad_id) {
  com::xiaonei::xce::Statement sql;
  sql << "select stype,content_data,extra_props from " << kConfigureTableName << " where id = " << ad_id;
  mysqlpp::StoreQueryResult res;
  try {
    res = com::xiaonei::xce::QueryRunner("feed_db", com::xiaonei::xce::CDbRServer,kConfigureTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("load db from db :" << kConfigureTableName << " failed, err->" << e);
    return 0;
  } 
  if (res.num_rows() == 0) {
    MCE_WARN("couldn't find ad configure:"<< ad_id <<" in db!");
    return 0;
  }
  mysqlpp::Row row = res.at(0);
  return  _FormatAdConfigure(row);

}
AdConfigurePtr FeedAdDispatcherI::_FormatAdConfigure(const mysqlpp::Row& row) {
  AdConfigurePtr configure_ptr = new AdConfigure();
  configure_ptr->stype_ = row["stype"];
  std::string content_data = (std::string)row["content_data"];
  std::string extra_props = (std::string)row["extra_props"];
  std::vector<std::string> content;
  boost::iter_split(content, content_data, boost::first_finder("\t"));
  if(content.size() == 0 || content.size() %3 != 0) {
    MCE_WARN("unexpect data,cotent size:" << content.size() << ",content:" << content_data);
  }
  for(size_t i = 0; i < content.size() ; i +=3) {
    configure_ptr->content_data_.insert(std::make_pair<std::string,std::string>(content.at(i),content.at(i+1)));
  }
  std::vector<std::string> extra_prop_values;
  boost::iter_split(extra_prop_values, extra_props, boost::first_finder("\t"));
  if(extra_prop_values.size() == 0 || extra_prop_values.size() %3 != 0) {
    MCE_WARN("unexpect data,extra_props:" << extra_props);
  }
  for(size_t i = 0; i < extra_prop_values.size() ; i +=3) {
    configure_ptr->extra_props_.insert(std::make_pair<std::string,std::string>(extra_prop_values.at(i),extra_prop_values.at(i+1)));
  }
  return configure_ptr;
}
}
}
