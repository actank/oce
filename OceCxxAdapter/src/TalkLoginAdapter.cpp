#include "TalkLoginAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


LoginHandlerPrx TalkLoginAdapter::getManager(int id) {
	return locate<LoginHandlerPrx>(_managers, "M", id, TWO_WAY);
}

LoginHandlerPrx TalkLoginAdapter::getManagerOneway(int id) {
	return locate<LoginHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

PassportPtr TalkLoginAdapter::verify(const string& email,const string& password)
{
	return getManager(email.size())->verify(email,password);
}

void TalkLoginAdapter::bind(const JidPtr& j)
{
	getManager(j->userId)->bind(j);
}
void TalkLoginAdapter::unbind(const JidPtr & j)
{
	getManager(j->userId)->unbind(j);
}


