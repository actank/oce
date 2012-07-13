#include "../zombie/ZombieUserScanner.h"
#include "UserStateAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::userfilter;
using namespace com::xiaonei::xce;
using namespace xce::adapter::userstate;

int main(int argc, char** argv){
	unsigned min = 0;
	unsigned max = ALL_ID_COUNT;
	bool toSet = false;
	char c;
	/* process arguments */
	while (-1 != (c = getopt(argc, argv,
		"m:"	/* min id */
		"M:"	/* max id */
		"s"		/* set active */
	))) {
		switch (c) {
		case 'm':
			min = atoi(optarg);
			break;
		case 'M':
			max = atoi(optarg);
			break;
		case 's':
			toSet = true;
			break;
		default:
			break;
		}
	}

	ZombieUserScannerPtr zus = new ZombieUserScanner(min, max);
	if(toSet){
		zus->setActive();
	}

	return 0;
}

