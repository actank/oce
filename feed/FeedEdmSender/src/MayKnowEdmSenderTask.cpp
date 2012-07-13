#include "MayKnowEdmSenderTask.h"
#include "QueryRunner.h"
#include "XmppTools.h"
#include "PUKFeedCacheAdapter.h"
#include "FeedAssistantAdapter.h"
#include "UserBaseAdapter.h"
#include "ServiceI.h"
#include "pugixml.hpp"
namespace xce {
namespace edm {
using namespace ::xce::socialgraph;
using namespace xce::feed;
using namespace pugi;
using namespace com::xiaonei::talk::util;

void MayKnowEdmSenderTask::handle() {
  //MCE_INFO("start MayKnowEdmSenderTask,user_id:" << user_id_);
  if (user_id_ < 0) {
    return;
  }
  if (MiscEdmSenderCache::instance().OverCount(user_id_, kMayKnow)) {
    return;
  }
  //MCE_INFO("May know sender Task:pass OverCount");
  Send();
}
void MayKnowEdmSenderTask::Send() {
  // get friend_id, common_id
  PUKFeedSeq data_list;
  try {
    data_list = PUKFeedCacheAdapter::instance().GetAndRemove(user_id_, 1);
  } catch (Ice::Exception& e) {
    MCE_WARN("get puk data from PUKFeedCacheAdapter failed, err->" << e);
  }
  //MCE_INFO("MayKnowEdmSenderTask,data_list size:" << data_list.size());
  if (data_list.empty()) {
    return;
  }
  PUKFeed data = data_list.at(0);
  int friend_id = data.friendId;
  int common_id = data.commonId;
  // get friend,common user info
  ::xce::adapter::userbase::UserBaseFullViewPtr friend_user_base_info_ptr = 0;
  try {
    friend_user_base_info_ptr = ::xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseFullView(friend_id);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserBaseAdapter error,can't get user base common view,user id = "  <<  friend_id << ", err->" << e);
  }
  if (friend_user_base_info_ptr == 0) {
    return;
  }
  std::string friend_name = friend_user_base_info_ptr->name();
  std::string friend_tiny_full_url = buildHeadUrl(friend_user_base_info_ptr->tinyurl());
  ::xce::adapter::userbase::UserBaseFullViewPtr common_user_base_info_ptr = 0;
  try {
    common_user_base_info_ptr = ::xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseFullView(common_id);
  } catch (Ice::Exception& e) {
    MCE_WARN("UserBaseAdapter error,can't get user base common view,user id = " + common_id << ", err->" << e);
  }
  if (common_user_base_info_ptr == 0) {
    return;
  }
  std::string common_name = common_user_base_info_ptr->name();
  std::string common_tiny_full_url = buildHeadUrl(common_user_base_info_ptr->tinyurl());
  // gen feed seed
  FeedSeedPtr feed_seed = new FeedSeed();
  Ice::Long source = (Ice::Long)time(NULL);
  int feed_type = 8009;
  int feed_parent_type = 8009;
  int type = feed_type&0xffff;
  type |= 1<<28;
  type |= 1<<30;
  type |= 0<<26;
  Ice::Long shifter = 0x4000000000000000LL;
  Ice::Long type_long= (Ice::Long)8009;
  Ice::Long source_merge = (Ice::Long)8009;
  type_long <<= 49;
  Ice::Long merge = type_long|source_merge;
  merge |= shifter;
  feed_seed->miniMerge = merge;
  feed_seed->newsMerge = merge;
  feed_seed->source = source;
  feed_seed->psource = source;
  feed_seed->actor = user_id_;
  feed_seed->type = type;
  feed_seed->ptype = feed_parent_type;
  feed_seed->baseWeight = 15;
  feed_seed->time = (int)time(NULL);
    // format xml
  xml_document doc;
  xml_node f_node = doc.append_child();
  f_node.set_name("f");
  f_node.append_attribute("v") = "1.0";
  f_node.append_attribute("type") = "edmAddFriend";
  xml_node time_node = f_node.append_child();
  time_node.set_name("time");
  std::string time_string = boost::lexical_cast<string>(feed_seed->time);
  MCE_DEBUG("time:" << feed_seed->time << ",string _time:" << time_string.c_str());
  time_node.append_child(node_pcdata).set_value(time_string.c_str());
  // feed uses quantifier ms,now time's quantifier is s,so times 1000
  time_node.append_child(node_pcdata).set_value("000");
  xml_node type_node = f_node.append_child();
  type_node.set_name("type");
  std::string type_string = boost::lexical_cast<string>(feed_seed->type);
  type_node.append_child(node_pcdata).set_value(type_string.c_str());
  xml_node entry_node = f_node.append_child();
  entry_node.set_name("entry");
  xml_node from_node = entry_node.append_child();
  from_node.set_name("from");
  xml_node from_id_node = from_node.append_child();
  from_id_node.set_name("id");
  std::string friend_id_string = boost::lexical_cast<string>(friend_id);
  from_id_node.append_child(node_pcdata).set_value(friend_id_string.c_str());
  xml_node from_name_node = from_node.append_child();
  from_name_node.set_name("name");
  from_name_node.append_child(node_pcdata).set_value(friend_name.c_str());
  xml_node from_img_node = from_node.append_child();
  from_img_node.set_name("tinyimg");
  from_img_node.append_child(node_pcdata).set_value(friend_tiny_full_url.c_str());
  xml_node friend_node = entry_node.append_child();
  friend_node.set_name("friend");
  xml_node friend_id_node = friend_node.append_child();
  friend_id_node.set_name("id");
  std::string common_id_string = boost::lexical_cast<string>(common_id);
  friend_id_node.append_child(node_pcdata).set_value(common_id_string.c_str());
  xml_node friend_name_node = friend_node.append_child();
  friend_name_node.set_name("name");
  friend_name_node.append_child(node_pcdata).set_value(common_name.c_str());
  xml_node friend_img_node = friend_node.append_child();
  friend_img_node.set_name("head");
  friend_img_node.append_child(node_pcdata).set_value(common_tiny_full_url.c_str());
  feed_seed->xml = doc.xml();
  //MCE_INFO(feed_seed->xml);
  std::ostringstream expression;
  expression << "u(" << feed_seed->actor << ")";
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("base_weight_id",boost::lexical_cast<string>(feed_seed->baseWeight).c_str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("expr",expression.str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_mini","0"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_news","1"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("send_config","0"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("update_time","0"));
  try {
    FeedAssistantAdapter::instance().dispatch(feed_seed);
  } catch (Ice::Exception& e) {
    MCE_WARN("send may know edm by FeedAssistantAdapter failed, err->" << e);
  }
  MCE_INFO("mayKnow userID:" << user_id_ << " friendID:" << friend_id << " commonID:" << common_id);
  MiscEdmSenderCache::instance().Add(user_id_,kMayKnow);
  return;
}

}
}
