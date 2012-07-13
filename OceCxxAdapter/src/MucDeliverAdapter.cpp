#include "MucDeliverAdapter.h"
#include <boost/lexical_cast.hpp>
#include "XmppTools.h"

using namespace talk::mucdeliver::adapter;
using namespace com::xiaonei::talk::util;


MucDeliverManagerPrx MucDeliverAdapter::getManager(int id) {
	return locate<MucDeliverManagerPrx>(_managers, "M", id, TWO_WAY);
}

MucDeliverManagerPrx MucDeliverAdapter::getManagerOneway(int id) {
	return locate<MucDeliverManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//----------------------------------------

void MucDeliverAdapter::deliver(const MessageSeq& seq) {
	if(seq.empty()){
		return;
	}
	getManagerOneway(seq.at(0)->to->userId)->deliver(seq);
}
