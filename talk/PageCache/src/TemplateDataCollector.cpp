#include <boost/lexical_cast.hpp>
#include "MucTalkRecordAdapter.h"
#include "TemplateDataCollector.h"
#include "PageCacheManagerI.h"
#include "MessageHolderAdapter.h"
#include "OnlineBitmapAdapter.h"
//#include "NotifyRplAdapter.h"
//#include "XFeedNewsAdapter.h"
//#include "CountCacheAdapter.h"
#include "UserCountAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"

using namespace xce::talk;
using namespace talk::onlinebitmap;
using namespace com::xiaonei::talk::muc;
using namespace talk::adapter;
using namespace com::xiaonei::talk;
//using namespace mop::hi::svc::dispatch;
//using namespace xce::xfeed;

using namespace talk::http;
using ctemplate::TemplateDictionary;
static int GetUserId(const Str2StrMap& cookies, const Str2StrMap& parameter) {
  int uid = 0;
  Str2StrMap::const_iterator it = cookies.find("id");
  if (it != cookies.end() && (!it->second.empty())) {
    try {
      uid = boost::lexical_cast<int>(it->second);
      return uid;
    } catch (...) {
      MCE_WARN("OnlineBuddyListCollector::fillData --> cast err, " << it->second);
    }
  }

  it = parameter.find("uid");
  if (it != parameter.end()) {
    try {
      uid = boost::lexical_cast<int>(it->second);
    } catch (std::exception& e) {
      MCE_WARN("OnlineBuddyListCollector::fillData --> cast wap uid err:"<<e.what());
    }
  }
  return uid;
}

static string GetTicket(const Str2StrMap& cookies, const Str2StrMap& parameter) {
  Str2StrMap::const_iterator it = cookies.find("t");
  if (it != cookies.end()) {
    return it->second;
  }
  it = parameter.find("t");
  if (it != parameter.end()) {
    return it->second;
  }
  return string();
}

static int GetGroupId(const Str2StrMap& parameter){
	int gid = -1;
	Str2StrMap::const_iterator it = parameter.find("gid");
	if(it != parameter.end()){
  	try {
      gid = boost::lexical_cast<int>(it->second);
    } catch (std::exception& e) {
      MCE_WARN("OnlineBuddyListCollector::fillData GetGroupId cast gid = " << it->second << "--> ERROR  err:"<<e.what());
			return -1;
    }
	}
	return gid;
}

static int GetTargetUserId(const Str2StrMap& parameter){
	int targetuser = -1;
	Str2StrMap::const_iterator it = parameter.find("targetuser");
	if(it != parameter.end()){
  	try {
      targetuser = boost::lexical_cast<int>(it->second);
    } catch (std::exception& e) {
      MCE_WARN("OnlineBuddyListCollector::fillData GetTargetUserId cast targetuser = " << it->second << "--> ERROR  err:"<<e.what());
			return -1;
    }
	}
	return targetuser;
}

static string GetGroupName(const Str2StrMap& parameter){
	Str2StrMap::const_iterator it = parameter.find("gname");
	if(it != parameter.end()){
		return it->second;
	}
	return string();
}

bool OnlineBuddyListCollector::fillData(const std::string& path,
    const Str2StrMap& cookies, const Str2StrMap& parameter,
    TemplateDictionary& dict) {
  int uid = GetUserId(cookies, parameter);

  if (uid <= 0) {
    MCE_WARN("OnlineBuddyListCollector::fillData --> uid:" << uid << " path:" << path);
    return false;
  }

  string ticket = GetTicket(cookies, parameter);
	int video_view_power = PageCacheManagerI::instance().GetVideoViewPower();
  IceUtil::Mutex::Lock lock(mutex_[uid/10 % kLockSize]);

  ActiveUserPtr au = ServiceI::instance().locateObject<ActiveUserPtr> (AU, uid);
  if (au->checkTicket(ticket, path.find("/wap_") == 0)) {

    dict.SetValue("UID", boost::lexical_cast<string>(uid));
    dict.SetValue("USER_NAME", au->name());
    vector<BuddyPtr> bl;
		try{
			bl = au->getBuddyList();
		}catch(std::exception& e){
			MCE_WARN("OnlineBuddyListCollector::fillData --> get buddy list uid" << uid << " error:" << e.what());
		}
    MCE_INFO("OnlineBuddyListCollector::fillData --> get buddy list uid:" << uid << " size" << bl.size());
    for (size_t i = 0; i < bl.size(); ++i) {
      TemplateDictionary* buddy_dict = dict.AddSectionDictionary( "BUDDY_SECTION");
      buddy_dict->SetValue("VIDEO_VIEW_POWER", boost::lexical_cast<string>(video_view_power));
      buddy_dict->SetValue("BUDDY_TINY_URL", bl.at(i)->tinyUrl());
      buddy_dict->SetValue("BUDDY_ONLINE_STATUS", boost::lexical_cast<string>( bl.at(i)->onlineStatus()));
      buddy_dict->SetValue("BUDDY_NAME", bl.at(i)->name());
      //buddy_dict->SetValue("BUDDY_DOING", bl.at(i)->doing());
      buddy_dict->SetValue("BUDDY_ID", boost::lexical_cast<string>( bl.at(i)->id()));
    }

    dict.SetValue("BUDDY_COUNT", boost::lexical_cast<string>(bl.size()));
    return true;

  } else {
    return false;
  }
}

bool OnlineBuddyCountCollector::fillData(const std::string& path,
    const Str2StrMap& cookies, const Str2StrMap& parameter,
    TemplateDictionary& dict) {
  int uid = GetUserId(cookies, parameter);
  if (uid <= 0) {
    MCE_WARN("OnlineBuddyListCollector::fillData --> uid:" << uid << " path:" << path);
    return false;
  }

  string ticket = GetTicket(cookies, parameter);

  IceUtil::Mutex::Lock lock(mutex_[uid/10 % kLockSize]);

  ActiveUserPtr au = ServiceI::instance().locateObject<ActiveUserPtr> (AU, uid);
  if (au->checkTicket(ticket)) {

    dict.SetValue("UID", boost::lexical_cast<string>(uid));
    dict.SetValue("USER_NAME", au->name());
    dict.SetValue("USER_TINY_URL", au->tiny_url());

    Ice::Int buddy_count = au->getBuddyCount();
		MCE_INFO("OnlineBuddyListCollector::fillData --> uid:" << uid << " count:" << buddy_count);
    dict.SetValue("BUDDY_COUNT", boost::lexical_cast<string>(buddy_count));
    return true;

  } else {
    return false;
  }
}

bool OnlineToolbarCountsCollector::fillData(const std::string& path,
    const Str2StrMap& cookies, const Str2StrMap& parameter,
    TemplateDictionary& dict) {

  int uid = GetUserId(cookies, parameter);
  if (uid <= 0) {
    MCE_WARN("OnlineBuddyListCollector::fillData --> uid:" << uid << " path:" << path);
    return false;
  }

  string ticket = GetTicket(cookies, parameter);

  IceUtil::Mutex::Lock lock(mutex_[uid/10 % kLockSize]);

  ActiveUserPtr au = ServiceI::instance().locateObject<ActiveUserPtr> (AU, uid);
  if (au->checkTicket(ticket)) {
    int offlineMessageCount = 0;
    int unreadReplyCount = 0;
    int unreadFeedCount = 0;
    int buddyApplyCount = 0;
    int notifyCount = 0;

    try {
      offlineMessageCount
          = MessageHolderAdapter::instance().getOfflineMessageCount(
              au->userId());
    } catch (Ice::Exception& e) {
      MCE_WARN("OnlineToolbarCountsCollector::fillData-->MessageHolderAdapter::getOfflineMessageCount-->" << e);
    } catch (std::exception& e) {
      MCE_WARN("ResponseAction --> get offline message count err"<< e.what());
    }
    try {
      unreadReplyCount = 0;//NotifyRplAdapter::instance().getCount(au->userId(),-1,true);
    } catch (...) {
      MCE_WARN("ResponseAction --> get reply count err");
    };

    try {
      xce::usercount::UserCounterPtr uc =
          xce::usercount::adapter::UserCountAdapter::instance().getAll(
              au->userId());
      int cc = 0;
      for (size_t i = 0; i < uc->values.size(); ++i) {
        if (uc->types.at(i) == xce::usercount::CGuestRequest) {
          buddyApplyCount = uc->values.at(i);
          if (++cc == 2) {
            break;
          }
        }
        if (uc->types.at(i) == xce::usercount::CNotify) {
          notifyCount = uc->values.at(i);
          if (++cc == 2) {
            break;
          }
        }
      }
    } catch (Ice::Exception & e) {
      MCE_WARN("OnlineToolbarCountsCollector::fillData-->UserCountAdapter::getAll-->"<<e);
    };

    dict.SetValue("OFFLINE_MSG_COUNT", boost::lexical_cast<string>(
        offlineMessageCount));
    dict.SetValue("REPLY_COUNT", boost::lexical_cast<string>(unreadReplyCount));
    dict.SetValue("FEED_COUNT", boost::lexical_cast<string>(unreadFeedCount));
    dict.SetValue("BUDDY_APPLY_COUNT", boost::lexical_cast<string>(
        buddyApplyCount));
    dict.SetValue("NOTIFY_COUNT", boost::lexical_cast<string>(notifyCount));

    return true;

  } else {
    return false;
  }
}

bool OnlineRoomUserCollector::fillData(const std::string& path,
    const Str2StrMap& cookies, const Str2StrMap& parameter,
    TemplateDictionary& dict) {
  int uid = GetUserId(cookies, parameter);
  if (uid <= 0) {
    MCE_WARN("OnlineRoomUserCollector::fillData --> uid:" << uid << " path:" << path);
    return false;
  }
	int gid = GetGroupId(parameter);
	if(-1 == gid){
		MCE_WARN("OnlineRoomUserCollector::fillData --> gid error");
		return false;
	}
	string groupid = boost::lexical_cast<string>(gid);
	string domain = "group.talk.xiaonei.com";
  IceUtil::Mutex::Lock lock(mutex_[gid/10 % kLockSize]);
	ActiveGroupPtr ag = ServiceI::instance().locateObject<ActiveGroupPtr>(AG, gid);
	string groupname = ag->GetGroupName();
	dict.SetValue("UID", boost::lexical_cast<string>(uid));
	dict.SetValue("GROUPID", groupid);
	dict.SetValue("GROUPNAME", groupname);

	vector<BuddyPtr> bl = ag->getMemberList();

	MCE_INFO("OnlineRoomUserCollector::fillData --> get member list groupid = " << groupid << " groupname = " << groupname << " uid:" << uid << " size" << bl.size());
	for (vector<BuddyPtr>::iterator bit = bl.begin(); bit != bl.end(); ++bit) {
		BuddyPtr b = (*bit);
		TemplateDictionary* buddy_dict = dict.AddSectionDictionary("MEMBER_SECTION");
		buddy_dict->SetValue("MEMBER_TINY_URL", b->tinyUrl());
		buddy_dict->SetValue("MEMBER_ONLINE_STATUS", boost::lexical_cast<string>(b->onlineStatus()));
		buddy_dict->SetValue("MEMBER_NAME", b->name());
		buddy_dict->SetValue("MEMBER_DOING", b->doing());
		buddy_dict->SetValue("MEMBER_ID", boost::lexical_cast<string>(b->id()));
	}

	dict.SetValue("MEMBER_COUNT", boost::lexical_cast<string>(bl.size()));
	return true;
}

bool GroupHistoryCollector::fillData(const std::string& path,
    const Str2StrMap& cookies, const Str2StrMap& parameter,
    TemplateDictionary& dict) {
  int uid = GetUserId(cookies, parameter);
  if (uid <= 0) {
    MCE_WARN("GroupHistoryCollector::fillData --> uid:" << uid << " path:" << path);
    return false;
  }
	int gid = GetGroupId(parameter);
	if(-1 == gid){
		MCE_WARN("GroupHistoryCollector::fillData --> gid error");
		return false;
	}
	string groupid = boost::lexical_cast<string>(gid);
  IceUtil::Mutex::Lock lock(mutex_[gid/10 % kLockSize]);
	ActiveGroupPtr ag = ServiceI::instance().locateObject<ActiveGroupPtr>(AG, gid);
	if(!ag->FindMember(uid)){
		MCE_DEBUG("GroupHistoryCollector::fillData --> user:" << uid << " is not the activeuser of the room return false");
		return false;
	}
	string groupname = ag->GetGroupName();
	dict.SetValue("UID", boost::lexical_cast<string>(uid));
	dict.SetValue("GROUPID", groupid);
	dict.SetValue("GROUPNAME", groupname);
	MucTalkMessageSeq msgs;
	TalkUserMap umap;
	try {
		msgs = MucTalkRecordAdapter::instance().GetRecentlyMsg(gid);
  } catch (Ice::Exception& e) {
    MCE_WARN("GroupHistoryCollector::fillData-->MucTalkRecordAdapter::GetRecentlyMsg-->" <<e);
  }
	IntSeq ids;
	for(MucTalkMessageSeq::iterator mit = msgs.begin(); mit != msgs.end(); ++mit){
		ids.push_back((*mit)->userid);
	}
	try {
		umap = TalkCacheClient::instance().GetUserBySeqWithLoad(uid, ids);
  } catch (Ice::Exception& e) {
    MCE_WARN("GroupHistoryCollector::fillData --> call TalkCacheClient err : " <<e);
  }
	
	MCE_INFO("GroupHistoryCollector::fillData --> history.size = " << msgs.size() << " ids.size = " << ids.size());
	for (MucTalkMessageSeq::iterator mit = msgs.begin(); mit != msgs.end(); ++mit) {
		MucTalkMessagePtr m = (*mit);
		TalkUserMap::iterator it = umap.find(m->userid);
		if(it == umap.end()){
			continue;
		}
  	TalkUserPtr u = it->second;
		TemplateDictionary* history_dict = dict.AddSectionDictionary("MESSAGE_SECTION");
		history_dict->SetValue("SENDER_TINY_URL", u->tinyurl);
		history_dict->SetValue("SENDER_NAME", u->name);
		history_dict->SetValue("MESSAGE", m->message);
		history_dict->SetValue("MESSAGE_TIME", boost::lexical_cast<string>(m->timestamp));
		history_dict->SetValue("MEMBER_ID", boost::lexical_cast<string>(m->userid));
	}
	return true;
}


//add by guoqing.liu : 传入一个userId 返回这个用户的在线状态，头像链接，姓名
bool UserInfoCollector::fillData(const std::string& path,
		const Str2StrMap& cookies, const Str2StrMap& parameter,
		TemplateDictionary& dict) {
	int uid = GetUserId(cookies, parameter);
	if (uid <= 0) {
		MCE_WARN("UserInfoCollector::fillData --> uid:" << uid << " path:" << path);
		return false;
	}
	string ticket = GetTicket(cookies, parameter);
	int targetuser = GetTargetUserId(parameter);
	if(-1 == targetuser || !targetuser){
		MCE_WARN("UserInfoCollector::fillData --> targetuser error");
		return false;
	}
	IceUtil::Mutex::Lock lock(mutex_[uid/10 % kLockSize]);
  ActiveUserPtr au = ServiceI::instance().locateObject<ActiveUserPtr> (AU, uid);
  if (au->checkTicket(ticket, path.find("/wap_") == 0)) {
		dict.SetValue("TARGETUSER_ID", boost::lexical_cast<string>(targetuser));
		TalkUserPtr uPtr;
		try {
			uPtr = TalkCacheClient::instance().GetUserByIdWithLoad(targetuser);
		} catch (Ice::Exception& e) {
			MCE_WARN("UserInfoCollector::fillData --> call TalkCacheClient err : " <<e);
		}
		UserOnlineTypePtr user_online_type;
		try{
			user_online_type = OnlineBitmapAdapter::instance().getUserType(targetuser);
		}catch(Ice::Exception& e){
			MCE_WARN("UserInfoCollector::fillData --> call OnlineBitmap err : " <<e);
		}
		if(user_online_type){
			MCE_INFO("UserInfoCollector::fillData --> targetuser = " << targetuser << " onlinetype = " << user_online_type->onlineType);
			dict.SetValue("TARGETUSER_ONLINE_TYPE", boost::lexical_cast<string>(user_online_type->onlineType));
		}
		else{
			dict.SetValue("TARGETUSER_ONLINE_TYPE", "0");
		}
		if(uPtr){
			string username = uPtr->name;
			string tinyurl = uPtr->tinyurl;
			dict.SetValue("TARGETUSER_TINY_URL", tinyurl);
			dict.SetValue("TARGETUSER_NAME", username);
    	return true;
		}
		return false;
  } else {
    return false;
  }
}

