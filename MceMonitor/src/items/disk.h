#ifndef MCEMONITOR_ITEM_DISK_API
#define MCEMONITOR_ITEM_DISK_API

#include <Ice/Ice.h>
#include <map>
#include <iostream>
#include "item.h"

namespace MceMonitor
{
namespace Item
{

typedef struct _MountPoint
{
	Ice::Long used;
	Ice::Long free;
	Ice::Long get_total()
	{
		return used + free;
	}
};
typedef std::map<std::string, _MountPoint> _Disk;

class Disk_Info : public Item
{
public:
	Disk_Info(const std::string& machine_id, const Resource& item);
	virtual void run();
	int getDiskFree(std::string);
	float getDiskFreeRate(std::string);
private:
	_Disk getStatus();
};

};
};
#endif

