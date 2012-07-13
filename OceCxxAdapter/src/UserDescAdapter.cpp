/*
 * =====================================================================================
 *
 *       Filename:  UserDescAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年11月11日 17时42分36秒
 *       Revision:  none
 *       Compiler:  zh
 *
 *         Author:  han.zhang (zh), han.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "UserDescAdapter.h"
#include "UserDescReaderAdapter.cpp"

using namespace xce::adapter::userdesc;

UserDescDataPtr UserDescAdapter::getUserDesc(int userid, const Ice::Context& ctx)
{
	return UserDescReaderAdapter::instance().getUserDesc(userid, ctx);
}

UserDescDataNPtr UserDescAdapter::getUserDescN(int userid, const Ice::Context& ctx)
{
	return UserDescReaderAdapter::instance().getUserDescN(userid, ctx);
}
