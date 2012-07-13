#ifndef _XCE_ACTIVEMOTION_ACTION_H_
#define _XCE_ACTIVEMOTION_ACTION_H_

#include "cwf/action.h"
#include "json/json.h"
#include "worker.h"

namespace cwf {
namespace socialgraph {

const static int TYPE_CLICK = 1;
const static int TYPE_SCROLLBOTTOM = 2;
const static int TYPE_FOCUS = 3;
const static int TYPE_UNFOCUS = 4;
const static int TYPE_APPHOLD = 5;
const static int TYPE_OTHER = 6;

static const std::string DjDefaultContentType("text/html; charset=utf-8");

/*
static void InitWorker()
{
}

class WorkerThread {
public:
	WorkerThread():
		worker_(1,1) {
	}
	static WorkerThread& GetInstance(){
		static XCE_DECLARE_ONCE(once_guard_);
		static WorkerThread worker;
		xce::XceOnceInit(&once_guard_, &InitWorker);
		return worker;
	}
	void Post( WorkMessage* msg ){
		worker_.Post(msg);
	}
private:
        xce::ThreadPool worker_;
};
*/

class ClickAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/click");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~ClickAction() { }
};

class ScrollBottomAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/scrollbottom");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~ScrollBottomAction() { }
};

class FocusAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/focus");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~FocusAction() { }
};

class UnfocusAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/unfocus");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~UnfocusAction() { }
};

class AppHoldAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/apphold");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~AppHoldAction() { }
};

class OtherAction : public cwf::BaseAction {
public:
	bool Match(const std::string& url) const
	{
		return boost::starts_with(url, "/other");
	}
	virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
	virtual ~OtherAction() { }
};

}
}

#endif
