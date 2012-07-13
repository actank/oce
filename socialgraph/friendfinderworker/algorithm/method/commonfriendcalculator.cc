#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"

namespace xce {
namespace socialgraph {

void CommonFriendCalculator::GetComm(int userid, CommonFriendSeq& comm) {
	std::ostringstream oss;
	oss << "[CommonFriendCalculator] GetComm userid(" << userid << ")";
	PTIME(pt, oss.str(), true, 0);

	userid_ = userid;
	Initialize(userid_);

	if (Hook()) {
		CalComm(comm);
	}
}

void CommonFriendCalculator::Initialize(int userid) {
	try {
		//com::xiaonei::xce::ConnectionPoolManager::instance().initialize();

		//获取用户好友列表
		RelationBatchHelper::GetFriendSeq(userid, friendlist_);

		//获取用户发过好友申请的列表
		RelationBatchHelper::GetApplySeq(userid, applylist_);

		//获取用户黑名单
		RelationBatchHelper::GetBlockSeq(userid, blocklist_);

		//获取用户删除过的推荐
		RelationBatchHelper::GetCommonFriendsBlock(userid, commonblock_);
		
		//获取好友的好友
		RelationBatchHelper::GetFriendSeqBatch(friendlist_, ffmap_);

		//获取发过申请的用户的好友
		RelationBatchHelper::GetFriendSeqBatch(applylist_, afmap_);
	} catch (Ice::Exception& e) {
		std::cout << "[CommonFriendCalculator] Initialize ice exception " << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << "[CommonFriendCalculator] Initialize std exception " << e.what() << std::endl;
	} catch (...) {
		std::cout << "[CommonFriendCalculator] Initialize unknown exception " << std::endl;
	}
}

}
}
