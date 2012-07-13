#ifndef XCE_ACCOUNT_ACCOUNT_ADAPTER_H__
#define XCE_ACCOUNT_ACCOUNT_ADAPTER_H__

#include "arch/adapter2.h"
#include "account/ticket.h"

namespace xce {

inline passport::TicketManagerPrx CreateTicketAdapter(int uid) {
  return Adapter::passport::Create<passport::TicketManagerPrx>("M@Ticket", uid % 10);
}

inline passport::TicketManagerPrx CachedTicketAdapter(int uid) {
  return Adapter::passport::Cached<passport::TicketManagerPrx>("M@Ticket", uid % 10, 10);
}

}
#endif // XCE_ACCOUNT_ACCOUNT_ADAPTER_H__
