#include <boost/lexical_cast.hpp>
#include <ctemplate/template.h>
#include <ctemplate/template_dictionary.h>
#include <cstdio>
#include "MucGateAdapter.h"
#include "BuddyByAddTimeReplicaAdapter.h"
#include "PageCacheManagerI.h"
#include "OnlineBitmapAdapter.h"
#include "OnlineCenterAdapter.h"
#include "TicketAdapter.h"
#include "LoginCacheAdapter.h"
#include "util/cpp/String.h"
#include "XmppTools.h"
#include "pugixml.hpp"
#include "FeedMemcProxy/client/talk_cache_client.h"
//#include "client/linux/handler/exception_handler.h"
using namespace xce::talk;
using namespace com::xiaonei::talk::muc;
using namespace talk::http;
using namespace talk::online::adapter;
using namespace xce::buddy::adapter;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk;
using namespace pugi;
//using namespace mop::hi::svc::adapter;

using namespace passport;
bool xiaonei = true;

/*
bool MyCallback(const char *dump_path,
   const char *minidump_id,
   void *context,
   bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}*/


void MyUtil::initialize() {

  //static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
  //                                   , 0, true);

  ServiceI& service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  service.getObjectCache()->registerCategory(AU, "AU", new ActiveUserFactoryI,
      props, true);
  service.getObjectCache()->registerCategory(AG, "AG", new ActiveGroupFactoryI,
      props, true);

  xiaonei = (bool) props->getPropertyAsIntWithDefault("Service." + ServiceI::instance().getName() + ".IsXiaoNei", 1);
  
	string configFilePath = props->getPropertyWithDefault("Service.ConfigFilePath", "/opt/XNTalk/etc/pagecache.xml");
  PageCacheManagerI::instance().loadConfig(configFilePath);
	PageCacheManagerI::instance().SetVideoViewPower(1);
  service.getAdapter()->add(&PageCacheManagerI::instance(),
      service.createIdentity("M", ""));




}
//------------------------------------------------

//---------------------------------------------------------------------------
void Buddy::name(const string& name) {
  _name = name;
  for (size_t i = 0; i < _name.size(); ++i) {
    if (_name[i] == '\r' || _name[i] == '\n' || _name[i] == '\t') {
      _name[i] = ' ';
    }
  }
}
string Buddy::tinyUrl() {
  if (_tinyurl == "") {
    return "http://head.xiaonei.com/photos/0/0/men_tiny.gif";
  } else {
    return _tinyurl;
  }
}

void Buddy::doing(const string& doing) {
  _doing = doing;
  for (size_t i = 0; i < _doing.size(); ++i) {
    if (_doing[i] == '\r' || _doing[i] == '\n' || _doing[i] == '\t') {
      _doing[i] = ' ';
    }
  }
}

//---------------------------------------------------------------------------
vector<BuddyPtr> ActiveUser::getBuddyList() {
  if (time(NULL) - _buddyListTimestamp < 60) {
    return _buddyList;
  }

  MyUtil::IntSeq buddy_ids;
  UserOnlineTypeSeq onlineStats;
  try {
    buddy_ids = xce::buddy::adapter::BuddyByAddTimeCacheAdapter::instance().getFriendList( _userid, -1);
  } catch (Ice::Exception& e) {
    MCE_WARN("Buddy::doing-->BuddyByAddTimeCacheAdapter::getFriendList-->uid=" << _userid << "-" <<e);
  }
	if(buddy_ids.empty()){
		return _buddyList;
	}
  if (!buddy_ids.empty()) {
    try {
      onlineStats = talk::onlinebitmap::OnlineBitmapAdapter::instance().getOnlineUsers( buddy_ids);
    } catch (Ice::Exception& e) {
      MCE_WARN("Buddy::doing-->OnlineBitmapAdapter::getOnlineUsers()-->uid=" << _userid << "-" << e);
    }
  }
	if(onlineStats.empty()){
		return _buddyList;
	}
  MyUtil::IntSeq ids;
  map<int, int> onlineStat;
  for (size_t i = 0; i < onlineStats.size(); ++i) {
    ids.push_back(onlineStats.at(i)->userId);
    onlineStat[onlineStats.at(i)->userId] = onlineStats.at(i)->onlineType;
  }
	TalkUserMap usermaps;
	try{ 
		usermaps = TalkCacheClient::instance().GetUserBySeqWithLoad(_userid, ids);
	}catch(Ice::Exception& e){
		MCE_WARN("ActiveUser::getBuddyList-->TalkCacheClient::GetUserBySeqWithLoad-->" << e);
	}catch(std::exception& e){
		MCE_WARN("ActiveUser::getBuddyList-->TalkCacheClient::GetUserBySeqWithLoad--> stl exception:" << e.what());
	}

  MCE_INFO("@@@ActiveUser::getBuddyList --> get UserCacheList " << _userid << " ids:" << ids.size() << " props:" << usermaps.size());
  vector<BuddyPtr> res;
  vector<string> urls;
	TalkUserMap::iterator it = usermaps.begin();
  for (; it != usermaps.end(); ++it) {
    BuddyPtr b = new Buddy();
		TalkUserPtr u = it->second;
    b->id(u->id);
    b->name(u->name);
		try{
			b->tinyUrl(buildHeadUrl(u->tinyurl, xiaonei));
		}catch(std::exception& e){
			MCE_WARN("ActiveUser::getBuddyList--> buildHeadUrl error:" << e.what());
		}
    b->doing(u->statusOriginal);
    b->onlineStatus(onlineStat[b->id()]);
    res.push_back(b);
  }

  _buddyList.swap(res);
  _buddyListTimestamp = time(NULL);
  return _buddyList;
}

int ActiveUser::getBuddyCount() {
  MyUtil::IntSeq buddy_ids;

  try {
    //onlineStats = OnlineCenterAdapter::instance().getBuddyOnlineTypeSeq(_userid,600);
    buddy_ids
        = xce::buddy::adapter::BuddyByAddTimeCacheAdapter::instance().getFriendList(
            _userid, -1);
  } catch (Ice::Exception& e) {
    MCE_WARN("ActiveUser::getBuddyCount-->BuddyByAddTimeCacheAdapter::getFriendList-->uid=" << _userid << "-" <<e);
  }
  int count = 0;
  if (!buddy_ids.empty()) {
    try {
      count = talk::onlinebitmap::OnlineBitmapAdapter::instance().getOnlineCount(buddy_ids);
    } catch (Ice::Exception& e) {
      MCE_WARN("ActiveUser::getBuddyCount-->OnlineBitmapAdapter::getOnlineCount()-->uid=" << _userid << "-" << e);
    }
  }
  return count;

}

bool ActiveUser::checkTicket(const string& ticket, bool isWap) {
  if ((_ticket == ticket) && (_ticket != "")) {
    return true;
  }
  int id = 0;
  try {
    if (isWap) {
			try{
      id = WapLoginCacheAdapter::instance().ticket2Id(ticket);
			}
			catch(Ice::Exception& e){
				MCE_WARN("ActiveUser::checkTicket-->WapLoginCacheAdapter::ticket2Id-->err" << e);
				}
    } else {
      id = TicketAdapter::instance().verifyTicket(ticket, "");
    }
    MCE_DEBUG("get passport -> id: " << id << " " << ticket << " " << isWap);
    if (id == _userid) {
      _ticket = ticket;
      return true;
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("ActiveUser::checkTicket-->TicketAdapter::verifyTicket-->" << _userid << " " << e);
  }
  _ticket = "";
  MCE_DEBUG("get passport -> err: userid:" << _userid << " uid:" << id << " " << ticket << " " << isWap);
  return false;
}

int ActiveUser::userId() {
  return _userid;
}

void ActiveUser::load() {
  IntSeq ids;
  ids.push_back(_userid);
	TalkUserMap usermaps;
	try{ 
		usermaps = TalkCacheClient::instance().GetUserBySeqWithLoad(_userid, ids);
	}catch(Ice::Exception& e){
		MCE_WARN("ActiveUser::load-->TalkCacheClient::GetUserBySeqWithLoad-->" << e);
	}

	TalkUserMap::iterator it = usermaps.find(_userid);
  if (it == usermaps.end()) {
    return;
  }
	TalkUserPtr u = it->second;
  _name = u->name;
  StrUtil::string_replace(_name, "\n", " ");
  StrUtil::string_replace(_name, "\r", " ");
  StrUtil::string_replace(_name, "\t", " ");

  _tiny_url = buildHeadUrl(u->tinyurl, xiaonei);
}

string ActiveUser::name() {
  if (!_name.empty()) {
    return _name;
  }
  load();
  if (_name.empty())
    return boost::lexical_cast<string>(_userid);
  return _name;
}


string ActiveUser::tiny_url() {
  if (!_tiny_url.empty()) {
    return _tiny_url;
  }
  load();
  if (_tiny_url.empty())
    return "http://head.xiaonei.com/photos/0/0/men_tiny.gif";
  return _tiny_url;
}
//-------------------------------------------------------------------------------------
vector<BuddyPtr> ActiveGroup::getMemberList(){
	if(!_groupMember.empty() && (time(NULL) - _groupListTimestamp  < 600)){
		MCE_INFO("ActiveGroup::getMemberList--> get from cache groupid = " << _groupid);
		vector<BuddyPtr> memberSeq;
		for(RoomMemberMap::iterator bit = _groupMember.begin(); bit != _groupMember.end(); ++bit){
			memberSeq.push_back(bit->second);
		}
		return memberSeq;
		//if (time(NULL) - _groupMemberTimestamp < 60) {
	  //MCE_INFO("ActiveUser::getRoomMember --> _groupMemberTimestamp = " << _groupMemberTimestamp << " time(NULL) - _groupMemberTimestamp = " << time(NULL) - _groupMemberTimestamp);
			//return memberSeq;
		//}
	}
	
	MCE_INFO("ActiveGroup::getMemberList--> CALL MucOnlineCenter to get online member");
	MucRoomIdPtr roomid = new MucRoomId();
	roomid->roomname = boost::lexical_cast<string>(_groupid);
	roomid->domain = "group.talk.xiaonei.com";

	MucUserIdentitySeq identitySeq;
	try{
		identitySeq = MucGateAdapter::instance().GetRoomUserList(roomid);
	} catch (Ice::Exception& e) {
    MCE_WARN("ActiveGroup::getMemberList-->MucGateAdapter::GetRoomUserList-->roomid=" << mucRoomIdToString(roomid) << " error:" << e);
	}
	if(identitySeq.empty()){
		return vector<BuddyPtr>();
	}
	MCE_INFO("ActiveGroup::getMemberList--> identitySeq.size = " << identitySeq.size());
  MyUtil::IntSeq ids;
  UserOnlineTypeSeq onlineStats;
	for(size_t i = 0; i < identitySeq.size(); ++i){
		ids.push_back(identitySeq.at(i)->userid);
	}
	MCE_INFO("ActiveGroup::getMemberList--> ids.size() = " << ids.size());
	for(IntSeq::iterator logit = ids.begin(); logit != ids.end(); ++logit){
		MCE_INFO("ActiveGroup::getMemberList --> identity  id =  " << (*logit));
	}
	if (!ids.empty()){
    try {
      onlineStats = talk::onlinebitmap::OnlineBitmapAdapter::instance().getOnlineUsers(ids);
    } catch (Ice::Exception& e) {
      MCE_WARN("ActiveGroup::getMemberList-->OnlineBitmapAdapter::getOnlineUsers-->groupid =" << _groupid << "-" << e);
    }
  }
	MCE_INFO("ActiveGroup::getMemberList --> onlineStats.size = " << onlineStats.size());
  map<int, int> onlineStat;
  for (size_t i = 0; i < onlineStats.size(); ++i) {
    onlineStat[onlineStats.at(i)->userId] = onlineStats.at(i)->onlineType;
  }
	
	TalkUserMap usermaps;
	try{ 
		usermaps = TalkCacheClient::instance().GetUserBySeqWithLoad(_groupid, ids);
	}catch(Ice::Exception& e){
		MCE_WARN("ActiveGroup::getMemberList-->TalkCacheClient::GetUserBySeqWithLoad--> " << e);
	}

  if(usermaps.empty()){
		MCE_INFO("ActiveGroup::getMemberList --> get UserCacheList call TalkCacheClient return NULL");	
  	return vector<BuddyPtr>();
  }
  MCE_INFO("@@@ActiveGroup::getMemberList --> get UserCacheList groupid = " << _groupid << " ids:" << ids.size() << " props:" << usermaps.size());
  
	vector<BuddyPtr> res;
  vector<string> urls;
	TalkUserMap::iterator it = usermaps.begin();
  for (; it != usermaps.end(); ++it) {
    BuddyPtr b = new Buddy();
    TalkUserPtr u = it->second;

    b->id(u->id);
    b->name(u->name);
    b->tinyUrl(buildHeadUrl(u->tinyurl, xiaonei));
    b->doing(u->statusOriginal);
    b->onlineStatus(onlineStat[b->id()]);
    res.push_back(b);
		_groupMember[u->id] = b;
  }
  _groupListTimestamp = time(NULL);
	try{
		_groupname = MucGateAdapter::instance().GetGroupName(_groupid);
	} catch (Ice::Exception& e) {
    MCE_WARN("ActiveGroup::getMemberList--> call MucGateAdapter::GetGroupName roomid = " << mucRoomIdToString(roomid));
	}
  return res;
}

void ActiveGroup::KickMember(int memberid){
	MCE_INFO("ActiveGroup::KickMember --> memberid = " << memberid);
	RoomMemberMap::iterator it = _groupMember.find(memberid);
	if(it != _groupMember.end()){
		_groupMember.erase(it);
	}
}
string ActiveGroup::GetGroupName(){
	return _groupname;
}
void ActiveGroup::UpdateGroupName(const string& groupname){
	_groupname = groupname;
}
bool ActiveGroup::FindMember(int memberid){
	MCE_INFO("ActiveGroup::FindMember --> memberid = " << memberid);
	return (_groupMember.find(memberid)) != _groupMember.end();
}
void ActiveGroup::AddMember(int memberid, const BuddyPtr& buddy){
	MCE_INFO("ActiveGroup::AddMember --> memberid = " << memberid);
	_groupMember[memberid] = buddy;
}
void ActiveGroup::ChangeMemberOnlineType(int memberid, int onlinetype){
	MCE_INFO("ActiveGroup::ChangeMemberOnlineType --> memberid = " << memberid);
	RoomMemberMap::iterator it = _groupMember.find(memberid);
	if(it != _groupMember.end()){
		it->second->onlineStatus(onlinetype);
	}
}
//---------------------------------------------------------------------------
Ice::ObjectPtr ActiveUserFactoryI::create(Ice::Int id) {
  ActiveUserPtr u = new ActiveUser(id);
  return u;
}
Ice::ObjectPtr ActiveGroupFactoryI::create(Ice::Int id) {
  ActiveGroupPtr u = new ActiveGroup(id);
  return u;
}
//------------------------------------------------
void PageCacheManagerI::loadConfig(const string& path) {
  xml_document doc;
  if (doc.load_file(path.c_str())) {

    xml_node configNode = doc.child("config");
    for (xml_node n = configNode.child("page"); n; n = n.next_sibling("page")) {

      string url = n.attribute("url").value();
      if (url != "") {
        Config c;
        c.template_path = n.attribute("file").value();
        c.timeout = boost::lexical_cast<int>(n.attribute("timeout").value());
        c.type = n.attribute("type").value();
        c.content_type = n.attribute("content_type").value();
        _config[url] = c;
      }
    }
  }
}

ContentPtr PageCacheManagerI::GetContent(const string& path,
    const MyUtil::Str2StrMap& cookies, const MyUtil::Str2StrMap& parameter,
    const Ice::Current&) {

  //取uid只为打log
  string strHostId;
  MyUtil::Str2StrMap::const_iterator tmpIt;
  if ((tmpIt = cookies.find("id")) != cookies.end()) {
    strHostId = tmpIt->second;
  } else if ((tmpIt = parameter.find("uid")) != parameter.end()) {
    strHostId = tmpIt->second;
  }
  int uid = 0;
  if (!strHostId.empty()) {
    try {
      uid = boost::lexical_cast<int>(strHostId);
    } catch (...) {
      MCE_WARN("PageCacheManagerI::GetContent --> cast err, " << strHostId);
    }
  }
  map<string, Config>::iterator it = _config.find(path);
  if (it == _config.end()) {
    ContentPtr c = new Content();
    c->statusCode = 404;
    c->timeout = 0;
    c->data = "url not find";
    c->isBin = false;
    MCE_WARN("PageCacheManagerI::GetContent --> url not configed, " << uid << " " << path);
    return c;
  }
	MCE_INFO("PageCacheManagerI::GetContent --> path = " << path << " template_path = " << it->second.template_path);
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(it->second.template_path, ctemplate::DO_NOT_STRIP);
  if (!tpl) {
    ContentPtr c = new Content();
    c->statusCode = 404;
    c->timeout = 0;
    c->data = "url not find";
    c->isBin = false;
    MCE_WARN("PageCacheManagerI::GetContent --> template is absent, " << uid << " " << path);
    return c;
  }
  //tpl->ReloadIfChanged();
  ctemplate::TemplateDictionary dict("page_template");

  ContentPtr c = new Content();
  if (it->second.type == "bin") {
    c->isBin = true;
  } else {
    c->isBin = false;
  }
  if (it->second.type == "template") {
    MCE_DEBUG("PageCacheManagerI::GetContent --> template_path:" <<it->second.template_path << " userid:" << uid);
    TemplateDataCollectorPtr tc = getTemplateCollector(path);

    if (tc) {
			try{
				tc->fillData(path, const_cast<Str2StrMap&> (cookies), const_cast<Str2StrMap&> (parameter), dict);
			}catch(std::exception& e){
				MCE_WARN("PageCacheManagerI::GetContent --> userid:" << uid << " path: " << path << " tc->fillData error:" << e.what());
			}
    } else {
      //			ContentPtr c = new Content();
      c->statusCode = 404;
      c->timeout = 0;
      c->data = "url not find";
      c->isBin = false;

      return c;
    }
  }
  bool error_free = tpl->Expand(&c->data, &dict);

  if (error_free) {
    c->timeout = it->second.timeout;
    c->statusCode = 200;
    c->contentType = it->second.content_type;
  } else {
    c->statusCode = 404;
    c->timeout = 0;
    c->data = "build content err";
  }
  MCE_INFO("PageCacheManagerI::GetContent --> " << uid << " url:" << path << " page size:" << c->data.length());
  return c;

}

void PageCacheManagerI::ReloadStaticPages(const Ice::Current&) {
  map<string, Config>::iterator it = _config.begin();
  for (; it != _config.end(); ++it) {
    if (it->second.type != "template") {
      ctemplate::Template* tpl = ctemplate::Template::GetTemplate(
          it->second.template_path, ctemplate::DO_NOT_STRIP);
      if (tpl) {
        tpl->ReloadIfChanged();
      }
    }
  }
}

void PageCacheManagerI::UpdateRoomName(int groupid, const string& groupname, const Ice::Current& current){
  ActiveGroupPtr ag = ServiceI::instance().findObject<ActiveGroupPtr> (AG, groupid);
	if(!ag){
		MCE_INFO("PageCacheManagerI::UpdateRoomName --> groupid = " << groupid << " could not find object so return");
		return;
	}
	MCE_INFO("PageCacheManagerI::UpdateRoomName --> groupid = " << groupid << " new name = " << groupname);
  IceUtil::Mutex::Lock lock(mutex_[groupid % kLockSize]);
	ag->UpdateGroupName(groupname);
}
void PageCacheManagerI::MemberStatusChange(int groupid, int userid, int mucstatus, const Ice::Current& current){
  ActiveGroupPtr ag = ServiceI::instance().findObject<ActiveGroupPtr> (AG, groupid);
	if(!ag){
		return;
	}
	UserOnlineTypePtr onlinetype; 
  try {
  	onlinetype = talk::onlinebitmap::OnlineBitmapAdapter::instance().getUserType(userid);
  } catch (Ice::Exception& e) {
    MCE_WARN("PageCacheManagerI::MemberStatusChange-->OnlineBitmapAdapter::getUserType-->uid=" << userid << "-" << e);
  }
  IceUtil::Mutex::Lock lock(mutex_[groupid % kLockSize]);
	if(!onlinetype || !(onlinetype->onlineType) || !mucstatus){
		ag->KickMember(userid);
	}
	else{
		bool find = ag->FindMember(userid);
		if(!find){
			MCE_INFO("PageCacheManagerI::MemberStatusChange--> the group " << groupid << " don't has the user << " << userid << " so make the user online");
			BuddyPtr b = MakeBuddyById(userid , mucstatus);
			if(!b){
				return;
			}
			ag->AddMember(userid, b);
			return;
		}
		ag->ChangeMemberOnlineType(userid, mucstatus);
	}
}

BuddyPtr PageCacheManagerI::MakeBuddyById(int userid, int mucstatus){
	int ot = mucstatus;
	TalkUserPtr uPtr;
	try{ 
		uPtr = TalkCacheClient::instance().GetUserByIdWithLoad(userid);
	}catch(Ice::Exception& e){
		MCE_WARN("PageCacheManagerI::MakeBuddyById-->TalkCacheClient::GetUserByIdWithLoad-->" << e);
	}
  if(!uPtr){
		MCE_INFO("PageCacheManagerI::MakeBuddyById--> get UserCacheList call TalkCacheClient return NULL");	
  	return NULL;
  }
  BuddyPtr b = new Buddy();
  b->id(uPtr->id);
  b->name(uPtr->name);
  b->tinyUrl(buildHeadUrl(uPtr->tinyurl, xiaonei));
  b->doing(uPtr->statusOriginal);
  b->onlineStatus(ot);
	return b;
}



//string PageCacheManagerI::getBinData(const string& localFile,
//		const string& url, const string& type) {
//	IceUtil::Mutex::Lock lock(_mutex);
//
//	map<string,string>::iterator it = _binData.find(url);
//	if (it != _binData.end()) {
//		return it->second;
//	}
//	unsigned char data[256];
//	FILE *fp;
//	if (type == "bin") {
//		fp = fopen(localFile.c_str(), "rb");
//	} else {
//		fp = fopen(localFile.c_str(), "rt");
//		MCE_DEBUG("text----------file:"<<localFile<< "  url:"<<url);
//	}
//	if (fp == NULL) {
//		MCE_WARN("PageCacheManagerI::getBinData --> open file:"<< localFile
//				<< " err");
//		return "";
//	}
//	size_t read = 0;
//	string res;
//	int cc = 0;
//	while (read = fread(&data[0], sizeof(unsigned char), 256, fp) != 0) {
//		cc+= read;
//		for (size_t i = 0; i < read; ++i) {
//			res.push_back(data[i]);
//		}
//
//	}
//	fclose(fp);
//
//	MCE_DEBUG("URL:"<<url<<"    text:"<<res.length() << " - " << res.size()
//			<< "  cc:"<<cc);
//
//	_binData[url] = res;
//	return res;
//}

TemplateDataCollectorPtr PageCacheManagerI::getTemplateCollector(
    const string& path) {
  IceUtil::Mutex::Lock lock(_mutex);

  map<string, TemplateDataCollectorPtr>::iterator it = _collectors.find(path);
  if (it != _collectors.end()) {
    return it->second;
  }
  TemplateDataCollectorPtr c;
  if (path == "/getonlinefriends.do" || path == "/wap_getonlinefriends.do") {
    c = new OnlineBuddyListCollector();
    _collectors[path] = c;
  } else if (path == "/getonlinecount.do") {
    c = new OnlineBuddyCountCollector();
    _collectors[path] = c;
  } else if (path == "/toolbar/counts.xml") {
    c = new OnlineToolbarCountsCollector();
    _collectors[path] = c;
  }else  if (path == "/getroominfo.do"){
		c = new OnlineRoomUserCollector();
	}else if(path == "/getgrouphistory.do"){
		c = new GroupHistoryCollector();
	}else if(path == "/getuserinfo.do"){
		c = new UserInfoCollector();
	}
  return c;
}

int PageCacheManagerI::GetVideoViewPower(const Ice::Current& current){
	IceUtil::RWRecMutex::RLock lock(_video_power_mutex);
	return _video_view_power;
}
void PageCacheManagerI::SetVideoViewPower(int value, const Ice::Current& current){
	IceUtil::RWRecMutex::WLock lock(_video_power_mutex);
	_video_view_power = value;
}



