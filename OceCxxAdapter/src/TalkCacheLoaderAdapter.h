#ifndef TALKCACHELOADERADAPTER_H_
#define TALKCACHELOADERADAPTER_H_

#include "AdapterI.h"
#include "TalkCacheLoader.h"

#include "Singleton.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::cache::loader;

class TalkCacheLoaderAdapter : public AdapterI, public AdapterISingleton<MyUtil::TalkChannel, TalkCacheLoaderAdapter> {
public:
  TalkCacheLoaderAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}

  void updateUser(int userid, const MyUtil::Str2StrMap& props);
  TalkUserPtr LoadById(int userid);
  TalkUserMap LoadBySeq(const MyUtil::IntSeq& userids);

protected:
	virtual string name() {
		return "TalkCacheLoader";
	}
	virtual string endpoints() {
		return "@TalkCacheLoader";
	}
	;
	virtual size_t cluster() {
		return 5;
	}

	CacheLoaderManagerPrx getManagerOneway(int id);
	CacheLoaderManagerPrx getManager(int id);

	vector<CacheLoaderManagerPrx> _managersOneway;
	vector<CacheLoaderManagerPrx> _managers;
};

}
;
}
;
}
;

#endif /*TALKCACHEADAPTER_H_*/
