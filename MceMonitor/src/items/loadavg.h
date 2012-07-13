#ifndef MCEMONITOR_ITEM_LOADAVG_API
#define MCEMONTIOR_ITEM_LOADAVG_API

#include "item.h"

namespace MceMonitor
{
namespace Item
{
	
typedef struct _Loadavg
{
	float min1;
	float min5;
	float min15;
};

class Loadavg_Info : public Item
{
public:
	Loadavg_Info(const std::string& machine_id, const Resource& item);
	virtual void run();
	float getLoadavg1();
	float getLoadavg5();
	float getLoadavg15();
private:
	_Loadavg getStatus();
};
};
};
#endif

