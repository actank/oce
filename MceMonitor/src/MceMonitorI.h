#ifndef __MceMonitorI_h__
#define __MceMonitorI_h__

#include <MceMonitor.h>
#include "ItemBuilder.h"
#include "MonitorStatus.h"
#include "Singleton.h"
#include "ServiceI.h"

namespace MceMonitor
{

const static std::string MONITOR_MANAGER = "MM";
const static std::string RESOURCE_MANAGER = "RM";
const static std::string USER_MANAGER = "UM";
const static std::string ALERT_MANAGER = "SM";

class MonitorManagerI : virtual public MonitorManager, public MyUtil::Singleton<MonitorManagerI>
{
public:
    virtual void addResourceGroup(const ::MceMonitor::ResourceGroup&,
			     const Ice::Current& = Ice::Current());

    virtual void addLog(const ::MceMonitor::Log&,
			const Ice::Current& = Ice::Current());
	void initialize();

	MonitorManagerI();
private:
	friend class MyUtil::Singleton<MonitorManagerI>;
	MonitorStatus* _status;
	std::vector<IceUtil::ThreadPtr> _threads;
	
public:
	MonitorManagerPrx getMonitorManager();
	UserManagerPrx getUserManager();
	AlertManagerPrx getAlertManager();
private:
	MonitorManagerPrx _monitorManager;
	UserManagerPrx _userManager;
	AlertManagerPrx _alertManager;
};

class ResourceManagerI : virtual public ResourceManager
{
public:
	ResourceManagerI(const ItemBuilder&);
    virtual void addResourceGroup(const ::MceMonitor::ResourceGroup&,
			     const Ice::Current& = Ice::Current());
private:
	ItemBuilder _builder;
	std::vector<IceUtil::ThreadPtr> _threads;

};

class UserManagerI : virtual public UserManager
{
public:

    virtual ::MceMonitor::UserSeq getUsers(::Ice::Int,
					   const Ice::Current& = Ice::Current());

    virtual void addUser(const ::MceMonitor::User&,
			 const Ice::Current& = Ice::Current());
private:
	UserSeq _users;
};

class AlertManagerI : virtual public AlertManager
{
public:

    virtual void sendAlert(const ::MceMonitor::UserSeq&,
			 const ::std::string&,
			 const Ice::Current& = Ice::Current());
private:
	void sendEmail(const ::MceMonitor::UserSeq&, const ::std::string&);
	void sendSMS(const ::MceMonitor::UserSeq&, const ::std::string&);
};

}

#endif
