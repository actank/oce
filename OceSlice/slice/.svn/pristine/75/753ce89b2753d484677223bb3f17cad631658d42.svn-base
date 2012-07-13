#ifndef ACCOUNT_ICE
#define ACCOUNT_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
     
    exception RollbackException extends MyUtil::MceException{};

   	class Account
    {
        int xnb;
    };

	class Deposit//save money log
	{
		int id ;
		int fid;
		int uid;
		int rmb;
		int xnb;
		string timestamp;	
	};

	class Expenditure
	{
		int id;
		int uid;
		int xnb;
		string timestamp;
		string description;
		string productName;
		string productType;
	};
    
    class AccountManager
    {
    	void inc( Deposit dep) throws RollbackException;
        void dec( Expenditure exp) throws RollbackException;
        Account getAccount(int id);
    };
};
};
};
};
#endif

