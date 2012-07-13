#include "RestLogicAdapter.h"
#include "IceLogger.h"
using namespace talk::rest;

RestLogicManagerPrx RestLogicAdapter::getManagerOneway(int id) {
	return locate<RestLogicManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

void RestLogicAdapter::setStatus(const JidPtr& jid, OnlineStat state, const string& status){
	if(!jid){
		return;
	}
	getManagerOneway(jid->userId)->setStatus(jid, state, status);
}

void RestLogicAdapter::replyStatus(int userid, Ice::Long statusId, int ownerId, int replieduserId, const string& content){
	getManagerOneway(userid)->replyStatus(userid, statusId, ownerId, replieduserId, content);
}
void RestLogicAdapter::replyPhoto(int userid, Ice::Long resId, int ownerId, int replieduserId, const string& content, int type){
	getManagerOneway(userid)->replyPhoto(userid, resId, ownerId, replieduserId, content, type);
}
void RestLogicAdapter::replyAlbum(int userid, Ice::Long resId, int ownerId, int replieduserId, const string& content, int type){
	getManagerOneway(userid)->replyAlbum(userid, resId, ownerId, replieduserId, content, type);
}
void RestLogicAdapter::replyBlog(int userid, Ice::Long resId, int ownerId, int replieduserId, const string& content, int type){
	getManagerOneway(userid)->replyBlog(userid, resId, ownerId, replieduserId, content, type);
}

void RestLogicAdapter::useImNotify(int userid){
	getManagerOneway(userid)->useImNotify(userid);
}

void RestLogicAdapter::activeUserNotify(int userid){
	getManagerOneway(userid)->activeUserNotify(userid);
}

void RestLogicAdapter::activeUserNotify2(int userid){
	getManagerOneway(userid)->activeUserNotify2(userid);
}

void RestLogicAdapter::notifyResult(Ice::Long reqId, const RestResultPtr& res){
	int index = ((reqId >> 32)&0xffffffff);
	MCE_WARN("RestLogicAdapter::notifyResult --> " << reqId << " index:" << index);
	getManagerOneway(index)->notifyResult(reqId, res);
}

void RestLogicAdapter::getSessionKey(const JidPtr& jid){
	getManagerOneway(jid->userId)->getSessionKey(jid);
}
void RestLogicAdapter::getSessionKey(const JidPtr& jid, int iqid){
  getManagerOneway(jid->userId)->getSessionKey2(jid, iqid);
}

