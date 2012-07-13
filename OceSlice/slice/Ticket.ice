#ifndef TICKET_ICE
#define TICKET_ICE

module passport
{
	interface TicketReloader
	{
		string createTicket(int userId, string infos);
		bool destroyTicket(string ticket);
		int verifyTicket(string ticket, string infos);
		string queryTicket(int userId);
    };

	interface TicketManager
	{
		string createTicket(int userId, string infos);
		bool destroyTicket(string ticket);
		int verifyTicket(string ticket, string infos);
		string queryTicket(int userId);
    };

};

#endif

