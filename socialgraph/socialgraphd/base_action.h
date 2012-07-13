#ifndef XCE_SOCIALGRAPH_BASE_ACTION_H__
#define XCE_SOCIALGRAPH_BASE_ACTION_H__

#include <boost/thread.hpp>

#include "cwf/action.h"
#include "socialgraph/socialgraphutil/zkclient.h"

namespace xce {
namespace socialgraph {

struct Reconnect {
	std::string desc_;
	int type_;
  int fetch_count_; //取多少个
};

typedef std::vector<Reconnect> ReconnectSeq;
typedef std::list<Reconnect> ReconnectList;

// 所有具体的action的基类
// 做了检票
// fastCGI请求, 只支持http get and post方法
class BaseAction : public cwf::BaseAction, public xce::Observer {
public:
	BaseAction();

	virtual ~BaseAction() {
		xce::ZooConfig::GetInstance().Detach(this);
	}
	
  virtual bool Match(const std::string& url) const {
    return false;
  }

	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);

  virtual cwf::HttpStatusCode DoGet(cwf::Request*, cwf::Response*, cwf::User*) {
    return cwf::HC_OK;
  }

  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*) {
    return cwf::HC_OK;
  }

	// implements from xce::Observer interface
	virtual void Update(const std::string& key,const std::string& value);


protected:
	void Init();
	void SetHeader(cwf::Response*);
	void RefreshReconnect(const std::string&);
	cwf::User* CheckTicket(cwf::Request*);
  ReconnectList CurrentReconnects();
  
private:
  ReconnectList current_rts_;
  boost::shared_mutex rw_mutex_;
}; 

//----------------------------

class Base2Action : public cwf::BaseAction {
 public:
  Base2Action();
  virtual ~Base2Action();

	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);

  virtual cwf::HttpStatusCode DoGet(cwf::Request*, cwf::Response*, cwf::User*) {
    return cwf::HC_OK;
  }
  virtual cwf::HttpStatusCode DoPost(cwf::Request*, cwf::Response*, cwf::User*) {
    return cwf::HC_OK;
  }
 protected:
	void SetHeader(cwf::Response*);
	cwf::User* CheckTicket(cwf::Request*);
};


}}
#endif
