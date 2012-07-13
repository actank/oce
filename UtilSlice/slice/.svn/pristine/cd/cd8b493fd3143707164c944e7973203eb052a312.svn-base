#include <Ice/BuiltinSequences.ice>

module com
{
module xiaonei
{
module xce
{
	const int CDbRServer = 1;
	const int CDbWServer = 2;
	const int CDbWRServer = 3;
	const int CDbSServer = 4;
	const string CDbInstanceSingler = "singler";
	const string CDbInstanceRouter = "router";
	
	exception XceException
	{
		string reason;
	};
	exception NoSuchConnectionPoolException extends XceException {};
	exception NoSuchConnectionException extends XceException {};
	exception ConnectionPoolFullException extends XceException {};
	exception ConnectFailException extends XceException {};
	exception QueryFailException extends XceException {};
		
	class DbServer
	{	
		string type;
		string database;
		string host;
		int port;		
		string charset;
		string user;
		string password;		
	};
	sequence<DbServer> DbServerSeq;
	
	class Route
	{
		string expression;
		string instance;
	};
	sequence<Route> RouteSeq;

	class DbInstance
	{		
		string name;
		int timestamp;	
		string type;
		
		DbServer wserver;			
		DbServerSeq rservers;
		
		RouteSeq routes;
	};

	sequence<DbInstance> DbInstanceSeq;
	dictionary<string, DbInstance> DbInstanceMap;
	
	class DbObserver
	{
		void updated(DbInstance instance);
    bool isMasterConnected(string host, int port);
	};
	sequence<DbObserver*> DbObserverSeq;
	dictionary<string, DbObserverSeq> DbObserverMap;
	
	class DbDescriptor
	{
		void reload();
		
		DbInstanceSeq getDbInstances();
		DbInstance getDbInstance(string name);
		
		void subscribe(string name, DbObserver* observer);
		void unsubscribe(string name, DbObserver* observer);
		
		DbObserverMap query(string name);
    void alert(string name, string dbinfo);
	};
};
};
};

