#include <Ice/Ice.h>
#include "OzeinFilterAdapter.h"
#include "FilterQuery.h"
#include "FilterAnswer.h"

using namespace MyUtil;
using namespace com::xiaonei::ozein::content::generation;
using namespace std;

FilterPrx OzeinFilterClient::getManager(int id) {
	return locate<FilterPrx> (_managers, "filter", id, TWO_WAY);
}

FilterPrx OzeinFilterClient::getManagerOneway(int id) {
	return locate<FilterPrx> (_managersOneway, "filter", id, ONE_WAY);
}


CheckResultMore OzeinFilterClient::checkerAndFilter(int fromUserId, int toId, int type, const vector<string>& contentV, const string& userIp) {
	if (contentV.size() == 0) {
		return CheckResultMore(0, contentV, contentV, contentV);
	}
	FilterQuery query(fromUserId, type, contentV);
	query.addToId(toId);
	FilterAnswer answer = ozeinContent(query);
	CheckResultMore result = answer.toCheckResultMore(contentV);
	result.setAuditFirst(answer.isAuditFirst());
	result.setNotNotify(answer.isNotNotify());
	return result;
}

FilterAnswer OzeinFilterClient::ozeinContent(FilterQuery query) {
	map<string, vector<string> > answerMap;
	answerMap = getManager(0)->ozeinContent(query.getMap());
	return FilterAnswer(answerMap);
}

