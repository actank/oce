#ifndef TALKSTATADAPTER_H_
#define TALKSTATADAPTER_H_

#include "TalkStat.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace stat {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class TalkStatAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkStatAdapter> {
public:
	TalkStatAdapter(){
	}
	void login(const MyUtil::IntSeq& ids);
	void wLogin(const MyUtil::IntSeq& ids);

	void totalMsgCount(int count);
	void chatingMsgCount(int count);
	void msgStat(const JidPtr& from, const JidPtr& to);
	void versionStat(int userId, const string& ver, const string& subver);

	void mucMsgCount(int msgCount, int msgBroadcastCount);

	void incSetStatusCount(int c = 1);
	void incReplyStatusCount(int c = 1);
	void incReplyPhotoCount(int c = 1);
	void incReplyBlogCount(int c = 1);
	void incReplyShareCount(int c = 1);
private:
	//virtual void initialize();
	virtual string name() {
		return "TalkStat";
	}
	virtual string endpoints() {
		return "@TalkStat";
	}
	virtual size_t cluster() {
		return 0;
	}
	StatManagerPrx _managerOneway;
	StatManagerPrx getManager();
	//StatManagerPrx _managerOneway;

};

}
;
}
;

#endif /*TALKSTATADAPTER_H_*/
