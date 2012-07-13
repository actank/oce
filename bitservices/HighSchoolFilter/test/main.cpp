#include <PhotoStateFilterAdapter.h>

using namespace std;
using namespace xce::photostatefilter::adapter;

bitset<PHOTOSTATE_FILTER_SIZE> s;

int main(int argc, char* argv[]) {
	int userid = 100;
	if( PhotoStateFilterAdapter::instance().isPhotoStateTrue(userid) ){
		cout << "check " << userid << " now is true!" << endl;
	} else{
		cout << "check " << userid << " now is false!" << endl;
	}
	PhotoStateFilterAdapter::instance().updatePhotoState(userid,"1test");
	if( PhotoStateFilterAdapter::instance().isPhotoStateTrue(userid) ){
		cout << "set " << userid << " \"1test\" success!" << endl;
	} else{
		cout << "set " << userid << " \"1test\" failed!" << endl;
	}
	PhotoStateFilterAdapter::instance().updatePhotoState(userid,"0test");
	if( PhotoStateFilterAdapter::instance().isPhotoStateTrue(userid) ){
		cout << "set " << userid << " \"0test\" failed!" << endl;
	} else{
		cout << "set " << userid << " \"0test\" success!" << endl;
	}
	cout << "preloading ..." << endl;
	PhotoStateFilterAdapter::instance().getAllPhotoState(s);
	cout << "preload done" << endl;
	int i;
	while (cin >> i) {
		cout << i << " " << PhotoStateFilterAdapter::instance().isPhotoStateTrue(i) << " " << s[i] << endl;
	}
	return 0;
}

