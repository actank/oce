#include "AdHotGateI.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <sstream>
#include <algorithm>

#include "AdConf.h"

#include "Allocator.h"
#include "Notify/util/url_encode.h"
#include "Notify/util/json_escape.h"
#include "UserCountAdapter.h"
#include "AdNotifyAdapter.h"
#include "QueryRunner.h"

using namespace com::xiaonei::xce;
using namespace xce::usercount::adapter;
using namespace com::xiaonei::platform::storage;

const static size_t kDefaultMaxNUMOfUserPerItem = 8;
const static int kDefaultMaxNUMOfUserPerItemVersion2 = 6;
// const static int MaxNUMOfDisplayFriends = 40;

const static string db_instance = "ad_db";

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::ad::AdHotGateI::instance(), service.createIdentity("M", ""));

  xce::ad::AdConfCache::instance().Reload();

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "10.3.18.186:9011");
  MCE_INFO("Fcgi listens on : " << fcgi_socket);
  
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 128);
  fcgi_server->RegisterRequestFactory(new xce::ad::AdHotRequestFactory());
  fcgi_server->Start();

  TaskManager::instance().scheduleRepeated(new xce::ad::ClearCacheTimer());
}


namespace xce {
namespace ad {

void AdHotGateI::Reload(const Ice::Current&) {
  AdConfCache::instance().Reload();
}

void AdHotGateI::ResetUser(Ice::Int uid, const Ice::Current&) {
  AdInfoCache::instance().ClearUserAdInfo(uid);
}

bool AdHotRequest::response() {
  if(!_fcgi_out) {
    MCE_WARN("null fcgi out stream : " << _query << '-' << _cookie_string);
    return false;
  }

  if (!Request::response()) {
    string loc("Location: http://www.renren.com/Login.do?origURL=");
    loc += xce::notify::UrlEncode("http://notify.renren.com/hot/hotnews.html");
    loc += "\r\n\r\n";
    FCGX_PutS(loc.c_str(), _fcgi_out);
    return false;
  }

  MCE_INFO("ad hot get. uid=" << _login_uid);
  FCGX_PutS("Content-type: text/html; charset=utf-8\r\n\r\n", _fcgi_out);

  // 填充辞典
  ctemplate::TemplateDictionary dict("hotnews");
  UserAdInfoPtr user_ad_ptr = AdInfoCache::instance().GetUserAdInfo(_login_uid);
  user_ad_ptr->FillDict(&dict);

  // 加载模板
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate("../etc/ad_hot/tpl/hotnews.tpl", ctemplate::DO_NOT_STRIP);

  string rsp_body;
  tpl->Expand(&rsp_body, &dict);
  FCGX_PutS(rsp_body.c_str(), _fcgi_out);

  try {
    UserCountAdapter::instance().set(_login_uid, CADNOTIFY, 0);
    AdNotifierAdapter::instance().MarkReadByUid(_login_uid);
    MCE_DEBUG("AdHotRequest::response --> MarkReadByUid uid:" << _login_uid);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdHotRequest::response MarkReadByUid Error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdHotRequest::response MarkReadByUid Error:" << e.what());
  }
  return true;
}

bool ExpandRequest::response() {
  if (!Request::response()) {
    return false;
  }

  MCE_INFO("ad hot expand. uid=" << _login_uid);

  Ice::Int ad_id = getPropertyInt("id");

  UserAdInfoPtr user_ad_ptr = AdInfoCache::instance().GetUserAdInfo(_login_uid);
  const PromotionItem * ad_item = user_ad_ptr->GetPromotionItem(ad_id);

  stringstream ss;
  ss << "Content-type: text/html; charset=utf-8\r\n\r\n";

  if (!ad_item) {
    ss << "{\"code\":1,\"msg\":\"您的页面已经过期.\"}";
  } else {
    ss << "{\"code\":0,\"msg\":[";

    const map<Ice::Int, UserCachePtr> & buddies = ad_item->buddies_;
    int count = 0;
    map<Ice::Int, UserCachePtr>::const_iterator it = buddies.begin();
    std::advance(it, kDefaultMaxNUMOfUserPerItem);
    while(it != buddies.end()) {
      if (count > 0) {
        ss << ",";
      }

      ss << "{\"id\":" << it->first << ",\"head\":\""
          << HeadUrlUtil::instance().getHeadFullUrl(it->second->tinyurl())
          << "\",\"name\":\"" << xce::notify::JsonEscape(it->second->name()) << "\"}";
      ++count;
      ++it;
    }

    ss << "]}";
  }
  string response = ss.str();
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool DeleteRequest::response() {
  Request::response();

  MCE_INFO("ad hot delete. uid=" << _login_uid);

  int ad_id = getPropertyInt("id");
  if (_login_uid > 0 && ad_id >= 0) {
    MCE_INFO("DeleteRequest::response --> Block uid:" << _login_uid << "  ad_id" << ad_id);
    try {
      AdNotifierAdapter::instance().Block(_login_uid, ad_id);
    } catch (Ice::Exception& e) {
      MCE_WARN("AdNotifierAdapter block error uid=" << _login_uid << " ad_id=" << ad_id << " : " << e);
    } catch (std::exception& e) {
      MCE_WARN("AdNotifierAdapter block error uid=" << _login_uid << " ad_id=" << ad_id << " : " << e.what());
    }
  }

  stringstream ss;
  ss << "Content-type: text/html; charset=utf-8\r\n\r\n" << "{\"code\":0,\"msg\":\"\"}";
  FCGX_PutS(ss.str().c_str(), _fcgi_out);

  return true;
}

bool NotJoinRequest::response() {
  Request::response();

  MCE_INFO("ad hot not-join. uid=" << _login_uid << " aid=" << getProperty("id"));

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << "{\"code\":0,\"msg\":\"\"}";
  FCGX_PutS(ss.str().c_str(), _fcgi_out);

  return true;
}

bool RedirectRequest::response() {
  if (!_fcgi_out) {
    return false;
  }
  string prop_url = getProperty("url");
  MCE_INFO("ad hot redirect. uid=" << _login_uid << " aid=" << prop_url);

  stringstream ss;
  ss << "Location: " << prop_url << "\r\n\r\n";
  FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
  return true;
}

}
}

