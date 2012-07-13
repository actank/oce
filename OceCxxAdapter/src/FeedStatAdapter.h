#ifndef __FEEDSTAT_ADAPTER_H__
#define __FEEDSTAT_ADAPTER_H__

#include "AdapterI.h"
#include "FeedStat.h"

namespace xce{
	namespace feed{
		using namespace MyUtil;
		class FeedStatAdapter : public AdapterI, public AdapterISingleton<MyUtil::TalkChannel, FeedStatAdapter>{
			public:
				void dispatchRecord(Ice::Int stype, const string& logmsg);
				void replyRecord(Ice::Int stype, const string& logmsg);
				void deliverRecord(Ice::Int stype, const string& logmsg, Ice::Int count);
				int getCluster(){
					return _cluster;
				}
			protected:
				virtual string name(){
					return "FeedStat";
				}
				virtual string endpoints(){
					return "@FeedStat";
				}
				virtual size_t cluster(){
					return 0;
				}
				FeedStatManagerPrx getManagerOneway();
			private:
				FeedStatManagerPrx 	_proxy;
		};
	};
};
#endif
