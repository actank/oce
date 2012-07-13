#include "FeedStatAdapter.h"
namespace xce{
	namespace feed{
		FeedStatManagerPrx FeedStatAdapter::getManagerOneway(){
			if(!_proxy){
				_proxy = FeedStatManagerPrx::checkedCast(locate<FeedStatManagerPrx>("FSM", ONE_WAY)->ice_datagram());
			}
			return _proxy;
		}
		void FeedStatAdapter::dispatchRecord(Ice::Int stype, const string& logmsg){
			getManagerOneway()->dispatchRecord(stype, logmsg);
		}
		void FeedStatAdapter::replyRecord(Ice::Int stype, const string& logmsg){
			getManagerOneway()->replyRecord(stype, logmsg);
		}
		void FeedStatAdapter::deliverRecord(Ice::Int stype, const string& logmsg, ::Ice::Int count){
			getManagerOneway()->deliverRecord(stype, logmsg, count);
		}
	}
}



