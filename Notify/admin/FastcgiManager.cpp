#include "FastcgiManager.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

#include "QueryRunner.h"
#include "IceLogger.h"

#include "NotifyGateAdapter.h"
#include "NotifyDispatcherAdapter.h"
#include "NotifyDispatcherNAdapter.h"
#include "NotifyIndexAdapter.h"
using namespace xce::notify;

const char * TPL_SEPERATOR = "__tpl_seperator_xyz_";

class AdminRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if(path)
    {
      if(strcmp(path, "/addschema.tpl") == 0)
        return new AddSchema(r);
      if(strcmp(path, "/delschema.tpl") == 0)
        return new DeleteSchema(r);
      if(strcmp(path, "/getschema.tpl") == 0)
        return new GetSchema(r);
      if(strcmp(path, "/setschemadisp.tpl") == 0)
        return new SetSchemaDisplay(r);
      if(strcmp(path, "/setschematype.tpl") == 0)
        return new SetSchemaType(r);
      if(strcmp(path, "/setschema-home-visible.tpl") == 0)
        return new SetSchemaHomeVisible(r);
      if(strcmp(path, "/getallschema.tpl") == 0)
        return new GetSchemaList(r);
      if(strcmp(path, "/apply.tpl") == 0)
        return new ApplyTemplate(r);
      if(strcmp(path, "/addtpl.tpl") == 0)
        return new AddTemplate(r);
      if(strcmp(path, "/gettpl.tpl") == 0)
        return new GetTemplateJson(r);
      if(strcmp(path, "/gettplstr.tpl") == 0)
        return new GetTemplateStr(r);
      if(strcmp(path, "/deltpl.tpl") == 0)
        return new DeleteTemplate(r);

      if(strcmp(path, "/setschemamerge.tpl") == 0)
        return new SetSchemaMerge(r);
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  // 启动 ice 服务
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&NotifyAdminI::instance(),
  			  service.createIdentity("M", ""));

  // 启动 fcgi 服务
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);
#if 0
	if(FCGX_Init() != 0){
		MCE_WARN("FCGX_Init err");
		return;
	}
	int listen_socket = FCGX_OpenSocket(fcgi_socket.c_str(), 100);
	if(listen_socket < 0){
		MCE_WARN("FCGX_OpenSocket err");
		return;
	}

  for(int i = 0; i < 4; ++i)
  {
    FcgiTask * task = new FcgiTask(new AdminRequestFactory());
    if(task->init(listen_socket)) {
      TaskManager::instance().execute(task);
    } else {
      delete task;
      MCE_WARN("FCGX_InitRequest err " << i);
    }
  }
#else
  // fcgi_server memory managed as RequestFactory ...
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 512);
  fcgi_server->RegisterRequestFactory(new AdminRequestFactory());
  fcgi_server->Start();
#endif
}

NotifyAdminI::NotifyAdminI() {
} 

bool ApplyTemplate::response(){
  if(!Request::response())
    return false;

  string out = "Content-type: text/html\r\n\r\n";

  bool ret = false;
	try{
		ret = NotifyGateAdapter::instance().reloadSchema();
		ret = NotifyDispatcherAdapter::instance().reloadSchema();
		ret = NotifyDispatcherNAdapter::instance().reloadSchema();
		ret = NotifyIndexAdapter::instance().reloadSchema();
	} catch (Ice::Exception& e){
    MCE_WARN("NotifyGateAdapter.reloadSchema exception : " << e.what());
    ret = false;
	}

  if (ret) {
    out += "0";
  } else {
    out += "-1";
    MCE_WARN("NotifyGateAdapter.reloadSchema return false");
  }

  FCGX_FPrintF(_fcgi_out, out.c_str());
  return ret;
}

bool GetSchemaList::response(){
  if(!Request::response())
    return false;

  //
	// getallschema.tpl?
  //
	MCE_DEBUG("GetSchemaList::response");
	Statement sql;
	sql << "select id, type, name, display, home_visible,merge from data_schemas";
	stringstream ss;
	try {
	  ss << "Content-type: text/html\r\n\r\n[";
		mysqlpp::StoreQueryResult res = QueryRunner("notify", CDbWServer).store(sql);
		if (!res) {
	    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-3");
			return false;
		}
		for (size_t i = 0; i<res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			if (i) {
				ss << ",";
			}

      string name = row["name"].c_str();
			jsonEscape(name);
			ss << "{'id':" << (int)row["id"] 
				 << ",'type':" << (int)row["type"] 
				 << ", 'name':'" << name 
				 << "','display':" << (int)row["display"] 
				 << ",'home_visible':" << (int)row["home_visible"]
				 << ",'merge':" << (int)row["merge"]
				 << "}";
		}
	  ss << "]";
	} catch (std::exception& e) {
		MCE_WARN("GetSchemaList err : " << e.what());
	  FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
    return false;
	} catch (...) {
		MCE_WARN("GetSchemaList unkown exception.");
	  FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-2");
    return false;
	}
	MCE_DEBUG("GetSchemaList::response --> " << ss.str());
	FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}

bool AddTemplate::response(){
	//
	// addtpl?schemaid=123&view=1&template=tpl_content
	//
  if(!Request::response())
    return false;
	Statement sql;
	sql<<"insert into template (schema_id,view,template) values ("
	   << getProperty("schemaid") << "," << getProperty("view") << ","
	   << mysqlpp::quote << UriDecode(getProperty("post_body"))
     << ") on duplicate key update template = VALUES(template), update_time = NOW()";

  bool ret = true;
	try {
		mysqlpp::SimpleResult res = QueryRunner("notify", CDbWServer).execute(sql);
		if (!res) {
		  MCE_WARN("AddTemplate err : res null ");
      ret = false;
		}
	} catch (std::exception& e) {
		MCE_WARN("AddTemplate exception : " << e.what());
    ret = false;
	} catch (...) {
		MCE_WARN("AddTemplate unknown exception.");
    ret = false;
	}

  if(ret)
	  FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n0");
  else
	  FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n-1");
  return ret;
}

bool DeleteSchema::response() {
  //
  // delschema.tpl?id=123
  //
	Statement sql;
	sql << "delete from data_schemas where id = " << getProperty("id");

	mysqlpp::StoreQueryResult ret;
	try {
		ret = QueryRunner("notify", CDbWServer).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("DeleteTemplate err : " << e.what());
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	} catch (...) {
		MCE_WARN("DeleteTemplate unkown exception.");
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-2" );
    return false;
	}

	FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n0" );
  return true;
}

bool SetSchemaType::response() {
  //
  // /setschematype.tpl?id=15&type=15
  //

	Statement sql;
  Ice::Int type = getPropertyInt("type");
  if(type < 0)
  {
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-3" );
    return false;
  }

	sql << "update data_schemas set type = " << type << " where id = " << getProperty("id");

	mysqlpp::StoreQueryResult ret;
	try {
		ret = QueryRunner("notify", CDbWServer).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("SetSchemaType err : " << e.what());
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	} catch (...) {
		MCE_WARN("SetSchemaType unkown exception.");
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-2" );
    return false;
	}

	stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << type;
	FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;

}

bool SetSchemaHomeVisible::response() {
  //
	// /setschema-home-visible.tpl?id=12&visible=1
  //
	Statement sql;
  int visible = getPropertyInt("visible");
  visible = visible > 0 ? 1 : 0;
	sql << "update data_schemas set home_visible = " << visible << " where id = " << getProperty("id");

	mysqlpp::StoreQueryResult ret;
	try {
		ret = QueryRunner("notify", CDbWServer).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("DeleteTemplate err : " << e.what());
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	} catch (...) {
		MCE_WARN("DeleteTemplate unkown exception.");
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	}

	stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << visible;
	FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}

bool SetSchemaDisplay::response() {
  //
  // /setschemadisp.tpl?id=123&display=0
  //
	Statement sql;
  int disp = getPropertyInt("display");
  disp = disp > 0 ? 1 : 0;
	sql << "update data_schemas set display = " << disp << " where id = " << getProperty("id");

	mysqlpp::StoreQueryResult ret;
	try {
		ret = QueryRunner("notify", CDbWServer).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("DeleteTemplate err : " << e.what());
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	} catch (...) {
		MCE_WARN("DeleteTemplate unkown exception.");
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	}

	stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << disp;
	FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}

bool DeleteTemplate::response() {
  //
  // deltpl.tpl?schemaid=123&view=0
  //
	Statement sql;
	sql << "delete from template where schema_id = " << getProperty("schemaid") 
      << " and view = " << getProperty("view");

	mysqlpp::StoreQueryResult ret;
	try {
		ret = QueryRunner("notify", CDbWServer).store(sql);
	} catch (std::exception& e) {
		MCE_WARN("DeleteTemplate err : " << e.what());
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	} catch (...) {
		MCE_WARN("DeleteTemplate unkown exception.");
	  FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
	}

	FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n0" );
  return true;
}

bool AddSchema::response(){
  //
	// addschema.tpl?keys="+keys+"&name="+name+"&id="+id;
  //
  if(!Request::response())
    return false;
	MCE_INFO("AddSchema::response");
	Statement sql;
	string id = getProperty("id");
  string post_body = UriDecode(getProperty("post_body"));
	if(id == "-1"){
		sql << "insert into data_schemas (key_list, name) values ("
		    << mysqlpp::quote << post_body << ", "
		    << mysqlpp::quote << getProperty("name") << ") ";
	} else {
		sql << "insert into data_schemas (id, key_list, name) values ("
		    << mysqlpp::quote << id <<","
		    << mysqlpp::quote << post_body << ", "
		    << mysqlpp::quote << getProperty("name") << ") "
		    <<" ON DUPLICATE KEY UPDATE key_list=VALUES(key_list), name=VALUES(name)";
	}

  int ret_code = -1;
	try {
		mysqlpp::SimpleResult res = QueryRunner("notify", CDbWServer).execute(sql);
		if (res) {
      ret_code = res.insert_id();
    }
	} catch (std::exception& e) {
		MCE_WARN("AddSchema exception : " << e.what());
	} catch (...) {
		MCE_WARN("AddSchema unkown exception.");
	}

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << ret_code;
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());

  return ret_code > 0;
}


bool GetTemplateStr::response(){
  //
	// gettplstr.tpl?schemaid=123
  //
	MCE_INFO("GetTemplateStr::response");
	Statement sql;
	sql << "select view, template from template where schema_id = " << getProperty("schemaid");
  string ss = "Content-type: text/html\r\n\r\n";
	try {
		mysqlpp::StoreQueryResult res = QueryRunner("notify", CDbWServer).store(sql);
		if (!res) {
		  MCE_WARN("load template retun null");
			FCGX_FPrintF(_fcgi_out, ss.c_str());
			return false;
		}
		for (size_t i = 0; i<res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			if (i) {
				ss += TPL_SEPERATOR;
			}

			ss += row["view"].c_str();
		  ss += ',';
			ss += row["template"].c_str();
		}
	} catch (std::exception& e) {
		MCE_WARN("load template err, " << e.what());
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n[]");
		return false;
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n[]");
		return false;
	}

  MCE_DEBUG("GetTemplateStr::response --> " << ss);
  FCGX_FPrintF(_fcgi_out, ss.c_str());
  return true;
}

bool GetTemplateJson::response(){
  //
	// gettpl.tpl?schemaid=123
  //
	MCE_INFO("GetTemplateJson::response");
	Statement sql;
	sql << "select view, template from template where schema_id = " << getProperty("schemaid");
  stringstream ss;
	try {
		mysqlpp::StoreQueryResult res = QueryRunner("notify", CDbWServer).store(sql);
		if (!res) {
		  MCE_WARN("load template retun null");
			FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n[]");
			return false;
		}
		ss << "Content-type: text/html\r\n\r\n[";
		for (size_t i = 0; i<res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			if (i) {
				ss << ",";
			}
			string tpl = row["template"].c_str();
			jsonEscape(tpl);

			ss << "{'view':'" << row["view"].c_str() << "','template':'" << tpl << "'}";
		}
		ss << "]";
	} catch (std::exception& e) {
		MCE_WARN("load template err, " << e.what());
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n[]");
		return false;
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
		FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\n[]");
		return false;
	}

  MCE_DEBUG("GetTemplateJson::response --> " << ss.str());
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}

bool GetSchema::response(){
	MCE_INFO("GetSchema::response");
  if(!Request::response())
    return false;

	Statement sql;

  stringstream ss;
	sql << "select id, name, key_list from data_schemas where id = " << getProperty("id");
	try {
		mysqlpp::StoreQueryResult res = QueryRunner("notify", CDbWServer).store(sql);
		if (!res) {
			return false;
		}

    if(res.num_rows() != 1)
    {
		  ss << "Content-type: text/html\r\n\r\n[]";
    }
    else
    {
      mysqlpp::Row row = res.at(0);
      ss << "Content-type: text/html\r\n\r\n[" << "'" << row["id"].c_str() 
         << "','" << row["name"].c_str() << "','" << row["key_list"].c_str() 
         <<"']";
    }

		MCE_DEBUG("GetSchema::response : " << ss.str());
		FCGX_FPrintF(_fcgi_out, ss.str().c_str());
		return true;
	} catch (std::exception& e) {
		MCE_WARN("GetSchema::response --> select data_schemas err, " << e.what());
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	return false;
}


bool SetSchemaMerge::response() {
  //
  // /setschemadisp.tpl?id=123&display=0
  //
  Statement sql;
  int merge = getPropertyInt("merge");
  merge = merge > 0 ? 1 : 0;
  sql << "update data_schemas set merge = " << merge << " where id = " << getProperty("id");

  mysqlpp::StoreQueryResult ret;
  try {
    ret = QueryRunner("notify", CDbWServer).store(sql);
  } catch (std::exception& e) {
    MCE_WARN("SetSchemaMerge err : " << e.what());
    FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
  } catch (...) {
    MCE_WARN("SetSchemaMerge unkown exception.");
    FCGX_FPrintF(_fcgi_out,"Content-type: text/html\r\n\r\n-1" );
    return false;
  }

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n" << merge;
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}
