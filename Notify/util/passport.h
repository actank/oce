#ifndef _XCE_NOTIFY_PASSPORT_H_
#define _XCE_NOTIFY_PASSPORT_H_

#include <IceUtil/Mutex.h>
#include "Evictor.h"
#include "Singleton.h"

namespace xce {
namespace notify {

class TicketPassport: public MyUtil::Singleton<TicketPassport> {
public:
	int Verify(const std::string & ticket);
private:
	typedef MyUtil::SizedEvictor<std::string, int> TicketCache;
	TicketCache cache_;
};

}
}

#endif //_XCE_NOTIFY_PASSPORT_H_
