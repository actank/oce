#ifndef MCEMONITOR_ITEM_SHELL_API
#define MCEMONITOR_ITEM_SHELL_API

#include <Ice/Ice.h>
#include <map>
#include <iostream>
#include "item.h"

namespace MceMonitor
{
namespace Item
{

class Shell_Info : public Item
{
public:
	Shell_Info(const std::string& machine_id, const Resource& item);
	virtual void run();
	int getResult(const std::string&, int atline);
private:
	int getStatus(const std::string&, int atline);
};
};
};
#endif
