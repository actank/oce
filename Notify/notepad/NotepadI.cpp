#include "NotepadI.h"

#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "Notify/util/json_escape.h"
#include "QueryRunner.h"

using namespace std;
using namespace com::xiaonei::talk::notepad;
using namespace xce::notify;
using namespace com::xiaonei::xce;

static const char SOURCE_SEPERATOR  = '-';
static const char * TABLE_NOTE_ID_SEQ = "note_id_seq";
const int ID_SEQ_STEP = 100;
static const char * DB_INSTANCE = "notify";

const int kMaxLimit = 50;

class GateRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if(path)
    {
      if(strcmp(path, "/add.note") == 0) {
        return new AddRequest(r);
      }
      if(strcmp(path, "/remove.note") == 0) {
        return new RemoveRequest(r);
      }
      if(strcmp(path, "/get.note") == 0) {
        return new GetRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&NotepadManagerI::instance(), service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:10000");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  // fcgi_server memory managed as RequestFactory ...
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 128);
  fcgi_server->RegisterRequestFactory(new GateRequestFactory());
  fcgi_server->Start();
}

NotepadManagerI::NotepadManagerI() 
  : cur_index_(-1)
  , top_index_(-1)
  , count_(0) {
}

Ice::Long NotepadManagerI::CreateNoteId() {
	{
		IceUtil::Mutex::Lock lock(mutex_);
		if (++count_ > 1000) {
			MCE_WARN("DbHelper::createId --> IdSeqDb is blocked");
			--count_;
			return -1;
		}
	}
	{
		IceUtil::Mutex::Lock lock(mutex_);
		while (1) {
			if (cur_index_ != -1 && cur_index_ < top_index_) {
				--count_;
				return cur_index_++;
			}

			{
				Statement sql;
				sql << "UPDATE " << TABLE_NOTE_ID_SEQ << " SET id = id +"
						<< ID_SEQ_STEP;
				QueryRunner(DB_INSTANCE, CDbWServer, TABLE_NOTE_ID_SEQ).store(
						sql);
			}

			{
				Statement sql;
				sql << "SELECT id FROM " << TABLE_NOTE_ID_SEQ;
				mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE,
						CDbRServer, TABLE_NOTE_ID_SEQ).store(sql);
				if (!res || res.num_rows() <= 0) {
					MCE_WARN("create id error:" << top_index_);
					--count_;
					return -1;
				}
				for (size_t i = 0; i < res.num_rows(); ++i) {
					mysqlpp::Row row = res.at(i);
					top_index_ = (Ice::Long)row["id"];
					cur_index_ = top_index_ - ID_SEQ_STEP;
					MCE_DEBUG("topIndex:" << top_index_);
					break;
				}
			}
		}
	}
}

void AddNoteTask::handle() {
  MCE_INFO("AddNoteTask : id=" << id_ << " owner=" << owner_);
  if (check_owner_) {
    // 覆盖之前, 要确保用户覆盖的是自己的note
    mysqlpp::StoreQueryResult res;
    try {
      Statement sql;
      sql << "SELECT owner FROM notepad WHERE id=" << id_;

      res = QueryRunner("notify", CDbRServer,
          "notepad").store(sql);
    } catch (std::exception& e) {
      MCE_WARN("AddNoteTask get owner error. uid=" << owner_ << " " << e.what());
    } catch (...) {
      MCE_WARN("AddNoteTask get owner unkown exception");
    }

    if (res && res.num_rows() > 0) {
      mysqlpp::Row row = res.at(0);

      if (owner_ != (Ice::Int)row["owner"]) {
        MCE_INFO("Illegal request. noteid = " << id_ << " db owner=" 
            << (Ice::Int)row["owner"] << " req owner=" << owner_);
        MCE_WARN("check owner error.");
        return;
      }
    }
  }

  MCE_INFO("add note --------- : " << id_);

  try {
    Statement sql;
    sql << "INSERT INTO notepad "
        << "(id, owner, content) VALUES (" << id_ << ", " 
        << owner_ << ", "
        << mysqlpp::quote << content_ 
        << ") ON DUPLICATE KEY UPDATE subject=" << mysqlpp::quote << subject_ << ", "
        << " content=" << mysqlpp::quote << content_ << ", updatetime=NOW()";

    mysqlpp::StoreQueryResult res = QueryRunner("notify", CDbWServer,
        "notepad").store(sql);
  } catch (std::exception& e) {
    MCE_WARN("AddNoteTask error. owner=" << owner_ << " " << e.what());
  } catch (...) {
    MCE_WARN("AddNoteTask unkown exception");
  }
}

void RemoveNoteTask::handle() {
  MCE_INFO("RemoveNoteTask : id=" << id_ << " owner=" << owner_);

  try {
    Statement sql;
    sql << "DELETE FROM notepad WHERE "
        << "id = " << id_ << " AND owner = " << owner_;

    mysqlpp::StoreQueryResult res = QueryRunner(
        "notify", CDbWServer,
        "notepad").store(sql);
  } catch (std::exception& e) {
    MCE_WARN("AddNoteTask error. owner=" << owner_ << " " << e.what());
  } catch (...) {
    MCE_WARN("AddNoteTask unkown exception. owner=" << owner_);
  }
}

bool RemoveRequest::response(){
  //
  // remove.note?id=1234&owner=240599893
  //
  if(!_fcgi_out) {
    MCE_WARN("remove notify fcgi_out null");
    return false;
  }

  if(!Request::response()) {
    MCE_DEBUG("user auth error");
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
  }

  Ice::Long id = getPropertyLong("id");
  MCE_INFO("user " << _login_uid << " remove node.id=" << id);

  if(id <= 0) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html; charset=utf-8\r\n\r\n-2");
    return false;
  }

	TaskManager::instance().execute(new RemoveNoteTask(id, _login_uid));

  FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n0");
  return true;
}

bool AddRequest::response() {
  //
  // add.note?id=0&owner=1234
  //    title & content, in POST BODY
  //
  if(!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }

  if(!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
  }


  Ice::Long nid = getPropertyLong("id");
  //Ice::Int owner = getPropertyInt("owner");
  string post_body = getProperty("post_body");

  if (post_body.empty()) {
    MCE_DEBUG("add.note post_body empty");
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-2");
    return false;
  }

  map<string, string> subject_map;
  std::vector<std::string> strs;
  boost::split(strs, post_body, boost::is_any_of("&"), boost::token_compress_on);
  for(size_t i = 0; i < strs.size(); ++i) {
    size_t pos = strs[i].find('=');
    if(pos != string::npos) {
      subject_map.insert(make_pair(strs[i].substr(0, pos), UriDecode(strs[i].substr(pos + 1))));
    }
  }

  // 不要进行trim
  string subject = UriDecode(subject_map["subject"]);
  string content = UriDecode(subject_map["content"]);

  MCE_DEBUG("subject : " << subject);
  MCE_DEBUG("content : " << content);

  bool check_owner = true;

  if (nid <= 0) {
    nid = NotepadManagerI::instance().CreateNoteId();
    check_owner = false;
  }

  MCE_INFO("user " << _login_uid << " add note.id=" << nid);

  MCE_DEBUG("create insert task : check_owner = " << check_owner);
	TaskManager::instance().execute(new AddNoteTask(nid, _login_uid, check_owner, subject, content));

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << nid;
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}

bool GetRequest::response() {
  //
  // get.note?id=0&owner=1234
  // id = 0, 表示取最新10条; id > 0, 表示取小于该id的10条
  //

  if(!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }

  if(!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
  }

  mysqlpp::StoreQueryResult res;
  try {
    Statement sql;
    sql << "SELECT id, owner, subject, content, UNIX_TIMESTAMP(updatetime) AS t FROM notepad WHERE owner=" 
        << _login_uid << " ORDER BY id DESC LIMIT " << kMaxLimit;

    res = QueryRunner("notify", CDbRServer,
        "notepad").store(sql);
  } catch (std::exception& e) {
    MCE_WARN("get error. uid=" << _login_uid << " " << e.what());
  } catch (...) {
    MCE_WARN("get.note unkown exception. uid=" << _login_uid);
  }

  if (!res) {
    return false;
  }

  stringstream ss;
  ss << "Content-type: text/html; charset=utf-8\r\n\r\n[";
  for ( size_t i = 0;  i < res.num_rows(); ++i) {
    mysqlpp::Row row = res.at(i);
    if (i != 0) {
      ss << ',';
    }
    ss << "{\"id\":" << (Ice::Long)row["id"]
       << ", \"owner\":" << (Ice::Int)row["owner"]
       << ", \"updatetime\":" << (Ice::Int)row["t"]
       //<< ", \"subject\":\"" << row["subject"].c_str()
       << ", \"content\":\"" << JsonEscape(row["content"].c_str())
       << "\"}";
  }
  ss << "]";
  MCE_INFO("user " << _login_uid << " count=" << res.num_rows());

  MCE_DEBUG("get notepad data : " << ss.str());

  FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());

  return true;
}

