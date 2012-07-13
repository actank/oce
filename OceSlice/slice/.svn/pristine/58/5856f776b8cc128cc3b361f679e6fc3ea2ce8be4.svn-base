#ifndef __COMMENT_ICE__
#define __COMMENT_ICE__

module com{
module xiaonei{
module wService {
module slice{
	
	exception UGCCommentNewException{
		string msg;
	};
	
	class UGCCommentNew{
		long id;
		int fromId;
		string fromName;
		string fromHeadUrl;
		string body;
		int owner;
		long sourceId;
		int type;
		long time; 
		int whisper;
	};
	
	sequence<UGCCommentNew> CommentSeq;
	
	interface UGCCommentNewLogic{
		void addReply(int from,int owner,int type,long sourceId, int to,string body,bool notify) throws UGCCommentNewException;
		CommentSeq getReplySeq(int from,int owner,int type,long sourceId, int begin,int limit)  throws UGCCommentNewException;
		CommentSeq getFirstAndLastReply(int from,int owner,int type,long sourceId)  throws UGCCommentNewException;
		void remove(int owner,int type,long sourceId, long id)  throws UGCCommentNewException;
	};
};
};
};
};

#endif
