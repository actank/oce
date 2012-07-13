#ifndef __USERTOUSERRECORD_ADAPTER_H__
#define __USERTOUSERRECORD_ADAPTER_H__

#include "AdapterI.h"
#include "TalkRecord.h"

namespace com {
	namespace xiaonei{
			namespace talk {
				namespace record{
					using namespace MyUtil;
					class TalkRecordAdapter : public AdapterI,public AdapterISingleton<MyUtil::TalkChannel, TalkRecordAdapter> {
					public:
						void record( ::Ice::Int from, ::Ice::Int to, const string& msg);
						int getCluster() {
							return _cluster;
						};
					protected:
						virtual string name() {
							return "TalkRecord";
						}
						virtual string endpoints() {
							return "@TalkRecord";
						}
						virtual size_t cluster() {
							return 0;
						}
						TalkRecordManagerPrx _proxy;
					};

				};
			}
	}
};

#endif
