#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include "Account.h"
#include "ServiceI.h"
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "QueryRunner.h"


namespace mop
{
namespace hi
{
namespace svc
{
namespace model
{
using namespace MyUtil;

const static string FB_ACCOUNT = "fb_account";
const static string MANAGER = "M"; 

class MoneyResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	MoneyResultHandler(int& ret, const std::string& field)
		: _ret(ret), _field(field) {
	}
	virtual bool handle(mysqlpp::Row& res) const;
private:
	int& _ret;
	std::string _field;
};

//----------------------------------------------------------------------------
class AccountManagerI : virtual public AccountManager
{
public:	
   virtual void inc(const DepositPtr& dep, const  Ice::Current& = Ice::Current());
   virtual void dec(const ExpenditurePtr& exp, const Ice::Current& = Ice::Current());
   virtual AccountPtr getAccount(Ice::Int id, const Ice::Current& = Ice::Current());   
};


};
};
};
};
#endif

