#ifndef __BUDDY_HOT_PRELOADER_H__
#define __BUDDY_HOT_PRELOADER_H__

#include <IceUtil/IceUtil.h>

namespace xce {
namespace buddy {

class BuddyHotPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<BuddyHotPreloader> {
public:
	BuddyHotPreloader() {}
	virtual ~BuddyHotPreloader() {}
};

}
}
#endif
