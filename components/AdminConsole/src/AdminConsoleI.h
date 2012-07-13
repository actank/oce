#ifndef __AdminConsoleI_h__
#define __AdminConsoleI_h__

#include "Singleton.h"
#include <AdminConsole.h>
#include "IceUtil/IceUtil.h"
#include "Ice/Ice.h"

namespace xce {

namespace adminconsole {

class AdminConsoleComponent : public Ice::Object {
public:

	virtual std::string description()=0;

	virtual std::string help()=0;

	virtual std::string execute(const std::vector<std::string> argv)=0;
};

typedef IceUtil::Handle<AdminConsoleComponent> AdminConsoleComponentPtr;

class AdminConsoleManagerI: virtual public AdminConsoleManager,
		public MyUtil::Singleton<AdminConsoleManagerI> {
public:
	AdminConsoleManagerI():
		commandNotFound("Command not found, please use \"help\" for more information\n"),
		helpDescription("Supported operators:\n1)\thelp\n2)\tcomponentType\n3)\tcomponentType help\n4)\tcomponentType list\n5)\tcomponentType componentName arg0 arg1 arg2 ...\n")
	{}

	virtual std::string parse(const std::string&, const Ice::Current&);

	virtual std::string registerComponent(const std::string&,
			const std::string&, const AdminConsoleComponentPtr);

private:

	std::string list(std::map<std::string, AdminConsoleComponentPtr>&);

	const std::string commandNotFound;

	std::string helpDescription;

	std::map<std::string, std::map<std::string, AdminConsoleComponentPtr> > componentLists;
};

}

}

#endif
