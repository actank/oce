#ifndef MCE_MONITOR_ICE
#define MCE_MONITOR_ICE

#include <Util.ice>

module MceMonitor
{
	exception MonitorException extends MyUtil::MceException{};
	
	struct Resource
	{
		string name;
		string type;
		MyUtil::StrSeq parameters;
		int interval;
		float min;
		float max;
		int warningCount;
		int resumeCount;
		string warningMessage;
	};
	sequence<Resource> ResourceSeq;
	
	struct ResourceGroup
	{
		string name;
		ResourceSeq resource;
	};

	struct Log
	{
		string resource;
		string resourcegroup;
		string type;
		float value;
		int time;
	};

	interface MonitorManager
	{
		void addResourceGroup(ResourceGroup rg);
		void addLog(Log result);
	};
	
	interface ResourceManager
	{
		void addResourceGroup(ResourceGroup rg);
	};
	
	struct User
	{
		string name;
		string phone;
		string email;
		int level;
	};
	sequence<User> UserSeq;
	
	interface UserManager
	{
		UserSeq getUsers(int level);
		void addUser(User u);
	};
	
	interface AlertManager
	{
		void sendAlert(UserSeq to, string msg);
	};
};
#endif

