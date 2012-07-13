#ifndef FASTCGI_MANAGER_H_
#define FASTCGI_MANAGER_H_

#include <fcgi_stdio.h>
#include <Ice/Ice.h>

#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"
#include "AdUnion.h"
#include "QueryRunner.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::notify;

namespace xce {
namespace ad {

class ReloadRequest : public Request {
public:
	ReloadRequest(FCGX_Request* r) : Request(r) {
	}

	virtual bool response();
};

class GetAllRequest : public Request {
public:
	GetAllRequest(FCGX_Request* r)
		: Request(r) {
	}

	virtual bool response();
};

class UpdateRequest : public Request {
public:
	UpdateRequest(FCGX_Request* r) : Request(r) {
	}

	virtual bool response();
};

class SaveRequest : public Request {
public:
	SaveRequest(FCGX_Request* r) : Request(r) {
	}

	virtual bool response();
};

class GetOneRequest : public Request {
public:
	GetOneRequest(FCGX_Request* r) : Request(r) {
	}

	virtual bool response();
};

class AdUnionAdminI : public xce::ad::AdUnionAdmin, public MyUtil::Singleton<AdUnionAdminI> {
public:
  AdUnionAdminI();
  virtual bool Reload(const Ice::Current& = Ice::Current());
};

}
}

#endif
