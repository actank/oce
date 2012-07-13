#ifndef _XCE_TIPS_ACTION_H_
#define _XCE_TIPS_ACTION_H_

#include "cwf/action.h"

namespace xce {
namespace tps {

const static int TYPE_SHOW = 1;
const static int TYPE_CLICK = 2;
const static int TYPE_CLOSE = 3;

static const std::string TipsDefaultContentType("text/html; charset=utf-8");

class ShowAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/show");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~ShowAction() { }
};

class ClickAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/click");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~ClickAction() { }
};

class CloseAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/close");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~CloseAction() { }
};

/*
class XXXAction : public cwf::BaseAction {
public:
	XXXAction():
		work_thrdp_(1,1) {
	}
	bool Match(const std::string& url) const {
		return boost::starts_with(url, "/xxx");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~HuoDongAction() { }
private:
        xce::ThreadPool work_thrdp_;
};
*/


}
}

#endif
