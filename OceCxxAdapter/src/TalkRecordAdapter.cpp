#include "TalkRecordAdapter.h"

namespace com {
namespace xiaonei {
namespace talk{
namespace record{
void TalkRecordAdapter::record(::Ice::Int from, ::Ice::Int to, const string& msg){
	if (!_proxy) {
		_proxy = TalkRecordManagerPrx::uncheckedCast(locate<TalkRecordManagerPrx>("TRM", ONE_WAY)->ice_datagram());
	}
	if(_proxy){
		_proxy->record(from, to, msg);
	}
}
}
}
}
}
