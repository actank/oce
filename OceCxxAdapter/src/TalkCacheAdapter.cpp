#include "TalkCacheAdapter.h"
#include "IceLogger.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache;
using namespace MyUtil;

CacheManagerPrx TalkCacheAdapter::getManagerOneway(int id) {
	return locate<CacheManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

ObjectResultPtr TalkCacheAdapter::getUserList(const IntSeq& ids, int timeout) {
	LongSeq lids;
	for (IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		lids.push_back(*it);
	}
	return get(UI, lids, true, timeout);
}

UserPtr TalkCacheAdapter::getUser(int id, int timeout) {
	return UserPtr::dynamicCast(get(UI, id, timeout));
}

void TalkCacheAdapter::reloadUser(int id) {
	remove(UI, id);
}

void TalkCacheAdapter::updateUser(int id, const MyUtil::Str2StrMap& props){
	getManagerOneway(id)->updateUser(id,props);
}





