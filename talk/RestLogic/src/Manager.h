#ifndef RESTLOGIC_H_
#define RESTLOGIC_H_

#include <IceUtil/RWRecMutex.h>
#include "RestTask.h"
#include "TaskManager.h"
#include "Singleton.h"
#include "IceLogger.h"

using namespace MyUtil;

namespace talk{
namespace rest{


class RestLogicManagerI : virtual public RestLogicManager,
							//virtual public Timer,
							public Singleton<RestLogicManagerI>
{
public:
	RestLogicManagerI()/* : Timer(1000)*/{
		_id = 0;
	}
	//void statusReply(const JidPtr& jid, OnlineState state, );
	void setStatus(const JidPtr& jid, OnlineStat state, const string& status,
					const Ice::Current& = Ice::Current());
	void replyStatus(int userid, Ice::Long statusId, int ownerId, int replieduserId,
					const string& content, const Ice::Current& = Ice::Current());
	void replyPhoto(int userid, Ice::Long photoId, int ownerId, int replieduserId,
					const string& content, int replyId, const Ice::Current& = Ice::Current());
	void replyAlbum(int userid, Ice::Long albumId, int ownerId, int replieduserId,
					const string& content, int replyId, const Ice::Current& = Ice::Current());
	void replyBlog(int userid, Ice::Long blogId, int ownerId, int replieduserId,
					const string& content, int replyId, const Ice::Current& = Ice::Current());

	void useImNotify(int userid, const Ice::Current& = Ice::Current());
	void activeUserNotify(int userid, const Ice::Current& = Ice::Current());
	void activeUserNotify2(int userid, const Ice::Current& = Ice::Current());
	void sendNotice(int userid, const string& data, const MyUtil::IntSeq& userids, const Ice::Current& = Ice::Current());
	void sendLobbyNotice(int userid, const string& data, const MyUtil::IntSeq& userids, const Ice::Current& = Ice::Current());
	void notifyResult(Ice::Long reqId, const RestResultPtr& content,
					const Ice::Current& = Ice::Current());
	void getSessionKey(const JidPtr& jid, const Ice::Current& = Ice::Current());
	void getSessionKey2(const JidPtr& jid, int iqid, const Ice::Current& = Ice::Current());
	void handle();

	void index(int i){
		_index = ((Ice::Long)i) << 32;
	}

  // 帮助客户端转码
  void ClientEncode(string& status);
	//int index(){
	// 帮助客户端对沙发回复做处理
	void EncodeReplyStatus(string& status);
	//	return _index;
	//}
protected:
	void addReq(const RestTaskPtr& req);
	void removeReq(Ice::Long id);
	Ice::Long id(){
		Ice::Long id = (_index | (Ice::Long)_id++);
		//MCE_DEBUG("id --> index " << _index << ", _id " << _id << ", id " << id);
		//MCE_DEBUG("id --> index " << (_index>>32) << ", _id " << _id << ", id " << id);
		return id;
	}
private:
	typedef list<RestTaskPtr>::iterator Pos;
	list<RestTaskPtr> 	_reqList;
	map<Ice::Long, Pos> _reqMap;
	IceUtil::RWRecMutex _mutex;
	int					_id;
	Ice::Long			_index;

};

class TimeoutTimer : public Timer{
public:
	TimeoutTimer() : Timer(1000){

	}
	void handle(){
		//TaskManager::instance().schedule(this);
		RestLogicManagerI::instance().handle();
	}
};

};
}
#endif
