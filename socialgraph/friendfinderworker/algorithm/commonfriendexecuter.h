#ifndef COMMON_FRIEND_EXECUTER_H_
#define COMMON_FRIEND_EXECUTER_H_

#include "socialgraph/friendfinderworker/algorithm/method/commonfriendcalculator.h"

namespace xce {
namespace socialgraph {

class CommonFriendExecuter {
public:
	CommonFriendExecuter() {}
	~CommonFriendExecuter() {}

	void SetCommonFriendInstance(CommonFriendCalculatorPtr instance) {
		instance_ = instance;
	}

	void GetComm(int userid, CommonFriendSeq& comm);	
	void Display(const CommonFriendSeq& comm);

private:
	//真正执行对象
	CommonFriendCalculatorPtr instance_;
};

}
}

#endif
