#ifndef MCEMONITOR_ITEMBUILDER
#define MCEMONITOR_ITEMBUILDER

#include "MceMonitor.h"
#include "items/item.h"

namespace MceMonitor
{
	
class ItemBuilder
{
public:
	ItemBuilder(const Ice::CommunicatorPtr&, const std::string&);
	IceUtil::ThreadPtr createThread(const Resource& item);
private:
	Ice::CommunicatorPtr _ic;
	MonitorManagerPrx _monitor;
	std::string _machine_id;
};

};

#endif

