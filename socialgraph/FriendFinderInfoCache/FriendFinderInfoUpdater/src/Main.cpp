#include <iostream>
#include <FriendFinderInfoWorkerReplicaAdapter.h>

using namespace xce::friendfinder::adapter;
using namespace std;

int main(){
	try {
		//FriendFinderInfoWorkerReplicaAdapter::instance().restartControl();
		int userId = 238111132;  
		FriendFinderInfoWorkerReplicaAdapter::instance().createDataFromDB(userId);
	} catch( Ice::Exception e ) {
		cout << "FriendFinderInfoUpdater Ice Exception " << e.what() << endl;
	//	cout << "wait for half minute" << endl;
	//	sleep(30);    //暂停半分钟
	//	cout << "trigger calculating" << endl;

	} catch( ... ) {
		cout << "FriendFinderInfoUpdater Unknown Exception " << endl;
	}
}
