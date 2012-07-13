
#ifndef UGC_COMMENT_ADAPTER_H
#define UGC_COMMENT_ADAPTER_H

#include "AdapterI.h"
#include "Singleton.h"
#include "UgcComment.h"

namespace xce {
namespace ucgcomment {

using namespace MyUtil;
using namespace com::xiaonei::wService::slice;

enum ReplyType{BLOG_REPLY = 1, ALBUM_REPLY = 2, PHOTO_REPLY = 3, STATUS_REPLY = 4, SHARE_REPLY = 5};

class UgcCommentAdapter: public MyUtil::AdapterI,
		//public AdapterISingleton<MyUtil::WServiceUGCChannel, UgcCommentAdapter> {
		public AdapterISingleton<MyUtil::ImUgcChannel, UgcCommentAdapter> {
public:
	UgcCommentAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

	void addReply(int from,int owner,ReplyType type,Ice::Long sourceId, int to, const string&  body);
	CommentSeq getReplySeq(int from,int owner,ReplyType type,Ice::Long sourceId, int begin,int limit);
	CommentSeq getFirstAndLastReply(int from,int owner,ReplyType type,Ice::Long sourceId);
	bool remove(int owner,ReplyType type,Ice::Long sourceId, Ice::Long id);

protected:
	virtual string name() {
		return "UGCCommentLogic";
	}
	virtual string endpoints() {
		return "@UGCCommentLogic";
	}
	virtual size_t cluster() {
		return 5;
	}

	UGCCommentNewLogicPrx  getManager(int id);
	UGCCommentNewLogicPrx  getManagerOneway(int id);

	vector<UGCCommentNewLogicPrx>  _managersOneway;
	vector<UGCCommentNewLogicPrx>  _managers;
};

}
}
#endif
