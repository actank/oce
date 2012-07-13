/*
 * =====================================================================================
 *
 *       Filename:  UserDescAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年11月11日 17时37分54秒
 *       Revision:  none
 *       Compiler:  zh
 *
 *         Author:  han.zhang (zh), han.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#ifndef __USER_DESC_ADAPTER_H__
#define __USER_DESC_ADAPTER_H__

#include "UserDescReaderAdapter.h"

namespace xce {
namespace adapter {
namespace userdesc {

class UserDescAdapter : public MyUtil::Singleton<UserDescAdapter> {
public:
	UserDescAdapter() {
	}

        //@Deprecated
        //旧接口，得到的UserContact的qq字段是int
	xce::userdesc::UserDescDataPtr getUserDesc(int userid, const Ice::Context& ctx = Ice::Context());

        //新接口，得到的UserContact的qq字段是long
	xce::userdesc::UserDescDataNPtr getUserDescN(int userid, const Ice::Context& ctx = Ice::Context());
};

}
}
}

#endif

