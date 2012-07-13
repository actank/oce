#include "TalkCacheLoaderAdapter.h"
#include "IceLogger.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::cache::loader;
using namespace MyUtil;


CacheLoaderManagerPrx TalkCacheLoaderAdapter::getManager(int id) {
  return locate<CacheLoaderManagerPrx>(_managers, "M", id, TWO_WAY);
}

CacheLoaderManagerPrx TalkCacheLoaderAdapter::getManagerOneway(int id) {
	return locate<CacheLoaderManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

void TalkCacheLoaderAdapter::updateUser(int id, const MyUtil::Str2StrMap& props){
	getManagerOneway(id)->updateUser(id,props);
}

TalkUserPtr TalkCacheLoaderAdapter::LoadById(int userid){
  return getManager(userid)->LoadById(userid);
}

TalkUserMap TalkCacheLoaderAdapter::LoadBySeq(const MyUtil::IntSeq& userids){
	if(userids.empty()){
		return TalkUserMap();
	}
	return getManager(userids.size())->LoadBySeq(userids);
}


