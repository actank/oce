#include <StatusCacheAdapter.h>

using namespace std;
using namespace xce::statuscache;

bitset<STATUS_CACHE_SIZE> s;

int main(int argc, char* argv[]) {
	StatusCacheAdapter::instance().getAllStatus(s);
	cout << "preload done" << endl;
	int i;
	while (cin >> i) {
		cout << i << " " << s[i] << endl;
	}
	return 0;
}

