#ifndef PRESENCETASK_H_
#define PRESENCETASK_H_

#include "TaskManager.h"
#include "TalkCommon.h"
#include "TalkUtil/src/pugixml.hpp"

namespace com
{
namespace xiaonei
{
namespace talk
{
using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;

/*class PresenceDefultTask : public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;

public:
	PresenceDefultTask(const JidPtr& jid, const xml_document_ptr& doc):_jid(jid),_doc(doc){};
	virtual void handle();
};*/

class PresenceSubscribeTask : public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;

public:
	PresenceSubscribeTask(const JidPtr& jid, const xml_document_ptr& doc):_jid(jid),_doc(doc){};
	virtual void handle();
};

class PresenceSubscribedTask : public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;

public:
	PresenceSubscribedTask(const JidPtr& jid, const xml_document_ptr& doc):_jid(jid),_doc(doc){};
	virtual void handle();
};

class PresenceUnsubscribedTask : public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;

public:
	PresenceUnsubscribedTask(const JidPtr& jid, const xml_document_ptr& doc):_jid(jid),_doc(doc){};
	virtual void handle();
};

class RoomPresenceTask: public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;

public:
	RoomPresenceTask(const JidPtr& jid, const xml_document_ptr& doc):_jid(jid),_doc(doc){};
	virtual void handle();
protected:
	void	_modifyPresence(OccupantPtr occupant, RoomJidPtr newRoomJid);
	string _affiliationStr(Affiliation affiliation);
	string _roleStr(RoomRole role);
};

class RoomPresenceUnavailableTask: public Task
{
	JidPtr _jid;
	xml_document_ptr _doc;

public:
	RoomPresenceUnavailableTask(const JidPtr& jid, const xml_document_ptr& doc):_jid(jid),_doc(doc){};
	virtual void handle();
protected:
	string _affiliationStr(Affiliation affiliation);
	string _roleStr(RoomRole role);
};

class KickUserTimer : public MyUtil::Timer, public Singleton<KickUserTimer>{
public:
        KickUserTimer():Timer(30*1000){}
	void handle();
        void push(int userid){
                IceUtil::Mutex::Lock lock(_mutex);
                _users.push_back(make_pair<int, int>(userid, time(NULL)));
        }
public:
        IceUtil::Mutex          _mutex;
        list<pair<int, int> > _users;
};

};
};
};


#endif /*PRESENCETASK_H_*/
