#include "AccountI.h"

using namespace com::xiaonei::xce;
using namespace mop::hi::svc::model;
using namespace MyUtil;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	// Add manager object and ServantFactory
	service.getAdapter()->add(new AccountManagerI, service.createIdentity(MANAGER, ""));
}

bool MoneyResultHandler::handle(mysqlpp::Row& row) const {
    _ret = (int) row[_field.c_str()];
	return false;
}

//--------------------------------------------------------------------------------
void AccountManagerI::inc(const DepositPtr& dep, const Ice::Current& current) {
	MCE_INFO("inc start " << dep->uid << " rmb=" << dep->rmb << " xnb=" << dep->xnb << " time=" << dep->timestamp );
	AccountPtr ptr = getAccount(dep->uid);

	Statement sql;
	sql << "call save_money ("<<dep->fid<<"," << dep->uid <<"," << dep->rmb
			<<"," << dep->xnb <<",'" << dep->timestamp<<"')";
	try {
		QueryRunner("account", CDbWServer).execute(sql);
	} catch(...) {
		MCE_WARN("do save_money procedure err");
		throw RollbackException();
	}
	//AccountPtr ptr = findAccount(dep->uid);
	ptr->xnb += dep->xnb;

	MCE_INFO("inc end " << dep->uid << " rmb=" << dep->rmb << " xnb=" << dep->xnb << " time=" << dep->timestamp );
}

void AccountManagerI::dec(const ExpenditurePtr& exp, const Ice::Current& current) {
	MCE_INFO("dec start " << exp->uid << " xnb=" << exp->xnb << " time=" << exp->timestamp << " productName=" << exp->productName << " productType=" << exp->productType );
	AccountPtr ptr = getAccount(exp->uid);

	if ((ptr->xnb - exp->xnb < 0) ||(exp->xnb < 0)) {
		MCE_WARN("dec expected exception ptr->xnb=" << ptr->xnb << " exp->xnb=" << exp->xnb );
		throw RollbackException();
	}
	Statement sql;
	sql << "SELECT fun_cost_money (" << exp->uid <<"," << exp->xnb
			<<",now(),'" << exp->description <<"','"
			<< exp->productName <<"','" << exp->productType <<"') AS ret";
	int ret = -1;
	try {
		QueryRunner("account", CDbWServer).query(sql, MoneyResultHandler(ret, "ret"));
	} catch(...) {
		MCE_WARN("do cost_money procedure err");
		throw RollbackException();
	}
	if (ret < 0){
		throw RollbackException();
	}
	MCE_INFO("dec end " << exp->uid << " xnb=" << exp->xnb << " time=" << exp->timestamp << " productName=" << exp->productName << " productType=" << exp->productType );
}

AccountPtr AccountManagerI::getAccount(Ice::Int id, const Ice::Current& current) {
	MCE_DEBUG("Loading " << id);
	int xnb = 0;
	Statement sql;
	sql << "SELECT coinnum FROM account WHERE userid='" << id << "'";
	QueryRunner("account", CDbRServer).query(sql, MoneyResultHandler(xnb, "coinnum"));
	AccountPtr res = new Account();
	res->xnb = xnb;
	MCE_DEBUG( "Loading " << id << " done.");
	return res;
	//return ServiceI::instance().locateObject<AccountPtr>(ACCOUNT, id);
}	

