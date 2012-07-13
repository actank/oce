#ifndef LOGIN_ICE
#define LOGIN_ICE

module passport
{
	class AccountInfo{
		int success;
		int userId;
		string account;
		int status;
		string domain;
		string ticket;
	};
	sequence<AccountInfo> AccountInfoSeq;
	enum AccountType
	{
		Account,Phone,Id,BindedId
	};	
	sequence<AccountType> AccountTypeSeq;

	interface LoginLogic {
		["amd"] AccountInfoSeq login(AccountTypeSeq types, string name, string pwd, string site, int expire, bool reuse);
    ["amd"] AccountInfoSeq loginDefaultAccount(AccountTypeSeq types, string name, string pwd, string site, int expire, bool reuse);
    ["amd"] AccountInfoSeq loginVirtual(AccountTypeSeq types, string name, string pwd, string site, int expire, bool reuse);
		string createTicket(int id, int expire, bool reuse);

		bool has(AccountType type, string name);
		bool hasWithTypes(AccountTypeSeq types, string name);
		AccountInfoSeq get(AccountType type, string name);
		AccountInfoSeq getWithTypes(AccountTypeSeq types, string name);
		AccountInfoSeq getById(int id);

		AccountInfo id2Account(int id);

    AccountInfo switchAccount(int id, string pwd, bool reuse);
    AccountInfo switchAccountIncVirtualId(int sourceId, int desId, string pwd, bool reuse);
	};
	
	interface LoginCache {
		void set(int uid,string ticket,int second);

		bool getStatus(int userId);
		void setStatus(int userId);

		bool destroyByTicket(string ticket);
		bool destroyById(int userId);

		int ticket2Id(string ticket);
		string id2Ticket(int userId);
	};
};
#endif

