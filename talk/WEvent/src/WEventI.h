#ifndef __WEVENT_I_H__
#define __WEVENT_I_H__

#include "WEvent.h"
#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include "TaskManager.h"
#include <IceUtil/Shared.h>
#include "Pipe.h"
#include "TalkProxy.h"
#include "XFeed.h"
#include "ServiceI.h"
#include "TalkCommon.h"

namespace talk {
namespace wevent {

using namespace MyUtil;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace mop::hi::svc::dispatch;

class QtfEscape : public Singleton<QtfEscape>{
public:
	QtfEscape();
	std::string escape(const std::string& text);
private:
	bool iscmd[256];
};



class NotifyPipe : public Pipe {
public:
	NotifyPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);
};

class XFeedPipe : public Pipe {
public:
	XFeedPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);

	static string getType(int stype);
	static string getSType(int type);
	static string getTemplate(int stype);
};

class ReplyTemplate : public IceUtil::Shared {
public:
	string prefix;
	string title;
	string sufix;
};
typedef IceUtil::Handle<ReplyTemplate> ReplyTemplatePtr;

class ReplyPipe : public Pipe {

public:
	ReplyPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);

	static string getType();
	static string getSType(const mop::hi::svc::dispatch::NotePtr& note);
	static string getTemplate(const mop::hi::svc::dispatch::NotePtr& note);

	static string getAid(const string& url);
	static string getId(const string& url, const string& name);
};

class NoticePipe : public Pipe{
public :
	NoticePipe(const string& name, int index) :
			Pipe(name, index) {
		}
		;

	virtual void handle(const ObjectSeq& seq);
};



class PipeFactoryI : public PipeFactory {
public:
	virtual PipePtr create(const string& type, int index) {
		if (type == "Notify") {
			return new NotifyPipe(type,index);
		} else if (type == "XFeed") {
			return new XFeedPipe(type, index);
		} else if (type == "Reply"){
			return new ReplyPipe(type,index);
		} else {
			return new NoticePipe(type,index);
		}

	}
	;
};

//--------------------------------------------------
const static string XFeedNewsDatabase = "xfeed_news";
const static string XFeedNewsTable = "xfeed_news";
const static int XFeedNewsTableCount = 100;

const static string XFeedContentDatabase = "xfeed_content";
const static string XFeedContentTable = "xfeed_content";

class WEventManagerI : public WEventManager,
	public MyUtil::Singleton<WEventManagerI> {

	map<int,ReplyTemplatePtr> _replyTempMap;
public:
	void loadReplyTemplate(const string& configFile);
	ReplyTemplatePtr getReplyTemplate(int type);

	virtual void deliverNotify(Ice::Int userid, const string& title,
			const string& msg, const Ice::Current& = Ice::Current());

	virtual void deliverXFeed(const xce::xfeed::XFeedLetterPtr& xftl,
			const Ice::Current& = Ice::Current());

	virtual void loadOfflineXFeed(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex,
			Ice::Int iqId, const Ice::Current& = Ice::Current());

	virtual void deliverReply(const NotePtr& note,
			const Ice::Current& = Ice::Current());
	//virtual void loadUnreadReply(const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void deliverAppNotice(Ice::Int toid,Ice::Int appid,const string& appName, const string& appIcon, const string& msg, const Ice::Current& = Ice::Current());
};

class LoadXFeedTask : public MyUtil::Task {
	JidPtr _jid;
	Ice::Long _feedIndex;
	Ice::Long _replyIndex;
	int _iqId;

	string getXFeedNewsTable() {
		ostringstream tab;
		tab<<XFeedNewsTable<<"_"<<_jid->userId % XFeedNewsTableCount;
		return tab.str();
	}
	void loadReply(const NoteSeq& seq);
public:
	LoadXFeedTask(const JidPtr& jid, Ice::Long feedIndex , Ice::Long replyIndex, int iqId) :
		Task(TASK_LEVEL_QUERY), _jid(jid), _feedIndex(feedIndex), _replyIndex(replyIndex), _iqId(iqId) {
	}
	;

	virtual void handle();
};


}
;
}
;

#endif /*MANAGERI_H_*/
