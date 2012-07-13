/*
 * 热备过渡期兼容方案，替代UserContactAdapter
 * 把CPP调用从oldwr切到Reader/Writer上
 * 调用者只需要修改头文件和编译规则，不需要修改代码
 *
 *
 */

#ifndef __USER_CONTACT_ADAPTER_WR_H__
#define __USER_CONTACT_ADAPTER_WR_H__

#include "UserContactReaderAdapter.h"
#include "UserContactWriterAdapter.h"

namespace xce {
namespace adapter {
namespace usercontact {

using namespace ::xce::userdesc;
using namespace MyUtil;

class UserContactWRAdapter : public MyUtil::Singleton<UserContactWRAdapter> {
public:
	UserContactDataPtr getUserContact(int userId, const Ice::Context& ctx = Ice::Context());
	void setUserContact(int userid, const UserContactInfoPtr& info);
	void setUserContact(int userId, const MyUtil::Str2StrMap& props);
	void createUserContact(int userId, const UserContactInfoPtr& info);
	void createUserContact(int userId, const MyUtil::Str2StrMap& props);
	void reload(int id, const Ice::Context& ctx = Ice::Context());
private:
};

}
}
}

#endif

