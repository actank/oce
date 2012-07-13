#ifndef PASSPORT_ICE
#define PASSPORT_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module passport
{
	class User
	{
		int userId;
		int timestamp;
	};
	interface PassportManager
	{
		string createTicket(int userId);
		void destroyTicket(string ticket);
		int verifyTicket(string ticket);
		string queryTicket(int userId);
    };

};
};
};
};

#endif

