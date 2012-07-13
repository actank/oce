#include <boost/lexical_cast.hpp>
#include "Manager.h"
#include "ServiceI.h"
#include "RestTask.h"
#include "TalkStatAdapter.h"
#include "XmppTools.h"
#include "client/linux/handler/exception_handler.h"
#include <boost/regex.hpp>

using namespace com::xiaonei::talk::util;
using namespace talk::rest;
using namespace talk::stat;
using namespace boost;

namespace MyUtil{

	bool MyCallback(const char *dump_path,
			const char *minidump_id,
			void *context,
			bool succeeded) {
		printf("got crash %s %s\n", dump_path, minidump_id);
		return true;
	}




void initialize() {
	static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
                                         , 0, true);
	
	
	ServiceI& service = ServiceI::instance();

	int serverId = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service.ServerIndex", -1);
	MCE_DEBUG("initialize --> index " << serverId);
	RestLogicManagerI::instance().index(serverId);
	service.getAdapter()->add(&RestLogicManagerI::instance(), service.createIdentity("M", ""));
	TaskManager::instance().scheduleRepeated(new TimeoutTimer());
	//TaskManager::instance().scheduleRepeated(&LastWeekLoginUser::instance());
}

}

void RestLogicManagerI::addReq(const RestTaskPtr& req){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	req->id(id());
	_reqList.push_back(req);
	_reqMap[req->id()] = --_reqList.end();
	MCE_DEBUG("RestLogicManagerI::addReq --> req id " << req->id());
	req->_handle();
}

void RestLogicManagerI::removeReq(Ice::Long id){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	map<Ice::Long, Pos>::iterator it = _reqMap.find(id);
	if(it != _reqMap.end()){
		_reqMap.erase(it);
		_reqList.erase(it->second);
	}
}

void RestLogicManagerI::handle(){
	MCE_DEBUG("RestLogicManagerI::handle");
	while(true){
		RestTaskPtr req;
		{
			IceUtil::RWRecMutex::WLock lock(_mutex);
			if(_reqList.empty()){
				break;
			}
			req = _reqList.front();
			if(!req->isTimeout()){
				break;
			}
			_reqList.pop_front();

			//MCE_WARN("timeout ");

			if(req->isRetry()){
				MCE_WARN("retry");
				_reqList.push_back(req);
				map<Ice::Long, Pos>::iterator it = _reqMap.find(req->id());
				if(it != _reqMap.end()){
					it->second = --_reqList.end();
				}else{
					MCE_WARN("somethig may be wrong in " << __LINE__);
					_reqMap[req->id()] = --_reqList.end();
				}
			}else{
				_reqMap.erase(req->id());
			}
		}
		if(req->isRetry()){
			req->_handle();
		}else{
			req->timeout();
		}
	}
}



void RestLogicManagerI::ClientEncode(string& status){
  //MCE_INFO("RestLogicManagerI::ClientEncode --> status = " << status);
  try{
    size_t pos = status.find("转自");
    if(string::npos == pos){
      return;
    }
    pos = status.find("&amp;");
    if(string::npos != pos){
      status = boost::regex_replace(status, boost::regex("&amp;"), string("&")); 
    }
    pos = status.find("&quot;");
    if(string::npos != pos){
      status = boost::regex_replace(status, boost::regex("&quot;"), string("\"")); 
    }
    pos = status.find("&#39;");
    if(string::npos != pos){
      status = boost::regex_replace(status, boost::regex("&#39;"), string("\\")); 
    }
    pos = status.find("&lt;");
    if(string::npos != pos){
      status = boost::regex_replace(status, boost::regex("&lt;"), string("<")); 
    }
    pos = status.find("&gt;");
    if(string::npos != pos){
      status = boost::regex_replace(status, boost::regex("&gt;"), string(">")); 
    }
  }catch(...){
    MCE_WARN("RestLogicManagerI::ClientEncode --> error  status = " << status);
  } 
}

//产品要求，过滤 “(shafa1[0-6]) (微笑)” 这样的客户端抢沙发按钮的回复 里面的自动回复的表情 (11.03.09 上午写好的代码，产品[康永乐]下午说不用了 555)
void RestLogicManagerI::EncodeReplyStatus(string& status){
	//MCE_INFO("RestLogicManagerI::EncodeReplyStatus --> status = " << status);
	try{
		size_t pos = status.find("(shafa");
		size_t pos1, pos2;
		if(string::npos == pos){
			return;
		}
		for(int i = 1; i <= 16; ++i){
			if(i <= 9){
				pos1 = 9;pos2 = 17;
			}else{
				pos1 = 10;pos2 = 18;
			}
			string sf = "(shafa" + boost::lexical_cast<string>(i) + ") ";
			string bq = sf + "(微笑)";
			pos = status.find(bq);
			if(string::npos != pos && !pos){
				status.erase(status.begin() + pos1, status.begin() + pos2);
				break;
			}
			bq = sf + "(害羞)";	
			pos = status.find(bq);
			if(string::npos != pos && !pos){
				status.erase(status.begin() + pos1, status.begin() + pos2);
				break;
			}
			bq = sf + "(调皮)";	
			pos = status.find(bq);
			if(string::npos != pos && !pos){
				status.erase(status.begin() + pos1, status.begin() + pos2);
				break;
			}
			bq = sf + "(谄笑)";	
			pos = status.find(bq);
			if(string::npos != pos && !pos){
				status.erase(status.begin() + pos1, status.begin() + pos2);
				break;
			}
			bq = sf + "(淘气)";	
			pos = status.find(bq);
			if(string::npos != pos && !pos){
				status.erase(status.begin() + pos1, status.begin() + pos2);
				break;
			}
		}
	}catch(...){
		MCE_WARN("RestLogicManagerI::EncodeReplyStatus --> error status = " << status);
	}
}


void RestLogicManagerI::setStatus(const JidPtr& jid, OnlineStat state, const string& status, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::setStatus --> " << jid->userId);
  string s = status;
  ClientEncode(s);
	RestTaskPtr req = new SetStatusTask(jid, state, s);
	addReq(req);
	try{
	TalkStatAdapter::instance().incSetStatusCount();
	}catch(Ice::Exception& e){
		MCE_WARN("RestLogicManagerI::setStatus-->TalkStatAdapter::incSetStatusCount-->" << e);
		}
}

void RestLogicManagerI::replyStatus(int userid, Ice::Long statusId, int ownerId, int replieduserId,
									const string& content, const Ice::Current&){
	//string s = content;
	//EncodeReplyStatus(s);
	MCE_DEBUG("RestLogicManagerI::replyStatus --> " << userid);
	RestTaskPtr req = new ReplyStatusTask(userid, statusId, ownerId, replieduserId, content);
	addReq(req);
}

void RestLogicManagerI::replyPhoto(int userid, Ice::Long photoId, int ownerId, int replieduserId,
							const string& content, int replyType, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::replyPhoto--> " << userid);
	RestTaskPtr req = new ReplyTask("photos.addComment", userid, photoId, ownerId, replieduserId, content, replyType);
	addReq(req);
}

void RestLogicManagerI::replyAlbum(int userid, Ice::Long albumId, int ownerId, int replieduserId,
							const string& content, int replyType, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::replyAlbum--> " << userid);
	RestTaskPtr req = new ReplyTask("photos.addAlbumComment", userid, albumId, ownerId, replieduserId, content, replyType);
	addReq(req);
}

void RestLogicManagerI::replyBlog(int userid, Ice::Long blogId, int ownerId, int replieduserId,
							const string& content, int replyType, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::replyBlog--> " << userid);
	RestTaskPtr req = new ReplyTask("blog.addComment", userid, blogId, ownerId, replieduserId, content, replyType);
	addReq(req);
}



void RestLogicManagerI::useImNotify(int userid, const Ice::Current&){
	return;
	MCE_DEBUG("RestLogicManagerI::useImNotify --> " << userid);
	RestTaskPtr req = new UseImNotifyTask(userid);
	addReq(req);
}

void RestLogicManagerI::activeUserNotify(int userid, const Ice::Current&){
	return;
  MCE_DEBUG("RestLogicManagerI::activeUserNotify --> " << userid);
	RestTaskPtr req = new ActiveUserNotifyTask(userid);
	addReq(req);
}

void RestLogicManagerI::activeUserNotify2(int userid, const Ice::Current&){
	return;
  MCE_DEBUG("RestLogicManagerI::activeUserNotify2 --> " << userid);
	RestTaskPtr req = new ActiveUserNotifyTask2(userid);
	addReq(req);
}

void RestLogicManagerI::sendNotice(int userid, const string& data, const MyUtil::IntSeq& userids, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::sendNotice --> " << userid);
	string tmp = boost::lexical_cast<string>(userid);
	//if(data.find(tmp) == string::npos){
	//	return;
	//}
	RestTaskPtr req = new SendNoticeTask(userid, data, const_cast<IntSeq&>(userids));
	addReq(req);
}

void RestLogicManagerI::sendLobbyNotice(int userid, const string& data, const MyUtil::IntSeq& userids, const Ice::Current&){
  MCE_DEBUG("RestLogicManagerI::sendNotice --> " << userid);
  string tmp = boost::lexical_cast<string>(userid);
  //if(data.find(tmp) == string::npos){$
  //return;$
  //}
  RestTaskPtr req = new SendNoticeLobbyTask(userid, data, const_cast<IntSeq&>(userids));
  addReq(req);
}



void RestLogicManagerI::notifyResult(Ice::Long reqId, const RestResultPtr& content, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::notifyResult --> req id " << reqId);
	RestTaskPtr task;
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		map<Ice::Long, Pos>::iterator it = _reqMap.find(reqId);
		if(it == _reqMap.end()){
			MCE_WARN("RestLogicManager::notifyResult --> the request had timeouted");
			return;
		}
		task = *(it->second);
		if(!task->validContent(content)){
			return;
		}
		_reqList.erase(it->second);
		_reqMap.erase(it);
	}
	task->result(content);
}
void RestLogicManagerI::getSessionKey(const JidPtr& jid, const Ice::Current&){
  MCE_DEBUG("RestLogicManagerI::getSessionKey--> jid =  " << jidToString(jid));
  int iqid = (int)time(NULL);
  RestTaskPtr req = new GetSessionKeyTask(jid, iqid);
  addReq(req);
}
void RestLogicManagerI::getSessionKey2(const JidPtr& jid, int iqid, const Ice::Current&){
	MCE_DEBUG("RestLogicManagerI::getSessionKey--> jid =  " << jidToString(jid) << " iqid = " << iqid);
	RestTaskPtr req = new GetSessionKeyTask(jid, iqid);
	addReq(req);
}
