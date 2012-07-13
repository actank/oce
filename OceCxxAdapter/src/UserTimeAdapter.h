/*
 * =====================================================================================
 *
 *       Filename:  UserTimeAdapter.h
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


#ifndef __USER_TIME_ADAPTER_H__
#define __USER_TIME_ADAPTER_H__

#include "UserTimeReaderAdapter.h"
#include "UserTimeWriterAdapter.h"

namespace xce {
namespace adapter {
namespace usertime {

class UserTimeAdapter : public MyUtil::Singleton<UserTimeAdapter> {
public:
	UserTimeAdapter() {
	}

	UserTimeInfoPtr getUserTime(int userId, const Ice::Context& ctx = Ice::Context());
	void reload(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserTime(int userId, const MyUtil::Str2StrMap& props, const Ice::Context& ctx = Ice::Context());
	void setUserTime(int userId, const UserTimeInfoPtr& info);
	void createUserTime(int userId, const MyUtil::Str2StrMap& props);
	void createUserTime(int userId, const UserTimeInfoPtr& info);

};

}
}
}

#endif

