#include "UserDescReaderAdapter.h"
#include "Date.h"
using xce::userdesc::UserDescDataPtr;
using xce::userdesc::UserDescReaderPrx;
using namespace xce::adapter::userdesc;
using namespace xce::userdesc;

UserDescReaderPrx UserDescReaderAdapter::getUserDescReader(int id)
{
	return getProxy(id);
}

UserDescReaderPrx UserDescReaderAdapter::getUserDescReaderOneway(int id)
{
	return getProxyOneway(id);
}

UserDescDataPtr UserDescReaderAdapter::getUserDesc(int userid, const Ice::Context& ctx)
{
	return getUserDescReader(userid)->getUserDesc(userid, ctx);
}

UserDescDataNPtr UserDescReaderAdapter::getUserDescN(int userid, const Ice::Context& ctx)
{
	return getUserDescReader(userid)->getUserDescN(userid, ctx);
}
