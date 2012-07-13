#include "FriendEdmSenderTask.h"
#include "QueryRunner.h"
#include "XmppTools.h"
#include "WriteReconnectWorkerAdapter.h"
#include "FeedAssistantAdapter.h"
#include "ServiceI.h"
#include "UserBaseAdapter.h"
#include "pugixml.hpp"
namespace xce {
namespace edm {
using namespace xce::feed;
using namespace pugi;
using namespace com::xiaonei::talk::util;
using namespace xce::socialgraph;
void FriendEdmSenderTask::handle() {
  //MCE_INFO("start FriendEdmSenderTask,user_id:" << user_id_);
  if (user_id_ < 0) {
    return;
  }
  if (MiscEdmSenderCache::instance().OverCount(user_id_, kFriend)) {
    return;
  }
  //MCE_INFO("FriendEdmSenderTask:pass OverCount");
  int friend_id = GetFriendToRecommend();
  //MCE_INFO("get friend_id:" << friend_id);
  if (friend_id == 0) {
      return;
  }
  Send(friend_id);
  MyUtil::IntSeq null_seq;
  WriteReconnectWorkerAdapter::instance().removeRecommendFriends(user_id_, friend_id, null_seq);

}
void FriendEdmSenderTask::Send(const int& friend_id) {
  //MCE_INFO("Send FriendEdmSender edm");
  ::xce::adapter::userbase::UserBaseFullViewPtr user_base_info_ptr = 0;
  try {
    user_base_info_ptr = ::xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseFullView(friend_id);
  } catch (Ice::Exception& e) {
    MCE_INFO("UserBaseAdapter error,can't get user base common view,user id = " + friend_id);
  }
  if (user_base_info_ptr == 0) {
    return;
  }  
  std::string friend_name = user_base_info_ptr->name();
  std::string tiny_full_url = buildHeadUrl(user_base_info_ptr->tinyurl());
  Ice::Long source = (Ice::Long)time(NULL);
  FeedSeedPtr feed_seed = new FeedSeed();
  int feed_type = 8006;
  int feed_parent_type = 8006;
  int type = feed_type&0xffff;
  type |= 1<<28;
  type |= 1<<30;
  type |= 0<<26;
  Ice::Long shifter = 0x4000000000000000LL;
  Ice::Long type_long= (Ice::Long)8006;
  Ice::Long source_merge = (Ice::Long)8006;
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
  feed_seed->baseWeight = 21;
  feed_seed->time = (int)time(NULL);
  // format xml
  xml_document doc;
  xml_node f_node = doc.append_child();
  f_node.set_name("f");
  f_node.append_attribute("v") = "1.0";
  f_node.append_attribute("type") = "edmFriend";
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
  xml_node edm_node = f_node.append_child();
  edm_node.set_name("edm");
  xml_node owner_node = edm_node.append_child();
  owner_node.set_name("owner");
  xml_node owner_id_node = owner_node.append_child();
  owner_id_node.set_name("id");
  std::string user_id_string = boost::lexical_cast<string>(feed_seed->actor);
  owner_id_node.append_child(node_pcdata).set_value(user_id_string.c_str());
  xml_node friend_node = edm_node.append_child();
  friend_node.set_name("friend");
  xml_node friend_id_node = friend_node.append_child();
  friend_id_node.set_name("id");
  std::string friend_id_string = boost::lexical_cast<string>(friend_id);
  friend_id_node.append_child(node_pcdata).set_value(friend_id_string.c_str());
  xml_node friend_name_node = friend_node.append_child();
  friend_name_node.set_name("name");
  friend_name_node.append_child(node_pcdata).set_value(friend_name.c_str());
  xml_node friend_logo_node = friend_node.append_child();
  friend_logo_node.set_name("logo");
  friend_logo_node.append_child(node_pcdata).set_value(tiny_full_url.c_str());
  feed_seed->xml = doc.xml(); 
  //MCE_INFO(feed_seed->xml);
  std::ostringstream expression;
  expression << "u(" << feed_seed->actor << ")";
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("base_weight_id",boost::lexical_cast<string>(feed_seed->baseWeight).c_str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("expr",expression.str()));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_mini","0"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("publish_news","1"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("send_config","0"));
  feed_seed->extraProps.insert(std::make_pair<std::string,std::string>("update_time","1"));
  try {
    FeedAssistantAdapter::instance().dispatch(feed_seed);
  } catch (Ice::Exception& e) {
    MCE_WARN("send friend send edm by FeedAssistantAdapter failed, err->" << e);
  }
  MCE_INFO("edmFriend userID:" << user_id_ << " friendID:" << friend_id);
  MiscEdmSenderCache::instance().Add(user_id_, kFriend);
  return;
}
int FriendEdmSenderTask::GetFriendToRecommend() {
  int friend_id = 0;
  try {
    std::vector<int> friend_seq = WriteReconnectWorkerAdapter::instance().getWriteRecommendRandom(user_id_,1);
    //MCE_INFO("friend id of user:" << user_id_ << " num is" << friend_seq.size());
    if (friend_seq.size() > 0) {
      friend_id = friend_seq.at(0);
     //MCE_INFO("friend id of user:" << user_id_ << " is " << friend_id);
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("get friend recommend from WriteReconnectWorkerAdapter failed, err->" << e);
  }
  return friend_id;
}



}
}
