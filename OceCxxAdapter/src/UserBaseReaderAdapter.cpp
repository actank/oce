/*
 * UserBaseReaderAdapter.cpp
 *
 */
#include "UserBaseReaderAdapter.h"
#include "Date.h"

using namespace xce::adapter::userbasereader;

UserBaseFullViewPtr UserBaseReaderAdapter::getUserBaseFullView(int id, const Ice::Context& ctx)  {
	return new UserBaseFullView(getUserBaseViewReader(id)->getUserBaseView(id, ctx));
}

UserBaseCommonViewPtr UserBaseReaderAdapter::getUserBaseCommonView(int id, const Ice::Context& ctx) {
	return new UserBaseCommonView(getUserBaseViewReader(id)->getUserCommonView(id, ctx));
}

UserBaseViewReaderPrx UserBaseReaderAdapter::getUserBaseViewReader(int id)
{
	return getProxy(id);
}

UserBaseViewReaderPrx UserBaseReaderAdapter::getUserBaseViewReaderOneway(int id)
{
	return getProxyOneway(id);
}

