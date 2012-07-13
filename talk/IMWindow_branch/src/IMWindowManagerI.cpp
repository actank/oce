#include "IMWindowManagerI.h"
#include "HttpUtil.h"
#include <boost/lexical_cast.hpp>
#include <ctemplate/template.h>
#include <ctemplate/template_dictionary.h>
#include "Notify/util/fcgi_request.h"
#include "XmppTools.h"
#include "pugixml.hpp"
#include "TalkDeliverAdapter.h"
#include "MessageType.h"
#include "OnlineCenterAdapter.h"
#include "ActiveUserNotify.h"
using namespace talk::online::adapter;
using namespace mop::hi::svc::adapter;
using namespace pugi;
using namespace com::xiaonei::talk::util;
using namespace talk::window;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	service.getAdapter()->add(&IMWindowManagerI::instance(), service.createIdentity("M", ""));
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO("Fcgi listens on : " << fcgi_socket);
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 128);
  fcgi_server->RegisterRequestFactory(new IMWindowRequestFactory());
  fcgi_server->Start();

	TaskManager::instance().scheduleRepeated(new CheckUserSendTimer(KICKTIMESTAMP * 1000));
	PhotoUrlHelper::instance().StartReloadTimer();

	string configFilePath = props->getPropertyWithDefault("Service.ConfigFilePath", "/data/xce/XNTalk/etc/IMWindow/IMWindow.conf");
	IMWindowManagerI::instance().SetPath(configFilePath);
	//IMWindowManagerI::instance().ReloadPathConfig();

  TaskManager::instance().scheduleRepeated(&ActiveUserNotify::instance());
  int index = service.getCommunicator()->getProperties()->getPropertyAsInt("Index");
  MCE_INFO("MyUtil::initialize.   index:"<<index);
  ActiveUserNotify::instance().setIndex(index);
}

RequestPtr IMWindowRequestFactory::Create(FCGX_Request * r) {
	char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
	if (path) {
		return new IMWindowRequest(r);
	}
	return NULL;
}

void IMWindowRequest::ErrorResponse(int status_code){
	ostringstream res;
	if(404 == status_code){
		res << "HTTP/1.1 404 Not Found" << "Content-type: text/html\r\n\r\n SORRY NOT FOUND";
	}
	else if(403 == status_code){
		res << "HTTP/1.1 404 Not Found" << "Content-type: text/html\r\n\r\n SORRY PLEASE LOGIN FIRST";
	}
	FCGX_PutS(res.str().c_str(), _fcgi_out);
}
bool IMWindowRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("_fcgi_out null");
    return false;
  }

	/*/---------------------------------------------------
	MCE_INFO("IMWindowRequest::response --> _login_uid = " << _login_uid << " _path = " << _path);
	for(Str2StrMap::iterator it = _cookies.begin(); it != _cookies.end(); ++it){
		MCE_INFO("IMWindowRequest::response --> key:" << it->first << " value:" << it->second);
	}
	for(Str2StrMap::iterator it = _props.begin(); it != _props.end(); ++it){
		MCE_INFO("IMWindowRequest::response --> key:" << it->first << " value:" << it->second);
	}
	//---------------------------------------------------*/


	stringstream res;
	PathConfigPtr config = IMWindowManagerI::instance().GetConfigByPath(_path);
	
	if(!config){
		ErrorResponse(404);
		MCE_WARN("IMWindowRequest::response --> could not found the path");
		return false;
	}
	ctemplate::Template* tpl = ctemplate::Template::GetTemplate(config->template_path, ctemplate::DO_NOT_STRIP);
	if (!tpl) {
		ErrorResponse(404);
		MCE_WARN("IMWindowRequest::response --> could not found the tpl path:" << config->template_path);
		return false;
	}
	else{
		if("template" != config->type){
			ErrorResponse(404);
			MCE_WARN("IMWindowRequest::response --> type is wrong type:" << config->type)
			return false;
		}
		TemplateDataCollectorPtr collector = IMWindowManagerI::instance().GetTempCollector(_path);
		if(!collector){
			ErrorResponse(404);
			MCE_WARN("IMWindowRequest::response --> could not found the collector")
			return false;
		}
		ctemplate::TemplateDictionary dict("page_template");
		bool success = collector->fillData(_path, _cookies, _props, dict);
		if(!success){
			ErrorResponse(403);
			MCE_WARN("IMWindowRequest::response --> userinfo error")
			return false;
		}
    res << "HTTP/1.1 200 " << GetReasonPhrase(200) << "\r\n"
        << "Content-type: " << config->content_type<< "\r\n" << "Cache-Control: no-cache"<< "\r\n" << "Expires: Thu, 01 Jan 1970 00:00:00 GMT"<< "\r\n";
		string ans;
		tpl->Expand(&ans, &dict);
		res << "\r\n" << ans;
	}
	//MCE_INFO(res.str());
  FCGX_PutS(res.str().c_str(), _fcgi_out);
  return true;
}

void IMWindowManagerI::SetNotifyData(const string& notifydata,  const Ice::Current&) {
  ActiveUserNotify::instance().SetNotifyData(notifydata);
}
void IMWindowManagerI::NeedNotify(int userid, float ver,  const Ice::Current&) {
  ActiveUserNotify::instance().needNotify(userid,ver);
}

void IMWindowManagerI::ReloadStaticPages(const Ice::Current&){
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	map<string, PathConfigPtr>::iterator it = _config.begin();
	for (; it != _config.end(); ++it) {
		//if (it->second->type != "template") {
			ctemplate::Template* tpl = ctemplate::Template::GetTemplate(it->second->template_path, ctemplate::DO_NOT_STRIP);
			if (tpl) {
				MCE_INFO("IMWindowManagerI::ReloadStaticPages --> " << it->second->template_path);
				tpl->ReloadIfChanged();
			}
		//}
	}
}

void IMWindowManagerI::ReloadPathConfig(const Ice::Current&){
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	xml_document doc;
	if (doc.load_file(_path.c_str())) {
		xml_node configNode = doc.child("config");
		for (xml_node n = configNode.child("page"); n; n = n.next_sibling("page")) {

			string url = n.attribute("url").value();
			if (url != "") {
				PathConfigPtr c = new PathConfig();
				c->template_path = n.attribute("file").value();
				c->timeout = boost::lexical_cast<int>(n.attribute("timeout").value());
				c->type = n.attribute("type").value();
				c->reminder_type= n.attribute("reminder_type").value();
				c->width= n.attribute("width").value();
				c->height= n.attribute("height").value();
				c->url= n.attribute("get_url").value();
				c->showtime= n.attribute("showtime").value();
				c->topmost= n.attribute("topmost").value();
				c->title= n.attribute("title").value();

				_config[url] = c;
			}
		}
	}
	MCE_INFO("IMWindowManagerI::ReloadPathConfig --> _config.size=" << _config.size());
}


PathConfigPtr IMWindowManagerI::GetConfigByPath(const string& path){
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	PathConfigPtr config;
	map<string, PathConfigPtr>::iterator it = _config.find(path);
	if(it == _config.end()){
		xml_document doc;
		if (doc.load_file(_path.c_str())) {
			xml_node configNode = doc.child("config");
			for (xml_node n = configNode.child("page"); n; n = n.next_sibling("page")) {
				string url = n.attribute("url").value();
				if (url != "") {
					if(_config.count(url) <= 0){
						PathConfigPtr c = new PathConfig();
						c->template_path = n.attribute("file").value();
						c->timeout = boost::lexical_cast<int>(n.attribute("timeout").value());
						c->type = n.attribute("type").value();
						c->content_type = n.attribute("content_type").value();
						c->reminder_type= n.attribute("reminder_type").value();
						c->width= n.attribute("width").value();
						c->height= n.attribute("height").value();
						c->url= n.attribute("get_url").value();
						c->showtime= n.attribute("showtime").value();
						c->topmost= n.attribute("topmost").value();
						c->title= n.attribute("title").value();
						_config[url] = c;
						if(path == url){
							config = c;
						}
					}
				}
			}
		}
	}
	if(!config){
		it = _config.find(path);
		if(it != _config.end()){
			config = it->second;
		}
	}
	return config;
}

TemplateDataCollectorPtr IMWindowManagerI::GetTempCollector(const string& path){
	if("/imuserinfo/getuserinfo.do" == path){
		return new UserInfoCollector();
	}
	else if(!(path.find("/imadinfo"))){
		return new StaticAdInfoCollector();
	}
	return NULL;
}


MessageSeq IMWindowManagerI::GetXmppMessage(const PathConfigPtr& config, const MyUtil::IntSeq& userids){
	JidSeqMap jids;
	try{
		jids = OnlineCenterAdapter::instance().getUsersJids(userids, 4);
	}catch(Ice::Exception& e){
		MCE_WARN("IMWindowManagerI::GetXmppMessage-->OnlineCenterAdapter::getUsersJids-->size:" << userids.size() << " error:" << e);
	}
	MessageSeq ans;
	for(JidSeqMap::iterator mit = jids.begin(); mit != jids.end(); ++mit){
		for(JidSeq::iterator vit = mit->second.begin(); vit != mit->second.end(); ++vit){
			MessagePtr msg = GetXmppMessage(config, (*vit));
			ans.push_back(msg);
		}
	}
	return ans;
}
MessagePtr IMWindowManagerI::GetXmppMessage(const PathConfigPtr& config, const JidPtr& jid){
	MCE_INFO("IMWindowManagerI::GetXmppMessage--> jid:" << jidToString(jid));
	xml_document doc;
	xml_node messageNode = doc.append_child();
	messageNode.set_name("message");
	messageNode.append_attribute("type").set_value("reminder");
	messageNode.append_attribute("from").set_value(jidToString(jid).c_str());
	xml_node remindNode = messageNode.append_child();
	remindNode.set_name("reminder");
	remindNode.append_attribute("type").set_value(config->reminder_type.c_str());
	remindNode.append_attribute("xmlns").set_value("http://talk.renren.com/reminder");
	
	xml_node widthNode = remindNode.append_child();
	widthNode.set_name("width");
	widthNode.append_child(node_pcdata).set_value(config->width.c_str());

	xml_node heightNode = remindNode.append_child();
	heightNode.set_name("height");
	heightNode.append_child(node_pcdata).set_value(config->height.c_str());


	xml_node urlNode = remindNode.append_child();
	urlNode.set_name("url");
	urlNode.append_child(node_pcdata).set_value(config->url.c_str());
	
	xml_node showtimeNode = remindNode.append_child();
	showtimeNode.set_name("showtime");
	showtimeNode.append_child(node_pcdata).set_value(config->showtime.c_str());

	xml_node topmostNode = remindNode.append_child();
	topmostNode.set_name("topmost");
	topmostNode.append_child(node_pcdata).set_value(config->topmost.c_str());


	xml_node titleNode = remindNode.append_child();
	titleNode.set_name("title");
	titleNode.append_child(node_pcdata).set_value(config->title.c_str());

	MessagePtr msg = new Message();
	msg->from = jid;
	msg->to = jid;
	msg->type = NOTIFY2_MESSAGE;
	msg->msg = doc.xml();

	return msg;
}

IntSeq IMWindowManagerI::UserSendCheck(const MyUtil::IntSeq& userids){
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	IntSeq ans;
	for(IntSeq::const_iterator uit = userids.begin(); uit != userids.end(); ++uit){
		if(_usersendcache.count((*uit)) > 0){
			//MCE_INFO("IMWindowManagerI::UserSendCheck-->userid:" << (*uit) << " has send in " << KICKTIMESTAMP << " minites do not send to him");
			continue;
		}
		_usersendcache.insert(make_pair<int, time_t>((*uit), time(NULL)));
		ans.push_back((*uit));
	}
	return ans;
}

void IMWindowManagerI::KickTimeoutUser(){
	map<int, time_t> usersendcache;
	{
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		usersendcache = _usersendcache;	
		MCE_INFO("IMWindowManagerI::KickTimeoutUser-->kick timeout users beforcheck size:" << _usersendcache.size());
	}
	time_t now = time(NULL);
	for(map<int, time_t>::iterator mit = usersendcache.begin(); mit != usersendcache.end();){
		//MCE_INFO("IMWindowManagerI::KickTimeoutUser--> userid:" << mit->first << " last send time:" << mit->second << " now:" << now << "  now - mit->second:" << now - mit->second);
		if(now - mit->second >= 30){
			usersendcache.erase(mit++);
		}else{
			++mit;
		}
	}
	{
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		_usersendcache = usersendcache;
	}
}



void IMWindowManagerI::UserAdReminderOnly(const string& path, const JidPtr& jid,  const Ice::Current&){
	MCE_DEBUG("IMWindowManagerI::UserAdReminderOnly-->head_TalkDeliverAdapter::deliver-->userid:" << jid->userId << " path:" << path);
	PathConfigPtr config = GetConfigByPath(path);
	if(!config){
		MCE_WARN("IMWindowManagerI::UserAdReminderOnly-->get config error userid:" << jid->userId << " path:" << path);
		return;
	}
	IntSeq userids;
	userids.push_back(jid->userId);
	IntSeq checkids = UserSendCheck(userids);
	if(checkids.empty()){
		return;
	}
	MessageSeq mseq;
	MessagePtr msg = GetXmppMessage(config, jid);
	mseq.push_back(msg);
	try{
		TalkDeliverAdapter::instance().deliver(mseq);
	}catch(Ice::Exception& e){
		MCE_WARN("IMWindowManagerI::UserAdReminderOnly-->TalkDeliverAdapter::deliver-->userid:" << jid->userId << " error");
	}
	MCE_INFO("IMWindowManagerI::UserAdReminderOnly-->TalkDeliverAdapter::deliver-->userid:" << jid->userId << " path:" << path);
}

void IMWindowManagerI::UserAdReminder(const string& path, int userid,  const Ice::Current&){
	PathConfigPtr config = GetConfigByPath(path);
	if(!config){
		MCE_WARN("IMWindowManagerI::UserAdReminder-->get config error userid:" << userid << " path:" << path);
		return;
	}
	IntSeq userids;
	userids.push_back(userid);
	IntSeq checkids = UserSendCheck(userids);
	if(checkids.empty()){
		return;
	}
	MessageSeq mseq = GetXmppMessage(config, checkids);
	try{
		TalkDeliverAdapter::instance().deliver(mseq);
	}catch(Ice::Exception& e){
		MCE_WARN("IMWindowManagerI::UserAdReminder-->TalkDeliverAdapter::deliver-->userid:" << userid << " error");
	}
	MCE_INFO("IMWindowManagerI::UserAdReminder-->TalkDeliverAdapter::deliver-->userid:" << userid << " path:" << path);
}
void IMWindowManagerI::UsersAdReminder(const string& path, const MyUtil::IntSeq& userids, const Ice::Current&){
	if(userids.size() >= 300 || userids.empty()){
		MCE_INFO("IMWindowManagerI::UsersAdReminder-->userids.size():" << userids.size() << " is too large, please split it, max is 300");
		return;
	}
	PathConfigPtr config = GetConfigByPath(path);
	if(!config){
		MCE_WARN("IMWindowManagerI::UserAdReminder-->get config error userid.size:" << userids.size() << " path:" << path);
		return;
	}
	//IntSeq checkids = UserSendCheck(userids);
	IntSeq checkids = userids;
	if(checkids.empty()){
		return;
	}
	MessageSeq mseq = GetXmppMessage(config, checkids);
	MCE_INFO("IMWindowManagerI::UsersAdReminder--> deliver size :" << mseq.size());
	try{
		TalkDeliverAdapter::instance().deliver(mseq);
	}catch(Ice::Exception& e){
		MCE_WARN("IMWindowManagerI::UserAdReminder-->TalkDeliverAdapter::deliver-->users.size:" << userids.size() << " error");
	}
}

void IMWindowManagerI::UserLoginReminder(const JidPtr& jid, const Ice::Current&){
	string path = "/imuserinfo/getuserinfo.do";
	PathConfigPtr config = GetConfigByPath(path);
	if(!config){
		MCE_WARN("IMWindowManagerI::UserAdReminder-->get config error jid:" << jidToString(jid) << " path:" << path);
		return;
	}
	MessagePtr msg = GetXmppMessage(config, jid);
	try{
		TalkDeliverAdapter::instance().deliver(msg);
	}catch(Ice::Exception& e){
		MCE_WARN("IMWindowManagerI::UserLoginReminder-->TalkDeliverAdapter::deliver-->jid:" << jidToString(jid) << " error");
	}

}




void CheckUserSendTimer::handle(){
	IMWindowManagerI::instance().KickTimeoutUser();
}










