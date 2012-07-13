#ifndef IQTASK_H_
#define IQTASK_H_

#include "TaskManager.h"
#include "TalkCommon.h"
#include "MucCommon.h"
#include "TalkUtil/src/pugixml.hpp"

namespace com {
namespace xiaonei {
namespace talk {
using namespace pugi;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::muc;


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


class IqGetTask : public Task {

	JidPtr _jid;
	xml_document_ptr _doc;
public:
	IqGetTask(int level, const JidPtr& jid, const xml_document_ptr& doc) : Task(level),
		_jid(jid), _doc(doc) {
	}
	;
	virtual void handle();
protected:
	void _discoverService(xml_document& reply);
	void _getItems(xml_document& reply);
	void _getRoomConfig();
	void _getMemberList();
	void _default();
	int	_mucGetRole(string role);
	int	_mucGetAffiliation(string affiliation);
	Permisions _mucGetPermision(string permision);
	void _getFeedReply();
	void _getFeedReplyCount();
	void _getSessionKey();
  void _getWebTicket(xml_document& reply);
private:
	void getPrivacyHandle(xml_document& reply);
	void getPrivateHandle(xml_document& reply);
	void getRosterHandle(xml_document& reply);
	void getVcardHandle(xml_document& reply);
	void getXFeedHandle(xml_document& reply);
	void getTransferHandle(xml_document& reply);
	void getUniqueRoomName(xml_document& reply);
};
class IqSetTask : public Task {
	JidPtr _jid;
	xml_document_ptr _doc;
public:
	IqSetTask(const JidPtr& jid, const xml_document_ptr& doc) :
		_jid(jid), _doc(doc) {
	}
	;
	virtual void handle();
private:
	void setSessionHandle(xml_document& reply);
	void setRosterHandle(xml_document& reply);
	void setPrivacyHandle(xml_document& reply);
	void setPrivateHandle(xml_document& reply);
	void setJingleHandle(xml_document& reply);
	void setNotifyHandle(xml_document& reply);
	
	Permisions Role2Permision(string role);
	Permisions Affiliation2Permision(const string& affilication);
	void _mucSetPrivilege();
	void _mucSetAffiliation();
	int	_mucGetRole(string role);
	int	_mucGetAffiliation(string affiliation);
	void _mucSetRoomConfig();
	void _mucDestroyRoom();
	string	_mucRoleStr(int role);
	string	_mucAffiliationStr(int affiliation);
};

class IqResultTask : public Task {

	JidPtr _jid;
	xml_document_ptr _doc;
public:
	IqResultTask(const JidPtr& jid, const xml_document_ptr& doc) :
		_jid(jid), _doc(doc) {
	}
	;
	virtual void handle();
};


class IqErrorTask : public Task {

	JidPtr _jid;
	xml_document_ptr _doc;
public:
	IqErrorTask(const JidPtr& jid, const xml_document_ptr& doc) :
		_jid(jid), _doc(doc) {
	}
	;
	virtual void handle();
};








//-------------------memcache TalkCacheLoader test
class TalkCacheLoaderIdTask : public Task{
public:
	TalkCacheLoaderIdTask(int id) : _id(id){
	}
	virtual void handle();
private:
	int _id;
};

class TalkCacheLoaderSeqTask : public Task{
public:
	TalkCacheLoaderSeqTask(int uid, const IntSeq& ids):_ids(ids), id(uid){
	}
	virtual void handle();
private:
	IntSeq _ids;
  int id;
};

}
;
}
;
}
;
#endif /*IQTASK_H_*/

