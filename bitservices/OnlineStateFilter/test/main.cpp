#include <OnlineStateFilterAdapter.h>

using namespace std;
using namespace xce::onlinestatefilter::adapter;

bitset<ONLINESTATE_FILTER_SIZE> s;

int main(int argc, char* argv[]) {
	OnlineStateFilterAdapter::instance().getAllOnlineState(s);
	cout << "preload done" << endl;
	int i;
	while (cin >> i) {
		cout << i << " " << s[i] << endl;
	}
	return 0;
}

