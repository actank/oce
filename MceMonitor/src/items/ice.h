#ifndef MCEMONITOR_ITEM_ICE_API
#define MCEMONITOR_ITEM_ICE_API
#include <Ice/Ice.h>
#include <iostream>
#include "item.h"

namespace MceMonitor
{
namespace Item
{
	
class Ice_Info : public Item
{
public:
	Ice_Info(const Ice::CommunicatorPtr& ic, const std::string& machine_id, const Resource&);
	virtual void run();
private:
	int getStatus();
	Ice::OutputStreamPtr _out;
	Ice::CommunicatorPtr _ic;
	std::string _strProxy;
	std::string _invokeType;
	std::string _strInvoke;
};

};
};
#endif
