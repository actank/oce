#ifndef MCEMONITOR_ITEM_ICEKEEP_API
#define MCEMONITOR_ITEM_ICEKEEP_API
#include <Ice/Ice.h>
#include <iostream>
#include "item.h"

namespace MceMonitor
{
namespace Item
{

class IceKeep_Info : public Item
{
public:
	IceKeep_Info(const Ice::CommunicatorPtr& ic, const std::string& machine_id, const Resource&);
	virtual void run();
	int get(std::string, std::string, std::string);
	int get(std::string, std::string, std::string, int);
	int get(std::string, std::string, std::string, int, int);
	int get(std::string, std::string, std::string, int, int, int);
	int get(std::string, std::string, std::string, int, int, int, int);
private:
	int getStatus(std::string, std::string, std::string);
    void reboot_ice(std::string);
	Ice::OutputStreamPtr _out;
	Ice::CommunicatorPtr _ic;
};
};
};
#endif
