#include <iostream>
#include "socialgraph/friendfinderworker/algorithm/commonfriendexecuter.h"
#include "socialgraph/friendfinderworker/algorithm/commonfactory.h"

int main(int argc, char* argv[]) {
	using namespace xce::socialgraph;
	int userid = 238111132;
	CommonFriendSeq comm;

	CommonFriendExecuter executer;
	executer.SetCommonFriendInstance(CommonFactory::GetInstance().Create(HashMap));
	executer.GetComm(userid, comm);
	executer.Display(comm);
}
