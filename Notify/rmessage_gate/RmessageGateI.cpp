#include "RmessageGateI.h"

#include <cstring>
#include <boost/algorithm/string/replace.hpp>
#include "pugixml.hpp"
#include "Notify/util/headurl.h"
#include "Notify/util/json_escape.h"
#include "Notify/util/schema_manager.h"
#include "Notify/util/time_stat.h"
#include "Notify/util/url_encode.h"
#include "Notify/util/clear_silver_wrap.h"
#include "NotifyContentAdapter.h"
#include "NotifyIndexAdapter.h"

using namespace std;
using namespace xce::notify;
using namespace MyUtil;
using namespace xce::feed;

class MessageCenterRequest : public Request {
public:
  MessageCenterRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();

  static ctemplate::Template * GetTemplate() {
    // TODO : lock
    if (!template_) {
      template_ = ctemplate::Template::GetTemplate("../etc/rmessage-center/message-center.tpl", ctemplate::DO_NOT_STRIP);
    }
    return template_; 
  }
private:
  static ctemplate::Template * template_;
};

ctemplate::Template * MessageCenterRequest::template_ = NULL;

class FeedRequestFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/rmessage/rmessage-center.html") == 0) {
        return new MessageCenterRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&RmessageGateI::instance(), service.createIdentity("M", ""));
  MessageCenterRequest::GetTemplate();

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service."
      + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO(service.getName() << " Fcgi listens on : " << fcgi_socket);

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 64);
  fcgi_server->RegisterRequestFactory(new FeedRequestFactory());
  fcgi_server->Start();
}

struct TplIncludeInfo {
  TplIncludeInfo(int id, const char * name, const char * file) : type_id(id), dict_name(name), tpl_file(file) {}

  int type_id; // 注意不是schema_id
  string dict_name;
  string tpl_file;
};

bool MessageCenterRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  if (!Request::response()) {
    stringstream ss;
    ss << "Location: " << "http://www.renren.com/SysHome.do?origURL="
      << UrlEncode("http://notify.renren.com/rmessage/rmessage-center.html")
      << "\r\n\r\n";
    FCGX_PutS(ss.str().c_str(), _fcgi_out);
    return false;
  }

  string emptyRet = "Content-type: text/html\r\n\r\n{}";

  FCGX_PutS("Content-type: text/html; charset=utf-8\r\n\r\n", _fcgi_out);

  ctemplate::TemplateDictionary dict("msg-center");
  ctemplate::Template * tpl = MessageCenterRequest::GetTemplate();

  map<int, TplIncludeInfo> sub_tpls;
  sub_tpls.insert(make_pair(2001, TplIncludeInfo(2001, "REMIND_LEAVE_WORD", "../etc/rmessage-center/remind-leave-word.tpl")));

  sub_tpls.insert(make_pair(2002, TplIncludeInfo(2001, "APPLY_FRIEND_REQUEST", "../etc/rmessage-center/apply-friend-request.tpl")));
  sub_tpls.insert(make_pair(2003, TplIncludeInfo(2001, "APPLY_FRIEND_RECOMMEND", "../etc/rmessage-center/apply-friend-recommend.tpl")));
  sub_tpls.insert(make_pair(2004, TplIncludeInfo(2001, "APPLY_QUANREN", "../etc/rmessage-center/apply-quanren.tpl")));
  sub_tpls.insert(make_pair(2005, TplIncludeInfo(2001, "APPLY_ADDRESS_LIST", "../etc/rmessage-center/apply-address-list.tpl")));
  sub_tpls.insert(make_pair(2006, TplIncludeInfo(2001, "APPLY_DECORATION_GIFT", "../etc/rmessage-center/apply-decoration-gift.tpl")));
  sub_tpls.insert(make_pair(2007, TplIncludeInfo(2001, "APPLY_GROUP_JOIN", "../etc/rmessage-center/apply-group-join.tpl")));
  sub_tpls.insert(make_pair(2008, TplIncludeInfo(2001, "APPLY_PAGE_RECOMMEND", "../etc/rmessage-center/apply-page-recommend.tpl")));

  sub_tpls.insert(make_pair(2009, TplIncludeInfo(2001, "NOTICE_GAMES", "../etc/rmessage-center/notice-games.tpl")));
  sub_tpls.insert(make_pair(2010, TplIncludeInfo(2001, "NOTICE_VIP", "../etc/rmessage-center/notice-vip.tpl")));
  sub_tpls.insert(make_pair(2011, TplIncludeInfo(2001, "NOTICE_HELLO", "../etc/rmessage-center/notice-hello.tpl")));

  map<int, ctemplate::TemplateDictionary *> sub_dicts;
  NotifyBodySeq rmessages = NotifyIndexAdapter::instance().GetRMessageByBigtype(_login_uid, 0, 10, 1);

//MyUtil::LongSeq rmessage_ids;
//rmessage_ids.reserve(rmessages.size());
//for(size_t i = 0; i < rmessages.size(); ++i) {
//  rmessage_ids.push_back(rmessages[i].id);
//}

//NotifyContentDict rmessage_contents = NotifyContentAdapter::instance().getNotifyDict(rmessage_ids);

  for(map<int, TplIncludeInfo>::const_iterator it = sub_tpls.begin(); it != sub_tpls.end(); ++it) {
    MCE_DEBUG("add sub tpl : type = " << it->first << " file=" << it->second.tpl_file);
    ctemplate::TemplateDictionary * sub_dict = dict.AddIncludeDictionary(it->second.dict_name);
    sub_dict->SetFilename(it->second.tpl_file);

    if(it->first == 2002) { // 测试好友请求SECTION
      ctemplate::TemplateDictionary * sec_dict = sub_dict->AddSectionDictionary("LI_SEC");
      sec_dict->SetValue("test", "test-value");
      sec_dict->SetValue("test2", "test2-value");

      sec_dict = sub_dict->AddSectionDictionary("LI_SEC");
      sec_dict->SetValue("test", "test-value-2");
      sec_dict->SetValue("test2", "test2-value-2");
    } 

    sub_dicts[it->first] = sub_dict;
  }
  
  for(size_t i = 0; i < rmessages.size(); ++i) {
    int type = rmessages[i]->type & 0x7FFFFFFF;

    map<int, ctemplate::TemplateDictionary *>::iterator it = sub_dicts.find(type);

    if (it == sub_dicts.end()) {
      continue;
    }
    ctemplate::TemplateDictionary * sub_dict = it->second;

    MCE_DEBUG("add section. type = " << type);
    ctemplate::TemplateDictionary * sec_dict = sub_dict->AddSectionDictionary("LI_SEC");
    map<string, string>::iterator prop_it = rmessages[i]->props.begin();
    while(prop_it != rmessages[i]->props.end()) {
      sec_dict->SetValue(prop_it->first, prop_it->second);
    }
  }

  // leave_word_dict->SetFilename("/data/xce/Notify/etc/rmessage-center/remind-leave-word.tpl");

  string res;
  tpl->Expand(&res, &dict);

  FCGX_PutS(res.c_str(), _fcgi_out);

  return true;
}

