#include "MsgFilterAdapter.h"

using namespace com::xiaonei::antispam::wap::generation;

ContentFilterWapPrx MsgFilterAdapter::getManager() {
	if (!_manager) {
		_manager = locate<ContentFilterWapPrx >("wapFilter", TWO_WAY);
	}
	return _manager;
}

string MsgFilterAdapter::wapFilter(const string& content){
	return getManager()->wapFilter(content);
}
