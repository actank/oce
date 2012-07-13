#ifndef MCEMONITOR_ITEM_MEM_API
#define MCEMONITOR_ITEM_MEM_API

#include "item.h"
#include <IceUtil/Mutex.h>

namespace MceMonitor
{
namespace Item
{
	
typedef struct _Mem
{
	int total;
	int free;
	int buffer;
	int cached;
	int swaptotal;
	int swapfree;
	int get_used()
	{
		return total - free - buffer - cached;
	}
	int get_swap_used()
	{
		return swaptotal - swapfree;
	}
};

class Mem_Info : public Item
{
public:
	Mem_Info(const std::string& machine_id, const Resource& item);
	virtual void run();
	float getMemUsedRate();
	float getSwapUsedKb();
	float getResult();
private:
	float (Mem_Info::*_getResult)();
	_Mem getStatus();
	static IceUtil::Mutex _proc_mutex;
};

};
};
	
#endif
