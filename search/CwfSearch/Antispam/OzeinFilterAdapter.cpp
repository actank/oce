#include <Ice/Ice.h>
#include "OzeinFilterAdapter.h"
#include "FilterQuery.h"
#include "FilterAnswer.h"
#include "UserActivityClient.h"
#include "base/logging.h"

using namespace MyUtil;
using namespace com::xiaonei::ozein::content::generation;
using namespace com::xiaonei::ozein::activity::generation;
using namespace std;

FilterPrx OzeinFilterClient::getManager(int id) {
	return locate<FilterPrx> (_managers, "filter", id, TWO_WAY);
}

FilterPrx OzeinFilterClient::getManagerOneway(int id) {
	return locate<FilterPrx> (_managersOneway, "filter", id, ONE_WAY);
}


CheckResultMore OzeinFilterClient::checkerAndFilter(int fromUserId, int toId, int type, const vector<string>& contentV, const string& userIp) {
//  LOG(INFO) << "OzeinFilterAdapter contentV.size:" << contentV.size();
	if (contentV.size() == 0) {
		return CheckResultMore(CheckResult::PROHIBITED, contentV, contentV, contentV);
	}
	ostringstream oss;
	for(vector<string>::const_iterator it=contentV.begin();it!=contentV.end();++it){
		oss<<*it;
	}
//  LOG(INFO) << "OzeinFilterAdapter " << oss.str();
	int activity = UserActivityClient::instance().checkActivity(fromUserId,	toId, type, oss.str());
//	int activity = 0;
	// 行为检测不通过
	if (activity > 0) {
	    return CheckResultMore(CheckResult::PROHIBITED, contentV);
	}


	FilterQuery query(fromUserId, type, contentV);
	query.addToId(toId);
	query.setIp(userIp);

	FilterAnswer answer = ozeinContent(query);
	CheckResultMore result = answer.toCheckResultMore(contentV);
//	result.setAuditFirst(answer.isAuditFirst());
//	result.setNotNotify(answer.isNotNotify());
	return result;
}

FilterAnswer OzeinFilterClient::ozeinContent(FilterQuery query) {
	map<string, vector<string> > answerMap;
	answerMap = getManager(0)->ozeinContent(query.getMap());
	return FilterAnswer(answerMap);
}

