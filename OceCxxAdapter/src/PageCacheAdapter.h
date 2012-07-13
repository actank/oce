#ifndef PAGECACHEADAPTER_H_
#define  PAGECACHEADAPTER_H_

#include "AdapterI.h"

#include "PageCache.h"

namespace talk {
namespace http {

class PageCacheAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::TalkChannel, PageCacheAdapter> {
public:
	ContentPtr GetContent(int id, const string& path, const MyUtil::Str2StrMap& cookies, const MyUtil::Str2StrMap& parameter);
	void MemberStatusChange(int groupid, int userid, int mucstatus);
	void UpdateRoomName(int groupid, const string& groupname);
	int GetVideoViewPower(int server_index);
	void SetVideoViewPower(int server_index, int value);
protected:
	virtual string name() {
		return "PageCache";
	}
	;
	virtual string endpoints() {
		return "@PageCache";
	}
	;
	virtual size_t cluster() {
		return 10;
	}
	;
	PageCacheManagerPrx getManager(int id, int timeout = 260);
	PageCacheManagerPrx getManagerOneway(int id);
	
	vector<PageCacheManagerPrx> _managers;
	vector<PageCacheManagerPrx> _managersOneway;
};

}
;
}
;

#endif /*CONTENTCOLLECTORADAPTER_H_*/
