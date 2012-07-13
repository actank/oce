/*
 * TransferManagerAdapter.h
 *
 *  Created on: 2010-5-21
 *      Author: ld
 */

#ifndef __PASSPORT_TRANSFERMANAGERADAPTER_H__
#define __PASSPORT_TRANSFERMANAGERADAPTER_H__

#include "AdapterI.h"
#include "Transfer.h"

namespace passport {

using namespace MyUtil;

class TransferManagerAdapter: public MyUtil::ReplicatedClusterAdapterI<TransferManagerPrx>,
    public MyUtil::Singleton<TransferManagerAdapter> {
public:

  TransferManagerAdapter() : MyUtil::ReplicatedClusterAdapterI<TransferManagerPrx> ("ControllerTransfer",120,300) {}

  string createTicket(int userId, int time);

  int verifyTicket(string ticket);



protected:

  TransferManagerPrx getManager();





};


}

#endif /* __PASSPORT_TRANSFERMANAGERADAPTER_H__ */
