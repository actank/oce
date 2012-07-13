#include <iostream>
#include <vector>
#include <stdio.h>
#include "XceAlertAdapter.h"

using namespace std;
using namespace com::xiaonei::xce;

int main(int argc, char* argv[]) {
	if(argc < 4) {
		cout << "Error, correct usage is : ./sendAlert 1 \"UserCountAdapter.getAll BuddyCoreAdapterImpl.getRelation\" \"UserCountAdapter error happened\"" << endl;
		return 0;
	}

	XceAlertAdapter adapter;
	vector<string> typeseq;
	istringstream in(argv[2]);
	string subject;
	while(in >> subject) {
		typeseq.push_back(subject);
	}
	int sendType = atoi(argv[1]);
	string msg(argv[3]);
	adapter.notifyAlertN(typeseq, msg, sendType);
	return 0;
}
