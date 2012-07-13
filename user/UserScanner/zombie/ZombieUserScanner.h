#ifndef _ZOMBIE_USER_SCANNER_H_
#define _ZOMBIE_USER_SCANNER_H_

#include "../../UserFilter/src/UserFilter.h"
#include "UserStateAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::userfilter;
using namespace com::xiaonei::xce;
using namespace xce::adapter::userstate;

namespace xce {
namespace userfilter{
class ZombieUserScanner : virtual public IceUtil::Shared{
public:
	ZombieUserScanner(unsigned min=0, unsigned max=ALL_ID_COUNT);
	~ZombieUserScanner();
	void setActive();

private:
	void loadHistoryData();
	void loadNewData();
	bool fileToMap();
	bool mapToFile();
	string getFilePath();

	UserBitMap* _historyBitMap;
	UserBitMap* _newBitMap;
	unsigned _minId;
	unsigned _maxId;
};
typedef IceUtil::Handle<ZombieUserScanner> ZombieUserScannerPtr;
}
}

#endif
