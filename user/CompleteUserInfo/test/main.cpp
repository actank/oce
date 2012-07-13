#include "CompleteUserInfoAdapter.h"
#include <vector>
#include "GetUserPassportInfo.h"

using namespace std;
using namespace xce::adapter::completeuserinfo;

int main(int argc, char* argv[]) {
	int testId = 832541251;
	vector<bool> result(USER_DETAIL_SIZE, true);
	getUserPassportInfo(testId, result);
	show_error_info(result, info, testId);
	cout << "----------------------------------------------" << endl;
	fill(result.begin(), result.end(), true);
	vector<int> userIds;
	userIds.push_back(testId);
	CompleteUserInfoAdapter::instance().completeUserInfo(userIds);
	getUserPassportInfo(testId, result);	
	show_error_info(result, info, testId);
	cout << "----------------------------------------------" << endl;
	fill(result.begin(), result.end(), true);
	UserPassportInfoPtr ptr = new UserPassportInfo;
	ptr->set_safestatus(198);
	ptr->set_account("shangmin_gaolei");
	ptr->set_domain("www.cas.cn");
	ptr->set_id(testId);
	UserPassportAdapter::instance().createUserPassport(testId, ptr);
	CompleteUserInfoAdapter::instance().completeUserInfo(userIds);
	getUserPassportInfo(testId, result);	
	show_error_info(result, info, testId);
	return 0;
}
