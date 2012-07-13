/*
 * =====================================================================================
 *
 *       Filename:  UserFavAdapter.h
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


#ifndef __USER_FAV_ADAPTER_H__
#define __USER_FAV_ADAPTER_H__

#include "UserDesc.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace userfav {

using namespace ::xce::userdesc;
using namespace MyUtil;

const int CLUSTER = 100;

class UserFavAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::UserChannel, UserFavAdapter>
{
public:
	UserFavAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);

	}
	void reload(int id, const Ice::Context& ctx = Ice::Context());
private:
		
	virtual string name() {
		return "UserDesc";
	}
	;
	virtual string endpoints() {
		return "@UserDesc";
	}
	;
	virtual size_t cluster() {
		return CLUSTER;
	}
	;

	UserFavManagerPrx getUserFavManager(int userId);
	UserFavManagerPrx getUserFavManagerOneway(int userId);

	vector<UserFavManagerPrx> _managers;
	vector<UserFavManagerPrx> _managersOneway;

};

}
}
}

#endif

