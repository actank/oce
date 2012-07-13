#include <iostream>
#include "FriendFinderByActiveUserWorkerAdapter.h"
using namespace std;
using namespace xce::friendfinder::adapter;

int main(){
	try {
		FriendFinderByActiveUserWorkerAdapter::instance().load();
	} catch ( Ice::Exception e ) {
		cout << "FriendFinderByActiveUserWorkerAdapter Ice Exception " << e.what() << endl;
	} catch ( ... ) {
		cout << "FriendFinderByActiveUserWorkerAdapter Unknown Exception " << endl;
	}
}
