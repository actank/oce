#ifndef TALKCACHEADAPTER_H_
#define TALKCACHEADAPTER_H_

#include "AdapterI.h"
#include "TalkCache.h"

#include "Singleton.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::cache;

const int UI = 0;
const int GL = 2;

class TalkCacheAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkCacheAdapter> {
public:
	TalkCacheAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	ObjectResultPtr getUserList(const IntSeq& ids, int timeout = 260);
	UserPtr getUser(int id, int timeout = 260);
	void reloadUser(int id);
	void updateUser(int id, const MyUtil::Str2StrMap& props);
protected:
	virtual string name() {
		return "TalkCache";
	}
	virtual string endpoints() {
		return "@TalkCache";
	}
	;
	virtual size_t cluster() {
		return 5;
	}

	CacheManagerPrx getManagerOneway(int id);

	vector<CacheManagerPrx> _managersOneway;

};

}
;
}
;
}
;

#endif /*TALKCACHEADAPTER_H_*/
