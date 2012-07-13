#ifndef PASSPORTADAPTER_H_
#define PASSPORTADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "Passport.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace mop::hi::svc::passport;
using namespace MyUtil;

class PassportAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::OceChannel, PassportAdapter> {
public:
	PassportAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}
	string createTicket(int userId);
	int verifyTicket(const string& ticket);
	string queryTicket(int userId);
protected:
	virtual void initialize();
	virtual string name() {
		return "Passport";
	}
	virtual string endpoints() {
		return "@Passport";
	}
	virtual size_t cluster() {
		return 10;
	}
//	PassportManagerPrx _manager;
	vector<PassportManagerPrx> _managers;

};
}
;
}
;
}
;
}
;

#endif /*PASSPORTADAPTER_H_*/
