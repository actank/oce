#include "LoginMsgHandlerAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


LoginHandlerPrx LoginMsgHandlerAdapter::getManager(int id) {
	return locate<LoginHandlerPrx>(_managers, "M", id, TWO_WAY);
}

LoginHandlerPrx LoginMsgHandlerAdapter::getManagerOneway(int id) {
	return locate<LoginHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

PassportPtr LoginMsgHandlerAdapter::verify(const string& email,const string& password)
{
	return getManager(email.size())->verify(email,password);
}

void LoginMsgHandlerAdapter::bind(const JidPtr& j,const Ice::Context& ctx)
{
	getManagerOneway(j->userId)->bind(j,ctx);
}
void LoginMsgHandlerAdapter::unbind(const JidPtr & j)
{
	getManagerOneway(j->userId)->unbind(j);
}


