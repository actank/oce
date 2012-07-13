/*
 * =====================================================================================
 *
 *       Filename:  UserPassportAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时02分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#ifndef __USER_PASSPORT_ADAPTER_H__
#define __USER_PASSPORT_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <boost/lexical_cast.hpp>
#include "Markable.h"

namespace xce {
namespace adapter {
namespace userpassport {

using namespace xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;

class UserPassportInfo;
typedef IceUtil::Handle<UserPassportInfo> UserPassportInfoPtr;

class UserPassportInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	UserPassportInfoPtr parse(const UserPassportDataPtr& data) { id_ = data->uID; status_ = data->status; safestatus_ = data->safeStatus; account_ = data->account;
																	domain_ = data->domain; passwordmd5_ = data->passwordmd5; return this; }
	int id() const { return id_; }
	int status() const { return status_; }
	int safestatus() const { return safestatus_; }
	string account() const { return account_; }
	string domain() const { return domain_; }
	string passwordmd5() const { return passwordmd5_; }
	void set_id(int id) { id_ = id; mark(xce::userbase::CUPID, boost::lexical_cast<std::string>(id)); }
	void set_status(int status) { status_ = status; mark(xce::userbase::CUPSTATUS, boost::lexical_cast<std::string>(status_)); }
	void set_safestatus(int safestatus) { safestatus = safestatus_; mark(xce::userbase::CUPSAFESTATUS, boost::lexical_cast<std::string>(safestatus_)); }
	void set_account(const string& account) { account_ = account; mark(xce::userbase::CUPACCOUNT, account_); }
	void set_domain(const string& domain) { domain_ = domain; mark(xce::userbase::CUPDOMAIN, domain_); }
	void set_passwordmd5(const string& passwordmd5) { passwordmd5_ = passwordmd5; mark(xce::userbase::CUPPASSWORDMD5, passwordmd5_); }
private:
	int id_;
	int status_;
	int safestatus_;
	string account_;
	string domain_;
	string passwordmd5_;
};
class UserPassportAdapter : virtual public MyUtil::ReplicatedClusterAdapterI<UserPassportManagerPrx>,
	virtual public MyUtil::Singleton<UserPassportAdapter>
{
public:
	UserPassportAdapter() : MyUtil::ReplicatedClusterAdapterI<UserPassportManagerPrx>( "ControllerUserBase",120,300,new MyUtil::OceChannel,true,"UPM"){

	}
	void reload(int id, const Ice::Context& ctx = Ice::Context());
	UserPassportInfoPtr getUserPassport(int uid, const Ice::Context& ctx = Ice::Context());
	void setUserPassport(int userId, const UserPassportInfoPtr& ptr);
	void createUserPassport(int userId, const UserPassportInfoPtr& ptr);
	UserPassportManagerPrx getUserPassport10sManager(int userId);
	UserPassportManagerPrx getUserPassportManager(int userId);
	UserPassportManagerPrx getUserPassportManagerOneway(int userId);
//private:
		
	//UserPassportManagerPrx getUserPassport10sManager(int userId);
	//UserPassportManagerPrx getUserPassportManager(int userId);
	//UserPassportManagerPrx getUserPassportManagerOneway(int userId);

};

}
}
}

#endif

