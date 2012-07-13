#ifndef __MASTER_SLAVE_SERVER_ICE__
#define __MASTER_SLAVE_SERVER_ICE__


#include <Util.ice>

module com
{
module xiaonei
{
module ms
{
	exception NotMasterBabaException extends MyUtil::MceException{};
	sequence<Object> ObjSeq;
	
	const int TYPEMASTERBABA = 0;
	const int TYPEMASTERMAMA = 1;
	const int TYPESLAVE = 2;	
	class ServerInfo
	{
		string name;
		Object* prx;
		int startTime;
		int now;
		int type; // 0 is active master , 1 is second master , 2 is slave
		bool isEnable;
	};
	
	sequence<ServerInfo> ServerInfoSeq;	
	dictionary<string,ServerInfo> ServerInfoMap;

	class MasterSlaveServer
	{	
		//ServerInfo info;
		
		ServerInfo masterHeartbeat(ServerInfo server);
		ServerInfo heartbeat(ServerInfoMap servers);
		void syncData(ObjSeq objs);	
		//void syncData(Object obj) throws NotMasterBabaException;
		
		ServerInfo regServer(ServerInfo server) throws NotMasterBabaException;
		//void unRegServer(ServerInfo server) throws NotMasterBabaException;
		void finishSyncData(int beginTime);		
	};

};
};
};

#endif

