#ifndef _ACTIVE_USER_H__
#define _ACTIVE_USER_H__

#include <Ice/Ice.h>
#include "ServiceI.h"
#include "ActiveUser.h"
#include "EvictorTemplate.h"

namespace mop {

namespace hi {

namespace svc {

namespace model {
using namespace std;
using namespace MyUtil;

const static string ACTIVE_USERER = "AU";

//-----------------------------------------------------------------------------
class ActiveUserManagerI : virtual public ActiveUserManager {
public:
	ActiveUserManagerI();
	virtual ~ActiveUserManagerI();

	virtual Int2IntMap checkActiveUsers(const IntSeq& userIds,
			const Ice::Current&);
	virtual IntSeq
			filterActiveUsers(const IntSeq& userIds, const Ice::Current&);
	virtual void addActiveUsers(const IntSeq& userIds, const Ice::Current&);
	virtual void removeActiveUsers(const IntSeq& userIds, const Ice::Current&);
	virtual Ice::Int getActiveUserSize(const Ice::Current&);

private:
	typedef TimedEvictor<int, int> ActiveUserEvictor;
	ActiveUserEvictor _activeUserIds;
	int _minSize;

	class LoadActiveUsersTask : public Task {
public:
		LoadActiveUsersTask(ActiveUserEvictor& activeUserIds,
				const string& table) :
			_activeUserIds(activeUserIds), _table(table) {
		}
		;

		virtual ~LoadActiveUsersTask() {
		}
		;

		virtual void handle();
private:
		ActiveUserEvictor& _activeUserIds;
		string _table;
	};

	string _table;
};
}
;
}
;
}
;
}
;

#endif
