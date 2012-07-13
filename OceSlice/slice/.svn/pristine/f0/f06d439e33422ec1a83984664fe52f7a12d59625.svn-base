#ifndef FOOTPRINT_VIEW
#define FOOTPRINT_VIEW

#include <Util.ice>

module xce {
module footprint {

const int STATELIMIT = 1;
const int STATESPAMMER = 2;

class VisitCountInfo {
	int timestamp;
	int count;
	int state;
};
["java:type:java.util.ArrayList"] sequence<VisitCountInfo> VisitCountInfoSeq;

class FootprintViewManager {
	bool verifyVisitCount(MyUtil::Int2IntMap counts);
	VisitCountInfoSeq getVisitCountHistory(int id);
	VisitCountInfo getRecentVisitCount(int id);
	void setVisitCountLimit(int id);
	void setVisitCountState(int id, int state);
};

};
};

#endif
