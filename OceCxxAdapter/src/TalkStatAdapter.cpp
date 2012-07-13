#include "TalkStatAdapter.h"

using namespace talk::stat;
using namespace com::xiaonei::talk::common;

StatManagerPrx TalkStatAdapter::getManager(){
	if(!_managerOneway){
		_managerOneway = locate<StatManagerPrx>("M", ONE_WAY);
	}
	return _managerOneway;
}

//void TalkStatAdapter::initialize() {
//	getManager() = locate<StatManagerPrx>("M", ONE_WAY);
//}

void TalkStatAdapter::login(const MyUtil::IntSeq& ids) {
	getManager()->login(ids);
}
void TalkStatAdapter::wLogin(const MyUtil::IntSeq& ids) {
	getManager()->wLogin(ids);
}

void TalkStatAdapter::totalMsgCount(int count) {
	getManager()->totalMsgCount(count);
}
void TalkStatAdapter::chatingMsgCount(int count) {
	getManager()->chatingMsgCount(count);
}
void TalkStatAdapter::msgStat(const JidPtr& from, const JidPtr& to) {
	getManager()->msgStat(from,to);
}
void TalkStatAdapter::versionStat(int userId, const string& ver, const string& subver){
	getManager()->versionStat(userId, ver, subver);
}
void TalkStatAdapter::mucMsgCount(int msgCount, int msgBroadcastCount){
	getManager()->mucMsgCount(msgCount, msgBroadcastCount);
}

void TalkStatAdapter::incSetStatusCount(int c){
	getManager()->incSetStatusCount(c);
}
void TalkStatAdapter::incReplyStatusCount(int c){
	getManager()->incReplyStatusCount(c);
}
void TalkStatAdapter::incReplyPhotoCount(int c){
	getManager()->incReplyPhotoCount(c);
}
void TalkStatAdapter::incReplyBlogCount(int c){
	getManager()->incReplyBlogCount(c);
}
void TalkStatAdapter::incReplyShareCount(int c){
	getManager()->incReplyShareCount(c);
}
