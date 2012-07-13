/*
 * =====================================================================================
 *
 *       Filename:  UserFavAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时09分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "UserFavAdapter.h"
#include "Date.h"
using namespace xce::adapter::userfav;

UserFavManagerPrx UserFavAdapter::getUserFavManager(int id)
{
	return locate<UserFavManagerPrx>(_managers, "F", id, TWO_WAY);
}

UserFavManagerPrx UserFavAdapter::getUserFavManagerOneway(int id)
{
	return locate<UserFavManagerPrx>(_managersOneway, "F", id, ONE_WAY);
}

void UserFavAdapter::reload(int id, const Ice::Context& ctx)
{
    getUserFavManager(id)->reload(id, ctx);
}
