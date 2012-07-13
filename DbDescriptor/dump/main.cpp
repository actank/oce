/* 从DbDescriptor服务dump observer 信息
 * 
 *
 *
 *
 */

#include <vector>
#include <iostream>
#include "DbDescriptor.h"
#include "Channel.h"
#include "Ice/Exception.h"

using namespace std;
using namespace com::xiaonei::xce;

const string DUMPALL = "dumpall";

void dumpObserver(DbDescriptorPrx proxy, string name) {
	try {
		DbObserverMap obm = proxy->query(name);
		for(DbObserverMap::iterator itm = obm.begin(); itm != obm.end(); itm++) {
			cout << "Observer [" << name << "] size : " << itm->second.size() << endl;
			for(DbObserverSeq::iterator its = itm->second.begin(); its != itm->second.end(); its++) {
				cout << "Observer [" << name << "] info : " << (*its) << endl;
			}
		}
	} catch (const Ice::Exception& e) {
		cout << "Observer [" << name << "] Ice Exception : " << e.what() << endl;
	} catch (const exception& e) {
		cout << "Observer [" << name << "] Std Exception : " << e.what() << endl;
	} catch (...) {
		cout << "Observer [" << name << "] Unknow Exception" << endl;
	}
}

int main(int argc, char ** argv){
	vector<string> names;

	if(argc == 1){
		cout << "useage:" << endl;
		cout << "1. Dump all instances : " << argv[0] << " " << DUMPALL << endl;
		cout << "2. Dump specified instances,combine instances by space : " << argv[0] << " instance1 [...]" << endl;
		cout << "\nexample:" << endl;
		cout << argv[0] << " vipmember user_config buddycore_friend_a" << endl;
		return 0;
	} else if (argc >= 2) {
		string name = argv[1];
		if(name == DUMPALL) {
			names.push_back("");
		} else {
			for(int i=1; i<argc; i++) {
				names.push_back(argv[i]);
			}
		}
	} else {
		cout << "Argument error" << endl;
		return 0;
	}

	MyUtil::OceChannel channel;
	Ice::CommunicatorPtr ic = channel.getCommunicator();
	DbDescriptorPrx proxy = DbDescriptorPrx::checkedCast(ic->stringToProxy("DCS@DbDescriptor"));
	cout << "dumping..." << endl;
	for(vector<string>::iterator iv = names.begin(); iv < names.end(); iv++){
		if((*iv) == ""){
			DbInstanceSeq instances = proxy->getDbInstances();
			cout << "Instances size = " << instances.size() << endl;
			for(DbInstanceSeq::iterator it = instances.begin(); it != instances.end(); it++){
				dumpObserver(proxy, (*it)->name);	
			}
		} else {
			dumpObserver(proxy, (*iv));
		}
	}

	ic->destroy();
	cout << "done." << endl;
	return 0;
}
