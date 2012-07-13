#ifndef TRANSFER_ICE
#define TRANSFER_ICE

module passport{
	interface TransferManager{
		string createTicket(int userId, int time);
		int verifyTicket(string ticket);
	};

};

#endif
