#include "PageCacheAdapter.h"

using namespace MyUtil;
using namespace talk::http;

PageCacheManagerPrx PageCacheAdapter::getManager(int id, int timeout) {
	if(timeout != 260){
		PageCacheManagerPrx prx =  locate<PageCacheManagerPrx>(_managers, "M", id, TWO_WAY);
		return PageCacheManagerPrx::uncheckedCast(prx->ice_timeout(timeout));
	}
	return locate<PageCacheManagerPrx>(_managers, "M", id, TWO_WAY);
}
PageCacheManagerPrx PageCacheAdapter::getManagerOneway(int id){
	return locate<PageCacheManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

ContentPtr PageCacheAdapter::GetContent(int id, const string& path, const MyUtil::Str2StrMap& cookies, const MyUtil::Str2StrMap& parameter){
		if(id){
			return getManager(id)->GetContent(path,cookies,parameter);
		}else{
			static int cc = 0;
			if(cc > 1000){
				cc = 0;
			}
			return getManager(++cc)->GetContent(path,cookies,parameter);
			
		}
}
void PageCacheAdapter::MemberStatusChange(int groupid, int userid, int mucstatus){
	getManagerOneway(groupid)->MemberStatusChange(groupid, userid, mucstatus);
}
void PageCacheAdapter::UpdateRoomName(int groupid, const string& groupname){
	getManagerOneway(groupid)->UpdateRoomName(groupid, groupname);
}

int PageCacheAdapter::GetVideoViewPower(int server_index){
	return getManager(server_index)->GetVideoViewPower();
}
void PageCacheAdapter::SetVideoViewPower(int server_index, int value){
	getManagerOneway(server_index)->SetVideoViewPower(value);
}
