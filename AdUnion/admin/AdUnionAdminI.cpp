#include "AdUnionAdminI.h"

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "AdUnionGateAdapter.h"

using namespace xce::notify;

namespace xce {
namespace ad {

class AdminRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/union/getall.html") == 0)
        return new GetAllRequest(r);
      if (strcmp(path, "/union/getone.html") == 0)
        return new GetOneRequest(r);
      if (strcmp(path, "/union/update.html") == 0)
        return new UpdateRequest(r);
      if (strcmp(path, "/union/save.html") == 0)
        return new SaveRequest(r);
      if (strcmp(path, "/union/reload.html") == 0)
        return new ReloadRequest(r);
    }
    return NULL;
  }
};

AdUnionAdminI::AdUnionAdminI()
{
} 

bool AdUnionAdminI::Reload(const Ice::Current&) {
  return false;
}

bool ReloadRequest::response() {
  if (!Request::response() && false)
    return false;

  string out = "Content-type: text/html\r\n\r\n";

  bool ret = false;
  try{
    ret = AdUnionGateAdapter::instance().Reload();
  } catch (Ice::Exception& e){
    MCE_WARN("AdUnionGateAdapter Reload() exception : " << e.what());
    ret = false;
  }

  MCE_INFO("AdUnionGateAdapter Reload() : " << ret);
  if (ret) {
    out += "0";
  } else {
    out += "-1";
  }

  FCGX_FPrintF(_fcgi_out, out.c_str());
  return ret;
}

bool GetOneRequest::response() {
  if (!Request::response() && false)
    return false;
  long aid = getPropertyLong("id");

  Statement sql;
  sql << "SELECT id, type, name, owner_id, group_id, campaign_id, budget, daily_budget, "\
      "click_price, click_count, title, content, img_url, dest_url, update_time FROM ad_union_ads WHERE id=" << aid;
  stringstream ss;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbRServer).store(sql);
    if (!res) {
      FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-3");
      return false;
    }
    ss << "Content-type: text/html\r\n\r\n[";
    for (size_t i = 0; i<res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      if (i) {
        ss << ",";
      }

      string name = row["name"].c_str();
      jsonEscape(name);
      int click_count = (int)row["click_count"];
      int click_price = (int)row["click_price"];
      int budget = (int)row["budget"];

      string title = row["title"].c_str();
      jsonEscape(title);
      string content = row["content"].c_str();
      jsonEscape(content);
      for(size_t i = 0; i < content.size(); ++i) {
        if (content[i] == '\r' || content[i] == '\n') {
          content[i] = ' ';
        }
      }

      ss << "{'id':'" << (long)row["id"] << '\''
         << ",'type':" << (long)row["type"] 
         << ", 'name':'" << name  << '\''
         << ", 'owner_id':'" << row["owner_id"] << '\''
         << ", 'group_id':'" << row["group_id"] << '\''
         << ", 'campaign_id':'" << row["campaign_id"] << '\''
         << ", 'budget':'" << budget << '\''
         << ", 'click_price':'" << click_price << '\''
         << ", 'click_count':'" << click_count << '\''
         << ", 'daily_budget':'" << row["daily_budget"] << '\''
         << ", 'title':'" << title << '\''
         << ", 'content':'" << content << '\''
         << ", 'img_url':'" << row["img_url"] << '\''
         << ", 'dest_url':'" << row["dest_url"] << '\''
         << ", 'update_time':'" << row["update_time"] << '\''
         << "}";
    }
    ss << "]";
  } catch (std::exception& e) {
    MCE_WARN("db err : " << e.what());
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
  } catch (...) {
    MCE_WARN("db unkown exception.");
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-2");
    return false;
  }
  MCE_DEBUG("response : " << ss.str());
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool GetAllRequest::response() {
  if (!Request::response() && false)
    return false;

  Statement sql;
  sql << "SELECT id, type, name, budget, click_price, click_count FROM ad_union_ads";
  stringstream ss;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbRServer).store(sql);
    if (!res) {
      FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-3");
      return false;
    }
    ss << "Content-type: text/html\r\n\r\n[";
    for (size_t i = 0; i<res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      if (i) {
        ss << ",";
      }

      string name = row["name"].c_str();
      jsonEscape(name);
      int click_count = (int)row["click_count"];
      int click_price = (int)row["click_price"];
      int budget = (int)row["budget"];

      ss << "{'id':'" << (long)row["id"]
         << "','type':" << (long)row["type"] 
         << ", 'name':'" << name 
         << "', 'remaining':" << budget - click_count * click_price
         << "}";
    }
    ss << "]";
  } catch (std::exception& e) {
    MCE_WARN("db err : " << e.what());
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
  } catch (...) {
    MCE_WARN("db unkown exception.");
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-2");
    return false;
  }
  MCE_DEBUG("response : " << ss.str());
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool UpdateRequest::response() {
  // http://ebpadmin.renren.com:8080/union/update.html?id=1000004323000300000&k=id&v=1000004323000300000
  if (!Request::response() && false)
    return false;

  Statement sql;

  sql << "UPDATE ad_union_ads SET " << getProperty("k") << "=" << mysqlpp::quote << getProperty("v") << " WHERE id=" << getProperty("id");

  mysqlpp::StoreQueryResult ret;
  try {
    ret = QueryRunner("ad_db", CDbRServer).store(sql);
  } catch (std::exception& e) {
    MCE_WARN("update ad err : " << e.what());
    FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
  } catch (...) {
    MCE_WARN("update ad unkown exception.");
    FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
  }

  stringstream ss;
  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n0" );
  return true;
}

bool SaveRequest::response(){
  //
  // /union/save.html?id=1&n=test&t=1&o=2&g=3&cam=4&b=5&p=1&t=t&ct=test%20content&img=image%20url&dst=dest%20url
  //
  if (!Request::response() && false)
    return false;

  Statement sql;
  string id = getProperty("id");

  sql << "INSERT INTO ad_union_ads (id, type, name, owner_id, group_id, campaign_id, budget, daily_budget, "\
         "click_price, click_count, title, content, img_url, dest_url, update_time)"
      << " VALUES (" << getProperty("id") << ',' << getProperty("tp") << ',' << mysqlpp::quote << getProperty("n") << ','
      << getProperty("o") << ',' << getProperty("g") << ',' << getProperty("cam") << ','
      << getProperty("b") << ',' << getProperty("b") << ',' << mysqlpp::quote << getProperty("p") << ",0,"
      << mysqlpp::quote << getProperty("tt") << ',' << mysqlpp::quote << getProperty("ct") << ',' << mysqlpp::quote << getProperty("img") << ','
      << mysqlpp::quote << getProperty("dst") << ", NOW())";

  int ret_code = -1;
  try {
    mysqlpp::SimpleResult res = QueryRunner("ad_db", CDbWServer).execute(sql);
    if (res) {
      ret_code = res.insert_id();
    }
  } catch (std::exception& e) {
    MCE_WARN("Store ad exception : " << e.what());
  } catch (...) {
    MCE_WARN("Store ad unkown exception.");
  }

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << ret_code;
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());

  return ret_code > 0;
}

}
}

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::ad::AdUnionAdminI::instance(),
          service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO("Fcgi listens on : " << fcgi_socket);

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 4);
  fcgi_server->RegisterRequestFactory(new xce::ad::AdminRequestFactory());
  fcgi_server->Start();
}

