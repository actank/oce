#include "TalkLogAdapter.h"

namespace xiaonei {
namespace talk {

void TalkLogAdapter::feed(const JidPtr& from, const JidPtr& to, const string& act){
	if (!_proxy) {
		_proxy = TalkLogManagerPrx::uncheckedCast(locate<TalkLogManagerPrx>("M", ONE_WAY)->ice_datagram());
	}
	if(_proxy){
		_proxy->feed(from, to, act);
	}
}

}
}
