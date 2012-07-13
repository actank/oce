/*
 * TransferManagerAdapter.cpp
 *
 *  Created on: 2010-5-21
 *      Author: ld
 */

#include "TransferManagerAdapter.h"

namespace passport {




string TransferManagerAdapter::createTicket(int userId, int time) {
	return getManager()->createTicket(userId, time);
}

int TransferManagerAdapter::verifyTicket(string ticket){
  return getManager()->verifyTicket(ticket);
}


TransferManagerPrx TransferManagerAdapter::getManager() {
  return getProxy(0);
}


}
