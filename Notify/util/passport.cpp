#include "passport.h"
#include "TicketAdapter.h"

namespace xce {
namespace notify {

int TicketPassport::Verify(const string & ticket) {
	int uid = cache_.find(ticket, -1);
	if (uid > 0) {
		return uid;
  }

	try {
		uid = passport::TicketAdapter::instance().verifyTicket(ticket, "");
	} catch (Ice::Exception& e) {
		MCE_WARN("Ticket " << ticket << " verify error : " << e);
	}
	if (uid > 0) {
		cache_.add(uid, ticket);
  }
	return uid;
}

}
}
