#include "TalkRightAdapter.h"

using namespace com::xiaonei::talk::adapter;
using namespace MyUtil;
using namespace com::xiaonei::talk;

RightManagerPrx TalkRightAdapter::getManager(int id) {
	return locate<RightManagerPrx>(_managers, "M", id, TWO_WAY);
}

RightManagerPrx TalkRightAdapter::getManagerOneway(int id) {
	return locate<RightManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//bool TalkRightAdapter::verify(int from, int to, int type) {
//	return getManager(to)->verify(from, to, type);
//}

MyUtil::IntSeq TalkRightAdapter::batchVerify(const VerifySeq& seq) {
	map<int, VerifySeq> vMap;
	for (size_t i = 0; i < seq.size(); ++i) {
		vMap[abs(seq.at(i)->to) % _cluster].push_back(seq.at(i));
	}
	MyUtil::IntSeq res;
	for (map<int,VerifySeq>::iterator it = vMap.begin(); it != vMap.end(); ++it) {
		try {
			MyUtil::IntSeq tmp = getManager(it->first)->batchVerify(it->second);
			res.insert(res.end(),tmp.begin(),tmp.end());
		} catch(...) {
			for(size_t i = 0; i < it->second.size(); ++i){
				res.push_back(it->second.at(i)->index);
			}
		}
	}
	return res;
}

MyUtil::IntSeq TalkRightAdapter::batchVerify(int index, const VerifySeq& seq) {
	
	MyUtil::IntSeq res;
	
		try {
			res = getManager(abs(index) % _cluster)->batchVerify(seq);
			
		} catch(...) {
			for(size_t i = 0; i < seq.size(); ++i){
				res.push_back(seq.at(i)->index);
			}
		}
	
	return res;
}

//VerifyResultSeq TalkRightAdapter::batchVerifySysMsg(const VerifySysMsgSeq& seq) {
//	map<int, VerifySysMsgSeq> vMap;
//	for (size_t i = 0; i < seq.size(); ++i) {
//		vMap[abs(seq.at(i)->to) % _cluster].push_back(seq.at(i));
//	}
//	VerifySysMsgSeq res;
//	for (map<int,VerifySysMsgSeq>::iterator it = vMap.begin(); it != vMap.end(); ++it) {
//		try {
//			VerifyResultSeq tmp = getManager(it->first)->batchVerifySysMsg(it->second);
//			res.insert(res.end(),tmp.begin(),tmp.end());
//		} catch(...) {
//
//		}
//	}
//	return res;
//}

bool TalkRightAdapter::isHiddenUser(int userId) {
	return getManager(userId)->isHiddenUser(userId);
}

void TalkRightAdapter::addHiddenUser(int userId) {
	getManagerOneway(userId)->addHiddenUser(userId);
}

void TalkRightAdapter::removeHiddenUser(int userId) {
	getManagerOneway(userId)->removeHiddenUser(userId);
}

void TalkRightAdapter::reloadBlockUser(int userId) {
	getManagerOneway(userId)->reloadBlockUser(userId);
}

void TalkRightAdapter::modifySystemMessageBlock(int userId,
		const MyUtil::StrSeq& typeSeq) {
	getManagerOneway(userId)->modifySystemMessageBlock(userId, typeSeq);
}

//void TalkRightAdapter::removeSystemMessageBlock(int userId, const string& type) {
//	getManagerOneway(userId)->removeSystemMessageBlock(userId,type);
//}

MyUtil::StrSeq TalkRightAdapter::getSystemMessageBlock(int userId) {
	return getManager(userId)->getSystemMessageBlock(userId);
}

UserStatusPtr TalkRightAdapter::getUserStatus(int userId){
	return getManager(userId)->getUserStatus(userId);
}
void TalkRightAdapter::reloadUserStatus(int userId){
	getManagerOneway(userId)->reloadUserStatus(userId);
}

