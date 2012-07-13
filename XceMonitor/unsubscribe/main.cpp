#include <iostream>
#include <vector>
#include <string>
#include "XceMonitor.h"
#include "ServiceI.h"
#include "TopicI.h"

using namespace MyUtil;
using namespace Ice;
using namespace xce::storm;
using namespace xce::monitor;
using namespace std;

int main(int argc, char* argv[]) {
	if(argc < 2) {
		cout << "Usage: UnSubscribe xcetest1 xcetest2 \"xce test2\"" << endl;
		return 0;
	}
	vector<string> subjects;
	for(int i = 1; i < argc; ++i) {
		subjects.push_back(argv[i]);
	}
	TopicI topic("XceMonitorStorm", "LiveMonitor");
	XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrx::uncheckedCast(topic.getPublisher());
	monitor->disable(subjects);
	return 0;
}
