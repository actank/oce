#ifndef MSGFILTER_ADAPTER_H_ 
#define MSGFILTER_ADAPTER_H_ 

#include "Singleton.h"
#include "AdapterI.h"
#include "antispam.h"

namespace com {
namespace xiaonei {
namespace antispam{
namespace wap {
namespace generation {

using namespace MyUtil;

class MsgFilterAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::MsgFilterChannel, MsgFilterAdapter> {
public:
	string wapFilter(const string& content);
protected:
	virtual string name() {
		return "WapFilter";
	}
	virtual string endpoints() {
		return "@WapFilter";
	}
	virtual size_t cluster() {
		return 0;
	}
	ContentFilterWapPrx getManager();
	ContentFilterWapPrx _manager;

};

}
}
}
}
}

#endif /*RESTMEDIATORADAPTER_H_*/
