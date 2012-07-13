#ifndef FASTCGI_MANAGER_H_
#define FASTCGI_MANAGER_H_

#include "Notify.h"

#include <fcgi_stdio.h>

#include "Notify/util/fcgi_request.h"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;

class ApplyTemplate : public Request {
public:
	ApplyTemplate(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};


class AddTemplate : public Request {
public:
	AddTemplate(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class GetTemplateStr : public Request {
public:
	GetTemplateStr(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};


class GetTemplateJson : public Request {
public:
	GetTemplateJson(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class DeleteTemplate : public Request {
public:
	DeleteTemplate(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class AddSchema : public Request {
public:
	AddSchema(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class SetSchemaHomeVisible : public Request {
public:
	SetSchemaHomeVisible(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class SetSchemaDisplay : public Request {
public:
	SetSchemaDisplay(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class SetSchemaType : public Request {
public:
	SetSchemaType(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class DeleteSchema : public Request {
public:
	DeleteSchema(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class GetSchema : public Request {
public:
	GetSchema(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class GetSchemaList : public Request {
public:
	GetSchemaList(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class NotifyAdminI : public xce::notify::NotifyAdmin,
	public MyUtil::Singleton<NotifyAdminI> {
public:
  NotifyAdminI();
};

class SetSchemaMerge : public Request {
public:
  SetSchemaMerge(FCGX_Request* r)
    : Request(r) {
  }

  virtual bool response();
};

}
}

#endif
