#include "socialgraph/friendfinderworker/algorithm/commonfriendexecuter.h"

namespace xce {
namespace socialgraph {

void CommonFriendExecuter::GetComm(int userid, CommonFriendSeq& comm) {
	if (NULL != instance_) {
		instance_->GetComm(userid, comm);
	}
}

void CommonFriendExecuter::Display(const CommonFriendSeq& comm) {
	if (NULL != instance_) {
		instance_->Display(comm);
	}
}

}
}
