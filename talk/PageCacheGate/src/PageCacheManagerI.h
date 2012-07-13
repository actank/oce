#ifndef PAGECACHEMANAGERI_H_
#define PAGECACHEMANAGERI_H_
#include <ctemplate/template.h>
#include "XmppTools.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"
#include "PageCache.h"

using namespace com::xiaonei::talk::util;
using namespace xce::notify;
using namespace ctemplate;

namespace talk {
namespace http {

using namespace MyUtil;


class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};




class PageCacheRequest : public Request{
public:
  PageCacheRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

//-------------------------------------------------
class PageCacheManagerI : public PageCacheManager, virtual public Singleton<PageCacheManagerI> {
public:
	virtual ContentPtr GetContent(const string& path,
			const MyUtil::Str2StrMap& cookies,
			const MyUtil::Str2StrMap& parameter,
			const Ice::Current& = Ice::Current()) {
    return NULL;
  }
	virtual void ReloadStaticPages(const Ice::Current& = Ice::Current()) {}
	virtual void MemberStatusChange(int groupid, int userid, int mucstatus, const Ice::Current& = Ice::Current()){}
	virtual void UpdateRoomName(int groupid, const string& gorupname, const Ice::Current& = Ice::Current()){}
	virtual int GetVideoViewPower(const Ice::Current& = Ice::Current()){return 0;}	
	virtual void SetVideoViewPower(int value, const Ice::Current& = Ice::Current()){}

};

}
}

#endif // PAGECACHEMANAGERI_H_
