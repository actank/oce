#include "UgcCommentAdapter.h"
//enum ReplyType{BLOG_REPLY = 1, ALBUM_REPLY = 2, PHOTO_REPLY = 3, STATUS_REPLY = 4, SHARE_REPLY = 5};
using namespace xce::ucgcomment;

UGCCommentNewLogicPrx UgcCommentAdapter::getManager(int id) {
  return locate<UGCCommentNewLogicPrx>(_managers, "M", id, TWO_WAY, 500);
}

UGCCommentNewLogicPrx UgcCommentAdapter::getManagerOneway(int id) {
  return locate<UGCCommentNewLogicPrx>(_managersOneway, "M", id, ONE_WAY, 300);
}

void UgcCommentAdapter::addReply(int from, int owner, ReplyType type, Ice::Long sourceId, int to, const string& body) {
	/*
	if(SHARE_REPLY == type){
		return getManager(3)->addReply(from,owner,type,sourceId,to,body, true);
	}
	else{
		return getManager(from % 3)->addReply(from,owner,type,sourceId,to,body, true);
	}
	*/
	getManager(from)->addReply(from,owner,type,sourceId,to,body, true);
}

CommentSeq UgcCommentAdapter::getReplySeq(int from, int owner, ReplyType type,
		Ice::Long sourceId, int begin, int limit) {
	/*
	if(SHARE_REPLY == type){
		return getManager(3)->getReplySeq(from,owner,type,sourceId,begin,limit);
	}
	else{
		return getManager(from % 3)->getReplySeq(from,owner,type,sourceId,begin,limit);
	}
	*/
	return getManager(from)->getReplySeq(from,owner,type,sourceId,begin,limit);
}










CommentSeq UgcCommentAdapter::getFirstAndLastReply(int from, int owner,
		ReplyType type, Ice::Long sourceId) {
	return getManager(from)->getFirstAndLastReply(from,owner,type,sourceId);
}

bool UgcCommentAdapter::remove(int owner, ReplyType type, Ice::Long sourceId,
		Ice::Long id) {
	getManager(owner)->remove(owner,type,sourceId,id);
}
