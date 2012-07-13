/*
 * =====================================================================================
 *
 *       Filename:  UserContactAdapter.h
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


#ifndef __USER_CONTACT_ADAPTER_H__
#define __USER_CONTACT_ADAPTER_H__

#include "UserContactReaderAdapter.h"
#include "UserContactWriterAdapter.h"

namespace xce {
namespace adapter {
namespace usercontact {

class UserContactAdapter : public MyUtil::Singleton<UserContactAdapter> {
public:
	UserContactAdapter(){
	}

	//@Deprecated
	//旧接口，得到的UserContact的qq字段是int
	xce::userdesc::UserContactDataPtr getUserContact(int userId, const Ice::Context& ctx = Ice::Context());

	//新接口，得到的UserContact的qq字段是long
	xce::userdesc::UserContactDataNPtr getUserContactN(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserContact(int userid, const UserContactInfoPtr& info);
	void setUserContact(int userId, const MyUtil::Str2StrMap& props);
	void createUserContact(int userId, const UserContactInfoPtr& info);
	void createUserContact(int userId, const MyUtil::Str2StrMap& props);
	void reload(int id, const Ice::Context& ctx = Ice::Context());
};

}
}
}

#endif

