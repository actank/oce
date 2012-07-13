#include "socialgraph/socialgraphd/base_action.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr.hpp>

#include "base/logging.h"

#include "arch/site_xiaonei.h"

namespace xce {
namespace socialgraph {

const static std::string kZNodeReconnect = "/RECONNECT"; 

BaseAction::BaseAction() {
	xce::ZooConfig::GetInstance().Attach(this);
	Init();
}

void BaseAction::Init() {
	std::string v;
	bool fetch = xce::ZooConfig::GetInstance().Get(kZNodeReconnect, &v);
	if (fetch) {
		const std::string value = v;
		Update(kZNodeReconnect, value);
	}
	else {
		LOG(LERROR) << "get znode:" << kZNodeReconnect << " from zk failed!"; 
		sleep(1);
		LOG(LERROR) << "init again!";
		Init();	
	}

}

cwf::User* BaseAction::CheckTicket(cwf::Request* request) {
	return xce::Authorize(request);
}

cwf::HttpStatusCode BaseAction::Process(cwf::Request* request, cwf::Response* response) {
	boost::scoped_ptr<cwf::User> host(CheckTicket(request));
	if (!host) {
		LOG(LERROR) << "check ticket failed!";
		return cwf::HC_UNAUTHORIZED;
	}

	SetHeader(response);
	cwf::HttpVerb hv = request->method();
	
	switch (hv) {
		case cwf::HV_GET:
			return DoGet(request, response, host.get());
		case cwf::HV_POST:
			return DoPost(request, response, host.get());
		default:
			return cwf::HC_OK; // return HC_METHOD_NOT_ALLOWED better?
	}
}

void BaseAction::SetHeader(cwf::Response* res) {
	static const std::string kDefaultContentType("text/html; charset=utf-8");
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();
}

void BaseAction::Update(const std::string& key,const std::string& value) {
	if (key == kZNodeReconnect) {
		LOG(INFO) << "BaseAction::Update  key:" << key << " value:" << value;
		RefreshReconnect(value);
	}	
}

void BaseAction::RefreshReconnect(const std::string& new_value) {
  ReconnectList list;
	std::vector<std::string> units;		
	boost::split(units, new_value, boost::is_any_of("#"));
	bool ec = false;
	for (std::vector<std::string>::iterator iter = units.begin(); iter != units.end(); ++iter) {
		std::vector<std::string> line;
		boost::split(line, *iter, boost::is_any_of(","));
		try {
			Reconnect rt;
			rt.desc_ = line[0];
			rt.type_ = boost::lexical_cast<int>(line[1]);
      rt.fetch_count_ = boost::lexical_cast<int>(line[2]);
      list.push_back(rt);
		}	catch (...) { 
			ec = true;
			break;
		}
	}
	if (!ec) {
    boost::unique_lock<boost::shared_mutex> write_lock(rw_mutex_);
    current_rts_.swap(list);
	}
}	

ReconnectList BaseAction::CurrentReconnects() {
  boost::shared_lock<boost::shared_mutex> read_lock(rw_mutex_);
  ReconnectList list =  current_rts_;
  return list;
}


//------------------------------
Base2Action::Base2Action() {

}

Base2Action::~Base2Action() {

}

cwf::HttpStatusCode Base2Action::Process(cwf::Request* request, cwf::Response* response) {
	boost::scoped_ptr<cwf::User> host(CheckTicket(request));
	if (!host) {
		LOG(LERROR) << "check ticket failed!";
		return cwf::HC_UNAUTHORIZED;
	}

	SetHeader(response);
	cwf::HttpVerb hv = request->method();
	
	switch (hv) {
		case cwf::HV_GET:
			return DoGet(request, response, host.get());
		case cwf::HV_POST:
			return DoPost(request, response, host.get());
		default:
			return cwf::HC_OK; // return HC_METHOD_NOT_ALLOWED better?
	}
}

void Base2Action::SetHeader(cwf::Response* res) {
	static const std::string kDefaultContentType("text/html; charset=utf-8");
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();
}

cwf::User* Base2Action::CheckTicket(cwf::Request* request) {
	return xce::Authorize(request);
}


}} // end xce::socialgraph


