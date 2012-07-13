#ifndef MCEMONITOR_ITEM_API
#define MCEMONITOR_ITEM_API

#include <Ice/Ice.h>
#include <IceUtil/Thread.h>
#include "MceMonitor.h"

namespace MceMonitor
{
namespace Item
{

class Item : public IceUtil::Thread
{
public:
	virtual void run();
	//virtual int getResult();
protected:
	std::string _machine_id;
	::MceMonitor::Resource _item;
	Item(const std::string& machine_id, const ::MceMonitor::Resource& item);
private:
};

typedef IceUtil::Handle<Item> ItemPtr;
};
};
#endif
