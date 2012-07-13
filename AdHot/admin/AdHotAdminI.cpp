#include "AdHotAdminI.h"

#include <vector>
#include <fstream>
#include <iterator>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "AdNotify.h"
#include "AdNotifyAdapter.h"
#include "AdHotGateAdapter.h"
#include "Notify/util/json_escape.h"
#include "Notify/util/file_upload_request.h"

using namespace xce::notify;

namespace xce {
namespace ad {

static string GetPartnerFile(int aid) {
  stringstream file_name;
  file_name << "../etc/ad_hot/partners/partners-"
            << aid << ".txt";
  return file_name.str();
}

static string GetBlockedFile(int aid) {
  stringstream file_name;
  file_name << "../etc/ad_hot/blocked/blocked-"
            << aid << ".txt";
  return file_name.str();
}


class UploadPartnersRequest : public FileUploadRequest {
public:
	UploadPartnersRequest(FCGX_Request* r) : FileUploadRequest(r) {}

protected:
  virtual string GetLocalFilename() {
    int aid = getPropertyInt("aid");
    if (aid > 0) {
      return GetPartnerFile(aid);
    }
    return string("");
  }
};

class UploadBlockedRequest : public FileUploadRequest {
public:
	UploadBlockedRequest(FCGX_Request* r) : FileUploadRequest(r) {}

protected:
  virtual string GetLocalFilename() {
    int aid = getPropertyInt("aid");
    if (aid > 0) {
      return GetBlockedFile(aid);
    }
    return string("");
  }
};

class ImportPartnersRequest : public Request {
public:
	ImportPartnersRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    stringstream ss;
    ss << "Content-type: application/x-javascript; charset=utf-8\r\n\r\n";
    int ad_id = getPropertyInt("aid");
    if (ad_id <= 0) {
      ss << "[-1, \"错误的广告id\"]";
      FCGX_PutS(ss.str().c_str(), _fcgi_out);
      return false;
    }
    ifstream partner_file(GetPartnerFile(ad_id).c_str());
    if (!partner_file) {
      ss << "[-1, \"无法打开参与者列表文件" << GetPartnerFile(ad_id) << "\"]";
      FCGX_PutS(ss.str().c_str(), _fcgi_out);
      return false;
    }

    istream_iterator<int> eos;
    istream_iterator<int> begin(partner_file);
    int partner_count = 0;
    Statement sql;
    sql << "INSERT IGNORE INTO ad_hot_partner(promotion_id, uid) VALUES ";
    while(true) {
      if (partner_count % 1000 == 999 || begin == eos) {
        if (begin != eos) {
          sql << ",(" << ad_id << "," << *begin << ")";
        }
        try {
          mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbWServer).store(sql);
        } catch (std::exception& e){
          MCE_WARN("insert ad_hot_partner exception : " << e.what());
        }
        if (begin == eos) {
          break;
        }

        sql.clear();
        sql << "INSERT IGNORE INTO ad_hot_partner(promotion_id, uid) VALUES ";
      } else {
        if (partner_count % 1000 == 0) {
          sql << "(" << ad_id << "," << *begin << ")";
        } else {
          sql << ",(" << ad_id << "," << *begin << ")";
        }
      }
      ++partner_count;
      ++begin;
    }
    ss << "[0, \"共有" << partner_count << "参与者保存成功\"]";
    FCGX_PutS(ss.str().c_str(), _fcgi_out);
    return true;
  }
};

class ImportBlockedRequest : public Request {
public:
	ImportBlockedRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    stringstream ss;
    ss << "Content-type: application/x-javascript; charset=utf-8\r\n\r\n";
    int ad_id = getPropertyInt("aid");
    if (ad_id <= 0) {
      ss << "[-1, \"错误的广告id\"]";
      FCGX_PutS(ss.str().c_str(), _fcgi_out);
      return false;
    }
    ifstream partner_file(GetBlockedFile(ad_id).c_str());
    if (!partner_file) {
      ss << "[-1, \"无法打开黑名单列表文件" << GetBlockedFile(ad_id) << "\"]";
      FCGX_PutS(ss.str().c_str(), _fcgi_out);
      return false;
    }

    istream_iterator<int> eos;
    istream_iterator<int> begin(partner_file);
    int id_count = 0;
    Statement sql;
    sql << "INSERT IGNORE INTO ad_hot_blocked(promotion_id, uid) VALUES ";
    while(true) {
      if (id_count % 1000 == 999 || begin == eos) {
        if (begin != eos) {
          sql << ",(" << ad_id << "," << *begin << ")";
        }
        try {
          mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbWServer).store(sql);
        } catch (std::exception& e) {
          MCE_WARN("insert ad_hot_blocked exception : " << e.what());
        }
        if (begin == eos) {
          break;
        }

        sql.clear();
        sql << "INSERT IGNORE INTO ad_hot_blocked(promotion_id, uid) VALUES ";
      } else {
        if (id_count % 1000 == 0) {
          sql << "(" << ad_id << "," << *begin << ")";
        } else {
          sql << ",(" << ad_id << "," << *begin << ")";
        }
      }
      ++id_count;
      ++begin;
    }
    ss << "[0, \"共有" << id_count << "黑名单id保存成功\"]";
    FCGX_PutS(ss.str().c_str(), _fcgi_out);
    return true;
  }
};

class ClearBlockedRequest : public Request {
public:
	ClearBlockedRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    string output("Content-type: application/x-javascript; charset=utf-8\r\n\r\n");
    int ad_id = getPropertyInt("aid");
    if (ad_id <= 0) {
      output += "[-1, \"错误的广告id\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }
    Statement sql;
    sql << "DELETE FROM ad_hot_blocked WHERE promotion_id=" << ad_id;
    try {
      mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbWServer).store(sql);
      output += "[0, \"成功\"]";
    } catch (std::exception& e){
      MCE_WARN("delete ad_hot_blocked exception : " << e.what());
      output += "[-1, \"";
      output += e.what();
      output += "\"]";
    }

    FCGX_PutS(output.c_str(), _fcgi_out);
    return true;
  }
};

class ClearPartnersRequest : public Request {
public:
	ClearPartnersRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    string output("Content-type: application/x-javascript; charset=utf-8\r\n\r\n");
    int ad_id = getPropertyInt("aid");
    if (ad_id <= 0) {
      output += "[-1, \"错误的广告id\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }
    Statement sql;
    sql << "DELETE FROM ad_hot_partner WHERE promotion_id=" << ad_id;
    try {
      mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbWServer).store(sql);
      output += "[0, \"成功\"]";
    } catch (std::exception& e){
      MCE_WARN("delete ad_hot_partner exception : " << e.what());
      output += "[-1, \"";
      output += e.what();
      output += "\"]";
    }

    FCGX_PutS(output.c_str(), _fcgi_out);
    return true;
  }
};

class UserResetRequest : public Request {
public:
	UserResetRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    string output("Content-type: application/x-javascript; charset=utf-8\r\n\r\n");
    int ad_id = getPropertyInt("aid");
    int uid = getPropertyInt("uid");

    if (ad_id <= 0 || uid <= 0) {
      output += "[-1, \"错误的广告id或uid\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }
    try {
      Statement sql;
      sql << "DELETE FROM ad_hot_partner WHERE promotion_id=" << ad_id << " AND uid=" << uid;
      mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbWServer).store(sql);
    } catch (std::exception& e){
      MCE_WARN("delete ad_hot_partner exception : " << e.what());
      output += "[-1, \"从ad_hot_partner表删除失败:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    try {
      Statement sql;
      sql << "DELETE FROM ad_hot_blocked WHERE promotion_id=" << ad_id << " AND uid=" << uid;
      mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbWServer).store(sql);
    } catch (std::exception& e){
      MCE_WARN("delete ad_hot_blocked exception : " << e.what());
      output += "[-1, \"从ad_hot_blocked表删除失败:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    try {
      AdNotifierAdapter::instance().ResetUserAd(uid, ad_id);
    } catch (std::exception& e){
      MCE_WARN("AdNotifierAdapter.ResetUserAd() exception : " << e.what());
      output += "[-1, \"AdNotifierAdapter.ResetUserAd()调用错误:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }
    try {
      AdHotGateAdapter::instance().ResetUser(uid);
    } catch (std::exception& e){
      MCE_WARN("AdHotGateAdapter.ResetUser() exception : " << e.what());
      output += "[-1, \"AdHotGateAdapter.ResetUser()调用错误:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    output += "[0, \"success\"]";
    FCGX_PutS(output.c_str(), _fcgi_out);
    return true;
  }
};


class GetWeekDayRequest : public Request {
public:
	GetWeekDayRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    stringstream output;
    output << "Content-type: text/html; charset=utf-8\r\n\r\n测试时间 ： ";
    const char * days[] = {"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"};
    time_t t = time(NULL);
    output << days[(t / 300) % 7];
    output << " 距离明天还有 " << 300 - (t % 300) << " 秒.";
    FCGX_PutS(output.str().c_str(), _fcgi_out);
    return true;
  }
};

class UserGetAdStateRequest : public Request {
public:
	UserGetAdStateRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    string output("Content-type: application/x-javascript; charset=utf-8\r\n\r\n");
    int ad_id = getPropertyInt("aid");
    int uid = getPropertyInt("uid");

    if (ad_id <= 0 || uid <= 0) {
      output += "[-1, \"错误的广告id或uid\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }
    UserHotAdState user_state;
    try {
      user_state = AdNotifierAdapter::instance().GetUserAdState(uid, ad_id);
    } catch (std::exception& e) {
      MCE_WARN("AdNotifierAdapter.GetUserAdState() exception : " << e.what());
      output += "[-1, \"AdNotifierAdapter.GetUserAdState()调用错误:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    FCGX_PutS(output.c_str(), _fcgi_out);
    stringstream body;

    body << "[0, {"
         << "\"uid\":" << uid << ','
         << "\"ad_id\":" << ad_id << ','
         << "\"weekly_received\":" << user_state.weeklyReceived << ','
         << "\"unread_count\":" << user_state.unreadCount << ','
         << "\"total_buddies\":" << user_state.totalBuddies << ','
         << "\"partner_buddies\":" << user_state.partnerBuddies << ','
         << "\"has_blocked\":" << user_state.hasBlocked << ','
         << "\"has_received\":" << user_state.hasReceived << ','
         << "\"is_partner\":" << user_state.isPartner
         << "}]";
    FCGX_PutS(body.str().c_str(), _fcgi_out);
    return true;
  }
};

class UserLoginRequest : public Request {
public:
	UserLoginRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    string output("Content-type: application/x-javascript; charset=utf-8\r\n\r\n");
    int uid = getPropertyInt("uid");
    MCE_DEBUG("user login " << uid);
    if (uid <= 0) {
      output += "[-1, \"错误的uid\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    try {
      AdNotifierAdapter::instance().Login(uid);
    } catch (std::exception& e){
      MCE_WARN("AdNotifierAdapter.Login() exception : " << e.what());
      output += "[-1, \"AdNotifierAdapter.Login()调用错误:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    output += "[0, \"success\"]";
    FCGX_PutS(output.c_str(), _fcgi_out);
    return true;
  }
};

class AdStopRequest : public Request {
public:
	AdStopRequest(FCGX_Request* r) : Request(r) {
	}
	virtual bool response() {
    string output("Content-type: application/x-javascript; charset=utf-8\r\n\r\n");
    int aid = getPropertyInt("aid", -1);
    MCE_DEBUG("to stop ad " << aid);
    if (aid < 0) {
      output += "[-1, \"错误的热点id\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }


    Statement sql;
    sql << "update ad_hot_promotion set end_time='1970-1-1 0:0:0' where id=" << aid;
    try {
      QueryRunner("ad_db", CDbWServer, "ad_hot_promotion").store(sql);
    } catch (std::exception& e) {
      MCE_WARN("update ad_hot_promotion " << aid << " error : " << e.what());
      output += "[-1, \"db update ad_hot_promotion错误:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    try {
      AdNotifierAdapter::instance().DelPromotion(aid);
      AdHotGateAdapter::instance().Reload();
    } catch (std::exception& e){
      MCE_WARN("AdNotifierAdapter.DelAd() exception : " << e.what());
      output += "[-1, \"AdNotifierAdapter.DelAd()调用错误:";
      output += e.what();
      output += "\"]";
      FCGX_PutS(output.c_str(), _fcgi_out);
      return false;
    }

    output += "[0, \"success\"]";
    FCGX_PutS(output.c_str(), _fcgi_out);
    return true;
  }
};

class AdHotAdminRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/hot/getall.html") == 0)
        return new GetAllRequest(r);
      if (strcmp(path, "/hot/update.html") == 0)
        return new UpdateRequest(r);
      if (strcmp(path, "/hot/reload.html") == 0)
        return new ReloadRequest(r);
      if (strcmp(path, "/hot/getone.html") == 0)
        return new GetOneRequest(r);
      if (strcmp(path, "/hot/save.html") == 0)
        return new SaveRequest(r);

      if (strcmp(path, "/hot/ad-stop.html") == 0)
        return new AdStopRequest(r);
      if (strcmp(path, "/hot/get-week-day.html") == 0)
        return new GetWeekDayRequest(r);

      if (strcmp(path, "/hot/upload-partners.html") == 0)
        return new UploadPartnersRequest(r);
      if (strcmp(path, "/hot/import-partners.html") == 0)
        return new ImportPartnersRequest(r);
      if (strcmp(path, "/hot/clear-partners.html") == 0)
        return new ClearPartnersRequest(r);

      if (strcmp(path, "/hot/upload-blocked.html") == 0)
        return new UploadBlockedRequest(r);
      if (strcmp(path, "/hot/import-blocked.html") == 0)
        return new ImportBlockedRequest(r);
      if (strcmp(path, "/hot/clear-blocked.html") == 0)
        return new ClearBlockedRequest(r);

      if (strcmp(path, "/hot/user-login.html") == 0)
        return new UserLoginRequest(r);
      if (strcmp(path, "/hot/user-reset.html") == 0)
        return new UserResetRequest(r);
      if (strcmp(path, "/hot/user-ad-state.html") == 0)
        return new UserGetAdStateRequest(r);
    }
    return NULL;
  }
};

}
}

void MyUtil::initialize() {
  // 启动 ICE 服务
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::ad::AdHotAdminI::instance(), service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");

  // 启动 FCGI 服务
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 128);
  fcgi_server->RegisterRequestFactory(new xce::ad::AdHotAdminRequestFactory);
  fcgi_server->Start();
}

namespace xce {
namespace ad {

AdHotAdminI::AdHotAdminI() {
} 

bool AdHotAdminI::Reload(const Ice::Current&) {
  return false;
}

bool ReloadRequest::response() {
  // /hot/reload.html
  if (!Request::response() && false)
    return false;

  string out = "Content-type: text/html\r\n\r\n";

  bool ret = true;
  try{
    AdHotGateAdapter::instance().Reload();
    AdNotifierAdapter::instance().Reload();
  } catch (Ice::Exception& e){
    ret = false;
    MCE_WARN("AdHotGateAdapter Reload() exception : " << e.what());
  }

  MCE_INFO("AdHotGateAdapter Reload() : " << ret);
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
  sql << "SELECT id, level, start_time, end_time, min_partners, max_daily_post, pic_url, pic_src, message1, "\
      "message2, join_button, join_title, not_join_title, related_posts FROM ad_hot_promotion WHERE id=" << aid;
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

      ss << "{'id':'" << (int)row["id"] << '\''
         << ",'level':" << (int)row["level"] 
         << ",'start_time':'" << row["start_time"].c_str() << '\''
         << ",'end_time':'" << row["end_time"].c_str() << '\''
         << ",'min_partners':'" << (int)row["min_partners"] << '\''
         << ",'max_daily_post':'" << (int)row["max_daily_post"] << '\''
         << ",'pic_url':'" << row["pic_url"].c_str() << '\''
         << ",'pic_src':'" << row["pic_src"].c_str() << '\''
         << ",'message1':'" << JsonEscape(row["message1"].c_str()) << '\''
         << ",'message2':'" << JsonEscape(row["message2"].c_str()) << '\''
         << ",'join_button':'" << JsonEscape(row["join_button"].c_str()) << '\''
         << ",'join_title':'" << JsonEscape(row["join_title"].c_str()) << '\''
         << ",'not_join_title':'" << JsonEscape(row["not_join_title"].c_str()) << '\''
         << ",'related_posts':'" << JsonEscape(row["related_posts"].c_str()) << '\''
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

  //+----------------+---------------+------+-----+---------+-------+
  //| Field          | Type          | Null | Key | Default | Extra |
  //+----------------+---------------+------+-----+---------+-------+
  //| id             | int(11)       | NO   | PRI |         |       | 
  //| start_time     | datetime      | NO   |     |         |       | 
  //| end_time       | datetime      | NO   |     |         |       | 
  //| level          | int(11)       | YES  |     | 0       |       | 
  //| min_partners   | int(11)       | YES  |     | 6       |       | 
  //| max_daily_post | int(11)       | YES  |     | 0       |       | 
  //| pic_url        | varchar(512)  | YES  |     | NULL    |       | 
  //| pic_src        | varchar(512)  | YES  |     | NULL    |       | 
  //| message1       | varchar(512)  | YES  |     | NULL    |       | 
  //| message2       | varchar(1024) | YES  |     | NULL    |       | 
  //| join_button    | varchar(512)  | YES  |     | NULL    |       | 
  //| join_title     | varchar(64)   | YES  |     | NULL    |       | 
  //| not_join_title | varchar(64)   | YES  |     | NULL    |       | 
  //| related_posts  | varchar(2048) | YES  |     | NULL    |       | 
  //+----------------+---------------+------+-----+---------+-------+
  //   // TODO:
  //   ADD : string desc, int max_partners

  Statement sql;
  sql << "SELECT id, UNIX_TIMESTAMP(start_time) AS start_time, UNIX_TIMESTAMP(end_time) AS end_time,"\
         " level, min_partners, max_daily_post FROM ad_hot_promotion";
  map<int, HotAdInfo> hot_map;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("ad_db", CDbRServer).store(sql);
    if (!res) {
      FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-3");
      return false;
    }

    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int ad_id = (int)row["id"];
      HotAdInfo & ad = hot_map[ad_id];

      ad.ad = ad_id;
      ad.state = kAsUnloaded;
      ad.level = (int)row["level"];
      ad.begintime = (int)row["start_time"];
      ad.endtime = (int)row["end_time"];
      ad.minPartners = (int)row["min_partners"];
      ad.maxDailyPost = (int)row["max_daily_post"];
      ad.dailyPosted = 0;
      ad.partnerCount = 0;

      MCE_DEBUG("load ad_notify_promotion. ad=" << ad_id);
    }
  } catch (std::exception& e) {
    MCE_WARN("db err : " << e.what());
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
  } catch (...) {
    MCE_WARN("db unknown exception.");
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-2");
    return false;
  }

  HotAdInfoSeq hot_seq;
  try {
    hot_seq = AdNotifierAdapter::instance().GetAdInfoSeq();
  } catch (Ice::Exception& e) {
    MCE_WARN("AdNotifierAdapter GetHotAdInfoSeq() exception : " << e.what());
    return false;
  }

  MCE_DEBUG("AdHotNotify ad count : " << hot_seq.size());

  for (size_t i = 0; i < hot_seq.size(); ++i) {
    hot_map[hot_seq[i].ad] = hot_seq[i];
  }

  stringstream ss;
  ss << "Content-type: application/x-javascript; charset=utf-8\r\n\r\n[";

  for (map<int, HotAdInfo>::const_iterator it = hot_map.begin(); it != hot_map.end(); ++it) {
    if (it != hot_map.begin()) {
      ss << ",";
    }
    ss << "{'id':" << it->second.ad << ','
       << "'state':" << it->second.state << ','
       << "'level':" << it->second.level << ','
       << "'begintime':" << it->second.begintime << ','
       << "'endtime':" << it->second.endtime << ','
       << "'min_partners':" << it->second.minPartners << ','
       << "'max_daily_post':" << it->second.maxDailyPost << ','
       << "'daily_posted':" << it->second.dailyPosted << ','
       << "'partner_count':" << it->second.partnerCount
       << "}";
  }
  ss << "]";

  // MCE_DEBUG("response : " << ss.str());
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool UpdateRequest::response() {
  // /hot/update.html?id=1000004323000300000&k=id&v=1000004323000300000
  if (!Request::response() && false)
    return false;

  Statement sql;

  // TODO : 有 sql 注入的危险.
  sql << "UPDATE ad_hot_promotion SET " << getProperty("k") << "=" << mysqlpp::quote << getProperty("v") << " WHERE id=" << getProperty("id");

  mysqlpp::StoreQueryResult ret;
  try {
    ret = QueryRunner("ad_db", CDbWServer).store(sql);
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
  // /hot/save.html?id=1&n=test&t=1&o=2&g=3&cam=4&b=5&p=1&t=t&ct=test%20content&img=image%20url&dst=dest%20url
  //
  if (!Request::response() && false)
    return false;

  ParseKeyValue(_post_body.c_str(), "&", '=', _props);

  Statement sql;
  sql << "INSERT INTO ad_hot_promotion (id, level, start_time, end_time, min_partners, max_daily_post, pic_url, pic_src, "\
         "message1, message2, join_button, join_title, not_join_title, related_posts)"
      << " VALUES (" << getProperty("id") << "," << getProperty("level") << "," << mysqlpp::quote << getProperty("start_time") << ","
      << mysqlpp::quote << getProperty("end_time") << "," << getProperty("min_partners") << "," << getProperty("max_daily_post") << ","
      << mysqlpp::quote << getProperty("pic_url") << "," << mysqlpp::quote << getProperty("pic_src") << "," 
      << mysqlpp::quote << getProperty("message1") << "," << mysqlpp::quote << getProperty("message2") 
      << "," << mysqlpp::quote << getProperty("join_button") << "," << mysqlpp::quote 
      << getProperty("join_title") << "," << mysqlpp::quote << getProperty("not_join_title") << ", '')";

  int ret_code = -1;
  string error_message;
  try {
    mysqlpp::SimpleResult res = QueryRunner("ad_db", CDbWServer).execute(sql);
    if (res) {
      ret_code = res.insert_id();
    }
  } catch (std::exception& e) {
    error_message = e.what();
    MCE_WARN("Store ad exception : " << e.what());
  } catch (...) {
    MCE_WARN("Store ad unkown exception.");
  }

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n[" << ret_code << ",\"" << JsonEscape(error_message) << "\"]";
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());

  return ret_code > 0;
}

}
}

