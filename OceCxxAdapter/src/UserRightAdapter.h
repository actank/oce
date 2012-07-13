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


#ifndef __USER_RIGHT_ADAPTER_H__
#define __USER_RIGHT_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Markable.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace adapter {
namespace userright {

using namespace xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;
class UserRightInfo;
typedef IceUtil::Handle<UserRightInfo> UserRightInfoPtr;

class UserRightInfo : virtual public Ice::Object, public MyUtil::Markable {
public:
	UserRightInfoPtr parse(const UserRightDataPtr& ptr) { id_ = ptr->uID; auth_ = ptr->auth; rights_ = ptr->rights; return this; }
	int id() const { return id_; }
	int auth() const { return auth_; }
	int rights() const { return rights_; }
	void set_id(int id) { id_ = id; mark(xce::userbase::CURID, boost::lexical_cast<std::string>(id)); }
	void set_auth(int auth) { auth_ = auth; mark(xce::userbase::CURAUTH, boost::lexical_cast<std::string>(auth)); }
	void set_rights(int rights) { rights_ = rights; mark(xce::userbase::CURCOMMONRIGHTS, boost::lexical_cast<std::string>(rights)); }
private:
	int id_;
	int auth_;
	int rights_;
};

class UserRightAdapter : public MyUtil::ReplicatedClusterAdapterI<UserRightManagerPrx>,
	public MyUtil::Singleton<UserRightAdapter>
{
public:
	UserRightAdapter() : MyUtil::ReplicatedClusterAdapterI<UserRightManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel, true,"URM"){
	}
	UserRightInfoPtr getUserRight(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserRight(int id, const UserRightInfoPtr& info);
	void createUserRight(int id, const UserRightInfoPtr& info);
	void reload(int id, const Ice::Context& ctx = Ice::Context());
	UserRightManagerPrx getUserRight10sManager(int userId);		
	UserRightManagerPrx getUserRightManager(int userId);
	UserRightManagerPrx getUserRightManagerOneway(int userId);

};

}
}
}
#endif

