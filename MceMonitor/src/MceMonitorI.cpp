#include "MceMonitorI.h"
#include "MonitorThread.h"
#include "Parser.h"
#include <boost/lexical_cast.hpp>
#include "ServiceI.h"
#include "String.h"
#include "converter.h"
#include "jwsmtp/jwsmtp.h"

using namespace ::MceMonitor;
using namespace std;
using namespace ::MyUtil;

void
MyUtil::initialize() {
	MonitorManagerI::instance().initialize();
}

MceMonitor::MonitorManagerI::MonitorManagerI() {
}

void
MceMonitor::MonitorManagerI::initialize(){
	MCE_DEBUG("MceMonitorServiceI initialize.");
	ServiceI& service = ServiceI::instance();
	string config_file = service.getCommunicator()->getProperties()->getPropertyWithDefault("MceMonitorCfg.Path",
			"../etc/MceMonitorCfg.xml");
	MCE_DEBUG("Using MceMonitorCfg.Path = " << config_file);

	DocumentPtr doc = Parser::parse(config_file);
	NodeList resources = doc->getChildren().at(0)->getChildren();
	MCE_DEBUG(resources.size());

	NodeList::iterator it_res = resources.begin();
	NodePtr node = *it_res;

	while (node->getName() == "") {
		// should be a bug of Parser
		++it_res;
		node = *it_res;
	};

	if (node->getName() != "Monitor") {
		MCE_FATAL("First element under root must be Monitor. but current is "
				<< node->getName() );
		//assert(false);
	};
	string monitorName = node->getAttributes()["Hostname"];

	char buff[255];
	gethostname(buff, 255);
	string hostname(buff);
	MCE_DEBUG("current hostname is "<< hostname);
	{
		// judgement of service name and hostname must be the same.
		ostringstream tmp;
		tmp << "MceMonitor_" << hostname;
		MCE_FATAL("assert " << tmp.str() << " == " << service.getName() );
		assert(tmp.str() == service.getName());
	}
	_status = new MonitorStatus();
	bool isMonitorNode = false;
	if (hostname == monitorName) {
		// This is a Monitor node;
		MCE_INFO("This is a monitor node.");
		_monitorManager = MonitorManagerPrx::uncheckedCast(service.getAdapter()->add(this, service.getCommunicator()->stringToIdentity(MONITOR_MANAGER)) );
		_userManager = UserManagerPrx::uncheckedCast(service.getAdapter()->add(new UserManagerI, service.getCommunicator()->stringToIdentity(USER_MANAGER)) );
		_alertManager = AlertManagerPrx::uncheckedCast(service.getAdapter()->add(new AlertManagerI, service.getCommunicator()->stringToIdentity(ALERT_MANAGER)) );
		isMonitorNode = true;
	} else {
		MCE_INFO("This is not a monitor node.");
		_monitorManager
				= MonitorManagerPrx::uncheckedCast(service.getCommunicator()->stringToProxy(MONITOR_MANAGER + "@MceMonitor_"
						+ monitorName));
		_userManager
				= UserManagerPrx::uncheckedCast(service.getCommunicator()->stringToProxy(USER_MANAGER + "@MceMonitor_"
						+ monitorName));
		_alertManager
				= AlertManagerPrx::uncheckedCast(service.getCommunicator()->stringToProxy(ALERT_MANAGER + "@MceMonitor_"
						+ monitorName));
	}
	ItemBuilder itemBuilder(service.getCommunicator(), hostname);
	ResourceManagerI* resourceManager = new ResourceManagerI(itemBuilder);
	service.getAdapter()->add(resourceManager, service.getCommunicator()->stringToIdentity(RESOURCE_MANAGER));
	//cout << "getting ItemBuilder" << endl;
	//ResourceManagerPrx resourceManager = 
	//	_channel->uncheckedCast<ResourceManagerPrx>(_channel->locate(
	//	getCommunicator()->stringToIdentity(RESOURCE_MANAGER)
	//	));

	for (++it_res; it_res != resources.end(); ++it_res) {
		node = *it_res;
		if (node->getName() == "") {
			// should be a bug of Parser
			continue;
		};
		if (node->getName() == "ResourceGroup") {
			// This is a Resource node;
			MCE_DEBUG("ResourceGroup: Hostname=" << node->getAttributes()["Hostname"]);
			ResourceGroup rsGroup;

			ResourceSeq rsSeq;

			NodeList items = node->getChildren();
			for (NodeList::iterator it_items = items.begin(); it_items
					!= items.end(); ++it_items) {
				NodePtr itemNode = *it_items;
				if (itemNode->getName() != "Resource") {
					// should be a bug of Parser
					continue;
				};

				//Attributes atts = itemNode->getAttributes();
				//for(Attributes::iterator it_att = atts.begin();
				//		it_att != atts.end();++it_att)
				//{
				//	MCE_DEBUG( "Resource "<< itemNode->getAttribute("Type") <<" Item attributes: " << it_att->first << "=" << it_att->second );
				//};

				Resource rs;
				rs.type = itemNode->getAttributes()["Type"];
				rs.name = itemNode->getAttributes()["Name"];
				rs.parameters = StrUtil::split(itemNode->getAttributes()["Parameter"], ",");
				rs.min = boost::lexical_cast<int>(itemNode->getAttributes()["Min"]);
				rs.max = boost::lexical_cast<int>(itemNode->getAttributes()["Max"]);
				rs.warningCount = boost::lexical_cast<int>(itemNode->getAttributes()["WarningCount"]);
				rs.resumeCount = boost::lexical_cast<int>(itemNode->getAttributes()["ResumeCount"]);
				rs.interval = boost::lexical_cast<int>(itemNode->getAttributes()["Interval"]);
				rs.warningMessage = itemNode->getAttributes()["Warning"];

				rsSeq.push_back(rs);
			};
			rsGroup.resource = rsSeq;

			StrSeq hostnames = StrUtil::split(node->getAttributes()["Hostname"], ",");
			for(StrSeq::iterator it = hostnames.begin();
					it != hostnames.end();
					++it)
			{
				if(*it == hostname )
				{
					rsGroup.name = *it;
					resourceManager->addResourceGroup(rsGroup);
				};
				if(isMonitorNode)
				{
					rsGroup.name = *it;
					_monitorManager->addResourceGroup(rsGroup);
				}
			};
		}
		else if(node->getName() == "Users")
		{
			if( ! isMonitorNode )
			{
				continue;
			};

			// This is a User node;
			NodeList items = node->getChildren();
			for(NodeList::iterator it_items = items.begin();
					it_items != items.end(); ++it_items)
			{
				NodePtr itemNode = *it_items;
				if(itemNode->getName() != "User")
				{
					// should be a bug of Parser
					continue;
				};

				User user;
				user.name = itemNode->getAttributes()["Name"];
				user.phone = itemNode->getAttributes()["Phone"];
				user.email = itemNode->getAttributes()["Email"];
				user.level = boost::lexical_cast<int>(itemNode->getAttributes()["Level"]);
				_userManager->addUser(user);
				MCE_DEBUG( "User: " << user2str(user) );
			};
		}
		else
		{
			MCE_FATAL("Config file format error!! node->getName()=" << node->getName() );
			//assert(false);
		}
	}
	MCE_INFO("MonitorManagerI initialize done.");
}

MonitorManagerPrx MceMonitor::MonitorManagerI::getMonitorManager() {
	return _monitorManager;
}
;

UserManagerPrx MceMonitor::MonitorManagerI::getUserManager() {
	return _userManager;
}
;

AlertManagerPrx MceMonitor::MonitorManagerI::getAlertManager() {
	return _alertManager;
}
;

void MceMonitor::MonitorManagerI::addResourceGroup(
		const ::MceMonitor::ResourceGroup& rg, const Ice::Current& current) {
	string name = rg.name;
	ResourceSeq res = rg.resource;
	for (ResourceSeq::iterator resIt = res.begin(); resIt != res.end(); ++resIt) {
		MCE_INFO("MonitorManager::addResourceGroup " << resource2str(*resIt));
		// Create a thread to monitor this Resource;
		IceUtil::ThreadPtr mt = new MonitorThread(name, *resIt, _status);
		_threads.push_back(mt);
		mt->start(65535).detach();
	}
}

void MceMonitor::MonitorManagerI::addLog(const ::MceMonitor::Log& result,
		const Ice::Current& current) {
	// add this Log object to local cache.
	_status->addLog(result);
}

MceMonitor::ResourceManagerI::ResourceManagerI(const ItemBuilder& builder) :
	_builder(builder) {
}

void MceMonitor::ResourceManagerI::addResourceGroup(
		const ::MceMonitor::ResourceGroup& rg, const Ice::Current& current) {
	string name = rg.name;
	ResourceSeq res = rg.resource;
	for (ResourceSeq::iterator resIt = res.begin(); resIt != res.end(); ++resIt) {
		MCE_INFO("ResourceManager: " << resource2str(*resIt));
		IceUtil::ThreadPtr thread = _builder.createThread(*resIt);
		_threads.push_back(thread);
		thread->start(65535).detach();
	}
}

::MceMonitor::UserSeq MceMonitor::UserManagerI::getUsers(::Ice::Int level,
		const Ice::Current& current) {
	return _users;
}

void MceMonitor::UserManagerI::addUser(const ::MceMonitor::User& u,
		const Ice::Current& current) {
	_users.push_back(u);
}

void MceMonitor::AlertManagerI::sendAlert(const ::MceMonitor::UserSeq& to,
		const ::std::string& msg, const Ice::Current& current) {
	for (UserSeq::const_iterator it = to.begin(); it != to.end(); ++it) {
		MCE_WARN("TO:" << it->name << " Phone:" << it->phone << " Email:"
				<< it->email << " SMS:" << msg);
	}
	sendEmail(to, msg);
	sendSMS(to, msg);
}

void MceMonitor::AlertManagerI::sendEmail(const ::MceMonitor::UserSeq& to,
		const ::std::string& msg) {
	jwsmtp::mailer m;
	for (UserSeq::const_iterator it = to.begin(); it != to.end(); ++it) {
		if ("" == it->email) {
			continue;
		};
		m.addrecipient(it->email);
	}
	m.setsender("status@ice.xiaonei.com");
	m.setsubject(msg);
	m.setmessage("ICE Server Status Warning");
	m.setserver("localhost");
	//m.authtype(authtype::PLAIN);
	m.send();
	cout << m.response() << endl;
}

void MceMonitor::AlertManagerI::sendSMS(const ::MceMonitor::UserSeq& to,
		const ::std::string& msg) {
	for (UserSeq::const_iterator it = to.begin(); it != to.end(); ++it) {
		if ("" == it->phone) {
			continue;
		};
		ostringstream cmd;
		cmd << "wget -q -O /dev/null \"http://alarm.xiaonei.com/alarm?mobile="
				<< it->phone << "&msg=" << msg << "\"";
		system(cmd.str().c_str());
	}
}

/* vim: set ts=4 shiftwidth=4 : */
