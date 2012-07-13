#include <boost/lexical_cast.hpp>
#include "RestTask.h"
#include "OnlineCenterAdapter.h"
#include "RestMediatorAdapter.h"
//#include "PassportAdapter.h"
//#include "BuddyByIdReplicaAdapter.h"
#include "TicketAdapter.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "TalkDeliverAdapter.h"
#include "util/cpp/String.h"
#include "pugixml.hpp"
#include "QueryRunner.h"
#include "MessageType.h"

using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace passport;
using namespace talk::rest;
using namespace xce::buddy::adapter;
using namespace talk::online::adapter;
using namespace pugi;
using namespace com::xiaonei::xce;
/*
bool RestTask::handle(){
	--_retryCount;
	string reqPath = path();
	map<string, string> paras = parameters();
	RestMediatorAdapter::instance().restRequest(reqPathm, paras);
}
*/

void RestTask::_handle(){
	_timeStamp = time(NULL);
	--_retryCount;
	//handle();
  Str2StrMap paras;
	paras["call_id"] = boost::lexical_cast<string>(time(NULL));
	paras["v"] = "1.0";
	paras["format"] = "xml";
  Str2StrMap tmp = parameters();
  paras.insert(tmp.begin(), tmp.end());
	if(paras.find("api_key") == paras.end()){
    paras["api_key"] = "ffa4add0b30141aa9eb9a8f1af34a8c3";
  }
	paras["sig"] = makeSig(paras);
	if(paras.empty()){
		return;
	}
	string path = "http://api.renren.com/server/restserver.do";

	int userid = boost::lexical_cast<int>(paras["session_key"]);
	try{
		RestMediatorAdapter::instance().restRequest(id(), userid, path, paras);
	}catch(Ice::Exception& e){
		MCE_WARN("RestTask::handle-->RestMediatorAdapter::restRequest-->" << e);
	}
}

string RestTask::makeSig(const map<string, string>& paras){
	string str;
	map<string, string>::const_iterator it = paras.begin();
	for(; it!=paras.end(); ++it){
		str += (it->first + "=" + it->second);
	}
	str += "na4kkj6y7at8kkpryakhfrer28f8xx38";
	MCE_DEBUG("**gen sig --> " << str);
	return MyUtil::StrUtil::md5(str);
}

bool RestTask::validContent(const RestResultPtr& content){
	MCE_DEBUG("SetStatusTask::validContent --> ");
	if(content->err == 200){
		xml_document doc;
		if(doc.load(content->xml.c_str())){
			string name = doc.first_child().name();
			if(name!="error_response"){
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////
map<string, string> SetStatusTask::parameters(){
	MCE_DEBUG("SetStatusTask::handle");

	map<string, string> paras;
	paras["method"] = "xnim.setStatus";
	//paras["api_key"] = "ffa4add0b30141aa9eb9a8f1af34a8c3";
	//paras["call_id"] = boost::lexical_cast<string>(time(NULL));
	//paras["v"] = "1.0";
	paras["session_key"] = boost::lexical_cast<string>(_jid->userId);
	paras["content"] = _status;
	//paras["format"] = "xml";
	//paras["sig"] = makeSig(paras);

/*	string path = "http://api.xiaonei.com/server/restserver.do";
	try{
		RestMediatorAdapter::instance().restRequest(id(), _jid->userId, path, paras);
	}catch(Ice::Exception& e){
		MCE_WARN("SetStatusTask::handle --> invoke RestMediatorAdapter error" << e);
	}*/
	return paras;
}

void SetStatusTask::result(const RestResultPtr& content){
	MCE_DEBUG("SetStatusTask::result --> " << content->err << " " << content->errstr << " " << content->xml);
	Str2StrMap res;
	xml_document doc;
	doc.load(content->xml.c_str());
	xml_node node = doc.child("xnim_setStatus_response");
					//.child("status")
	string doing = node.child("text").first_child().value();
	res["DOING"] = doing;

	xml_node htmlDoingNode =  node.child("embehtml");
	string htmlDoing;
	if(htmlDoingNode){
		htmlDoing = htmlDoingNode.xml();
		res["HTMLDOING"] = htmlDoing;
	}

	MCE_DEBUG("SetStatusTask::result --> " << _jid->userId << " " << res["DOING"] << " " << res["HTMLDOING"]);

	try{
		OnlineCenterAdapter::instance().presence(_jid, _state, "", "", res);
	}catch(Ice::Exception& e){
		MCE_WARN("SetStatusTask::result-->OnlineCenterAdapter::presence" << e << " " << _jid->userId);
	}
}

void SetStatusTask::timeout(){
	MCE_WARN("SetStatusTask::timeout --> " << _jid->userId);
	try{
		OnlineCenterAdapter::instance().presence(_jid, _state, "", "", Str2StrMap());
	}catch(Ice::Exception& e){
		MCE_WARN("SetStatusTask::timeout-->OnlineCenterAdapter::presence-->" << e << " " << _jid->userId);
	}
}

///////////////////////////////////////////////////////
map<string, string> ReplyStatusTask::parameters(){
	MCE_DEBUG("ReplyStatusTask::parameters");

	map<string, string> paras;
	paras["method"] = "status.addComment";
	paras["session_key"] = boost::lexical_cast<string>(_userid);
	paras["owner_id"] = boost::lexical_cast<string>(_ownerId);
	paras["status_id"] = boost::lexical_cast<string>(_statusId);
	if(_replieduserId != 0){
		paras["uid"] = boost::lexical_cast<string>(_replieduserId);
	}
	paras["content"] = _content;
	return paras;
}
void ReplyStatusTask::result(const RestResultPtr& content){
	MCE_DEBUG("ReplyStatusTask::result --> " << content->err << " " << content->errstr << " " << content->xml);
}
void ReplyStatusTask::timeout(){
	MCE_DEBUG("ReplyStatusTask::result --> timeout");
}


////////////////////////////////////////////////////////////////////
map<string, string> UseImNotifyTask::parameters(){
	map<string, string> paras;

	paras["method"] = "notifications.send";
	paras["session_key"] = boost::lexical_cast<string>(_userid);
	//paras["api_key"] = "ffa4add0b30141aa9eb9a8f1af34a8c3";
	//paras["call_id"] = boost::lexical_cast<string>(time(NULL));
	//paras["v"] = "1.0";
	//paras["format"] = "xml";
  MyUtil::IntSeq ids;
	try{
	 ids = BuddyByOnlineTimeCacheAdapter::instance().getFriendList(_userid, 20);
	}
	catch(Ice::Exception& e){
		MCE_WARN("UseImNotifyTask::parameters-->BuddyByOnlineTimeCacheAdapter::getFriendList-->" << e);
		}
	if(ids.empty()) {
	 return map<string, string>();
	}
	MCE_INFO("UseImNotifyTask::parameters --> " << _userid << " " << ids.size());
	ostringstream buddyids;
	for(size_t i = 0; i < ids.size(); ++i) {
	 //  int c = ids.size() < 10? ids.size(): 10;
	 //  for(size_t i = 0; i < c; ++i) {
	 if(i) {
		 buddyids << ",";
	 }
	 buddyids << ids.at(i);
	}
	paras["to_ids"] = buddyids.str();

	ostringstream msg[3];

	msg[0] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>悄悄安装了<a href=\"http://im.renren.com/desktop.html?answer1\">人人桌面</a>，能定时启动游戏，采摘你的果实，升级更快了！| <a href=\"http://im.renren.com/desktop/ver19/rrsetup.exe?answer1\">赶紧安装不能落后！</a>";
	
	msg[1] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>抢先体验了<a href=\"http://im.renren.com/desktop.html?settime2\">人人桌面</a>，留言提醒、快速回复、好友聊天更快更稳定！ | <a href=\"http://im.renren.com/desktop/ver19/rrsetup.exe?settime2\">快来安装吧</a>";
	
	msg[2] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>用<a href=\"http://im.renren.com/desktop.html?onemin3\">人人桌面</a>登录了人人网，TA不开网页，就能一键登录、隐身上线、定时玩游戏！ | <a href=\"http://im.renren.com/desktop/ver19/rrsetup.exe?onemin3\">我也要安装</a>";
	
	paras["notification"] = msg[abs(_userid)%3].str();
/*	paras["sig"] = makeSig(paras);


	string path = "http://api.xiaonei.com/server/restserver.do";
	try{
		RestMediatorAdapter::instance().restRequest(id(), _userid, path, paras);
	}catch(Ice::Exception& e){
		MCE_WARN("UseImNotifyTask::handle --> invoke RestMediatorAdapter error" << e);
	}*/
	return paras;
}
void UseImNotifyTask::result(const RestResultPtr& content){
	MCE_DEBUG("UseImNotifyTask::result --> ");
}
void UseImNotifyTask::timeout(){
	MCE_DEBUG("UseImNotifyTask::result --> timeout");
}

///////////////////////////////////////////////////////
map<string, string> ReplyTask::parameters(){
	MCE_DEBUG("ReplyPhotoTask::parameters");

	map<string, string> paras;
	paras["method"] = _method;
	paras["session_key"] = boost::lexical_cast<string>(_userid);
	paras["uid"] = boost::lexical_cast<string>(_ownerId);
	paras["type"] = boost::lexical_cast<string>(_replyType);
	if(_method == "photos.addComment"){
		paras["pid"] = boost::lexical_cast<string>(_resId);
	}else if(_method == "photos.addAlbumComment"){
		paras["aid"] = boost::lexical_cast<string>(_resId);
	}else if(_method == "blog.addComment"){
		paras["id"] = boost::lexical_cast<string>(_resId);
	}
	if(_replieduserId != 0){
		paras["rid"] = boost::lexical_cast<string>(_replieduserId);
	}
	paras["content"] = _content;
	return paras;
}
void ReplyTask::result(const RestResultPtr& content){
	MCE_DEBUG("ReplyStatusTask::result --> " << content->err << " " << content->errstr << " " << content->xml);
}
void ReplyTask::timeout(){
	MCE_DEBUG("ReplyStatusTask::result --> timeout");
}

//////////////////////////////////////////////////////
map<string, string> ActiveUserNotifyTask::parameters(){
	
  map<string, string> paras;
  /*
	paras["method"] = "notifications.send";
	paras["session_key"] = boost::lexical_cast<string>(_userid);

	MyUtil::IntSeq ids = BuddyByOnlineTimeCacheAdapter::instance().getFriendList(_userid, 50);
	MyUtil::IntSeq needToNotifyIds;
	for(size_t i = 0; i < ids.size(); ++i) {
		if(needToNotifyIds.size()>18){
		//size 超过20的话，开放平台认为是错误的调用
			break;
		}
		if(!LastWeekLoginUser::instance().isLogin(ids.at(i))){
			needToNotifyIds.push_back(ids.at(i));
		}
	}
	MCE_INFO("ActiveUserNotifyTask::parameters --> " << _userid << " " << ids.size() << " " << needToNotifyIds.size());
	if(needToNotifyIds.empty()) {
		return map<string, string>();
	}

	ostringstream buddyids;
	for(size_t i = 0; i < needToNotifyIds.size(); ++i) {
	 if(i) {
		 buddyids << ",";
	 }
	 buddyids << needToNotifyIds.at(i);
	}
	paras["to_ids"] = buddyids.str();

	ostringstream msg[3];

	msg[0] << "赶紧跟<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>一起用<a href=\"http://im.renren.com/?subver=3&im1.2-2.0&notice01\">人人桌面</a>吧！不开网页，快速查看、快速回复、快速分享！巨快无比！<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?im1.2-2.0&notice01\">我也来试试</a>";
	
	msg[1] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>告诉你人人网“快速”小诀窍—<a href=\"http://im.renren.com/?subver=3&im1.2-2.0&notice02\">人人桌面</a>！快速查看新鲜事、秒速回复！<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?im1.2-2.0&notice02\">好，去试试</a>";
	
	msg[2] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>又登录了<a href=\"http://im.renren.com/?subver=3&im1.2-2.0&notice03\">人人桌面</a>，抢到了好多沙发！比比看谁抢的快！<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?im1.2-2.0&notice03\">我也来试试</a>";
	
	paras["notification"] = msg[abs(_userid)%3].str();*/
	return paras;
}
void ActiveUserNotifyTask::result(const RestResultPtr& content){
	//MCE_DEBUG("UseImNotifyTask::result --> ");
}
void ActiveUserNotifyTask::timeout(){
	//MCE_DEBUG("UseImNotifyTask::result --> timeout");
}

//////////////////////////////////////////////////////
map<string, string> ActiveUserNotifyTask2::parameters(){
	map<string, string> paras;
  /*
	paras["method"] = "notifications.send";
	paras["session_key"] = boost::lexical_cast<string>(_userid);

	MyUtil::IntSeq ids = BuddyByOnlineTimeCacheAdapter::instance().getFriendList(_userid, 50);
	MyUtil::IntSeq needToNotifyIds;
	for(size_t i = 0; i < ids.size(); ++i) {
		if(needToNotifyIds.size()>18){
		//size 超过20的话，开放平台认为是错误的调用
			break;
		}
		if(!LastWeekLoginUser::instance().isLogin(ids.at(i))){
			needToNotifyIds.push_back(ids.at(i));
		}
	}
	MCE_INFO("ActiveUserNotifyTask2::parameters --> " << _userid << " " << ids.size() << " " << needToNotifyIds.size());
	if(needToNotifyIds.empty()) {
		return map<string, string>();
	}

	ostringstream buddyids;
	for(size_t i = 0; i < needToNotifyIds.size(); ++i) {
	 if(i) {
		 buddyids << ",";
	 }
	 buddyids << needToNotifyIds.at(i);
	}
	paras["to_ids"] = buddyids.str();

	ostringstream msg[3];

	msg[0] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>发现了<a href=\"http://im.renren.com/?subver=3&notice01\">人人桌面</a>新秘密！设置安静模式，新鲜事默默接收，无弹窗打扰！<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?notice01\">我也来试试</a>！";
	msg[1] << "开网页？No！不断刷新？No！<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>刚刚用了<a href=\"http://im.renren.com/?subver=3&notice02\">人人桌面</a>，不开网页，小窗口直接分享看视频，<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?notice02\">我也来“快速”一把</a>！";
	msg[2] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>刚刚登录了最快速的回复工具---<a href=\"http://im.renren.com/?subver=3&notice03\">人人桌面</a>，与好友极速互动，<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?notice03\">我也来安装试试</a>！";
	
	//msg[1] << "<a href=\"http://im.renren.com/new.html?notice02\">人人桌面2.0</a>公测火爆开始啦！你的好友<xn:name uid=\""<<_userid<<"\" linked=\"true\"/> 已经抢到了公测名额！不要落后，<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?notice02\">快来抢用</a>！";
	
	//msg[2] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>参加了<a href=\"http://im.renren.com/new.html?notice03\">人人桌面2.0</a>新版公测！秀头像、看视频、刷新鲜事儿、做秒回达人！！<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?notice03\">你也试试</a>！";
	
	//msg[3] << "你的好友<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>抢先体验了<a href=\"http://im.renren.com/new.html?notice04\">人人桌面2.0</a>公测版！不开网页看视频分享，“秒回”  状态直接群聊！<a href=\"http://im.renren.com/desktop/rrsetup-2.0-3.exe?notice04\">点击下载</a>！";
	
	
	paras["notification"] = msg[abs(_userid)%3].str();*/
	return paras;
}
void ActiveUserNotifyTask2::result(const RestResultPtr& content){
	//MCE_DEBUG("UseImNotifyTask2::result --> ");
}
void ActiveUserNotifyTask2::timeout(){
	//MCE_DEBUG("UseImNotifyTask2::result --> timeout");
}

/////////////////////////////////////////////////////////
map<string, string> SendNoticeTask::parameters(){
	map<string, string> paras;

	paras["method"] = "notifications.send";
	paras["session_key"] = boost::lexical_cast<string>(_userid);

	if(_toIds.empty()){
		try{
			_toIds = BuddyByOnlineTimeCacheAdapter::instance().getFriendList(_userid, 20);
		}catch(Ice::Exception& e){
			MCE_WARN("SendNoticeTask::parameters-->BuddyByOnlineTimeCacheAdapter::getFriendList-->" << _userid);
		}
	}
	if(_toIds.empty()) {
		return map<string, string>();
	}

	MCE_INFO("SendNoticeTask::parameters --> " << _userid << " " << _toIds.size());
	ostringstream buddyids;
	for(size_t i = 0; i < _toIds.size(); ++i) {
	 if(i) {
		 buddyids << ",";
	 }
	 buddyids << _toIds.at(i);
	}
	paras["to_ids"] = buddyids.str();
	paras["notification"] = _data;
	return paras;
}
void SendNoticeTask::result(const RestResultPtr& content){
	MCE_DEBUG("SendNoticeTask::result ");
}
void SendNoticeTask::timeout(){
	//MCE_DEBUG("UseImNotifyTask::result --> timeout");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
map<string, string> SendNoticeLobbyTask::parameters(){
  map<string, string> paras;
  paras["method"] = "notifications.send";
  paras["session_key"] = boost::lexical_cast<string>(_userid);
  paras["api_key"] = "2e98186998f44a4abb00f1c32c7a6981";
  if(_toIds.empty()){
    try{
      _toIds = BuddyByOnlineTimeCacheAdapter::instance().getFriendList(_userid, 20);
    }catch(Ice::Exception& e){
      MCE_WARN("SendNoticeLobbyTask::parameters-->BuddyByOnlineTimeCacheAdapter::getFriendList-->" << _userid);
    }
  }
  if(_toIds.empty()) {
    return map<string, string>();
  }
  MCE_INFO("SendNoticeLobbyTask::parameters --> " << _userid << " " << _toIds.size());
  ostringstream buddyids;
  for(size_t i = 0; i < _toIds.size(); ++i) {
    if(i) {
      buddyids << ",";
    }
    buddyids << _toIds.at(i);
  }
  paras["to_ids"] = buddyids.str();
  paras["notification"] = _data;
  return paras;
}
void SendNoticeLobbyTask::result(const RestResultPtr& content){
  MCE_DEBUG("SendNoticeLobbyTask::result ");
}
void SendNoticeLobbyTask::timeout(){
}
map<string, string> GetSessionKeyTask::parameters(){
	map<string, string> paras;
	paras["method"] = "xnim.createSession";
	paras["session_key"] = boost::lexical_cast<string>(jid_->userId);
	string ticket = "";
	paras["uid"] = boost::lexical_cast<string>(jid_->userId);
	try{
		ticket = TicketAdapter::instance().createTicket(jid_->userId, "");
	}catch(Ice::Exception& e){
		MCE_WARN("GetSessionKeyTask::parameters-->TicketAdapter::createTicket-->" << jid_->userId << " e = " << e);
	}
	paras["ticket"] = ticket;
	return paras;
}
void GetSessionKeyTask::result(const RestResultPtr& content){
	xml_document doc, tmp;
	tmp.load(content->xml.c_str());
	xml_node node = tmp.child("xnim_createSession_response");
	string sessionkey = node.first_child().value();


	xml_node iqNode = doc.append_child();
	iqNode.set_name("iq");
	iqNode.append_attribute("type") = "result";
	iqNode.append_attribute("id") = iqid_;
	xml_node queryNode = iqNode.append_child();
	queryNode.set_name("query");
	queryNode.append_attribute("xmlns") = "http://talk.renren.com/getsessionkey";
	xml_node key_node = queryNode.append_child();
	key_node.set_name("sessionkey");
	key_node.append_child(node_pcdata).set_value(sessionkey.c_str());
	MCE_INFO("GetSessionKeyTask::result --> userid = " << jid_->userId << " xml = " << doc.xml());
	MessagePtr mPtr = new Message();
	mPtr->to = jid_;
	mPtr->type = IQ_SET;
	mPtr->msg = doc.xml();
	try{
	TalkDeliverAdapter::instance().deliver(mPtr);
	}catch(Ice::Exception& e){
		MCE_WARN("GetSessionKeyTask::result-->TalkDeliverAdapter::deliver-->" << e);
		}

}
void GetSessionKeyTask::timeout(){
		MCE_DEBUG("GetSessionKeyTask::timeout!!!");
}
////////////////////////////////////////////////////////////////////////////////
/*void LastWeekLoginUser::handle(){
	bool load = false;
	{
		int day = Date(time(NULL)).day();
        	IceUtil::Mutex::Lock lock(_mutex);
		if(_users.empty() || day!=_day){
			_day = day;
			load = true;
		}
	}
	if(!load){
		MCE_INFO("LastWeekLoginUser::handle --> need not load");
		return;
	}
	Statement sql;
        sql << "select distinct userid from xntalk_version_dist where time>subdate(date(from_unixtime(" << time(NULL)
        << ")), interval 7 day) and time<=date(from_unixtime(" << time(NULL) << "))";
        
	mysqlpp::StoreQueryResult res =
        QueryRunner("im_stat", CDbRServer).store(sql);
        set<int> ids;
        if (res) {
                for(int i=0; i<res.num_rows(); i++){
                        mysqlpp::Row row = res.at(i);
                        int userid = (int) row["userid"];
                        ids.insert(userid);
                }
        }
        MCE_INFO("LastWeekLoginUser::handle --> last week login user size:" << ids.size());
        IceUtil::Mutex::Lock lock(_mutex);
        _users.swap(ids);
	_inited = true;
}*/






