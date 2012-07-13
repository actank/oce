#ifndef RESTLOGICADAPTER_H_
#define RESTLOGICADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RestLogic.h"

using namespace com::xiaonei::talk::common;
namespace talk {
namespace rest {

using namespace MyUtil;

class RestLogicAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, RestLogicAdapter> {
public:
	void setStatus(const JidPtr& jid, OnlineStat state, const string& status);
	void replyStatus(int userid, Ice::Long statusId, int ownerId, int replieduserId, const string& content);
	void replyPhoto(int userid, Ice::Long resId, int ownerId, int replieduserId, const string& content, int type);
	void replyAlbum(int userid, Ice::Long resId, int ownerId, int replieduserId, const string& content, int type);
	void replyBlog(int userid, Ice::Long resId, int ownerId, int replieduserId, const string& content, int type);
	void useImNotify(int userid);
	void activeUserNotify(int userid);
	void activeUserNotify2(int userid);
	void notifyResult(Ice::Long reqId, const RestResultPtr& res);
	void getSessionKey(const JidPtr& jid);
	void getSessionKey(const JidPtr& jid, int iqid);
protected:
	virtual string name() {
		return "RestLogic";
	}
	virtual string endpoints() {
		return "@RestLogic";
	}
	virtual size_t cluster() {
		return 5;
	}

	RestLogicManagerPrx getManagerOneway(int id);
	vector<RestLogicManagerPrx> _managersOneway;

};
}
}

#endif /*RESTMEDIATORADAPTER_H_*/
