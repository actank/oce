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


#ifndef __USER_LOGINCOUNT_ADAPTER_H__
#define __USER_LOGINCOUNT_ADAPTER_H__

#include "UserBase.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace userlogincount{

using namespace xce::userbase;
using namespace MyUtil;

const int CLUSTER = 100;

class UserLoginCountAdapter : public MyUtil::ReplicatedClusterAdapterI<UserLoginCountManagerPrx>,
	public MyUtil::Singleton<UserLoginCountAdapter>
{
public:
	UserLoginCountAdapter() : MyUtil::ReplicatedClusterAdapterI<UserLoginCountManagerPrx>("ControllerUserBase",120,300,new MyUtil::OceChannel,true,"ULCM"){
	}
	
	int getUserLoginCount(int userId, const Ice::Context& ctx = Ice::Context());
  void createUserLoginCount(int userid);
  void incUserLoginCount(int userid);
	void reload(int id, const Ice::Context& ctx = Ice::Context());
private:
		

	UserLoginCountManagerPrx getUserLoginCountManager(int userId);
	UserLoginCountManagerPrx getUserLoginCount10sManager(int userId);
	UserLoginCountManagerPrx getUserLoginCountManagerOneway(int userId);

};

}
}
}

#endif

