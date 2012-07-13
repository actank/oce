#include "ItemBuilder.h"
#include "items/control.h"
#include "items/loadavg.h"
#include "items/cpu.h"
#include "items/mem.h"
#include "items/disk.h"
#include "items/ice.h"
#include "items/icekeep.h"
#include "items/shell.h"
#include "items/item.h"

#include "MceMonitorI.h"

using namespace ::MceMonitor;
using namespace ::MceMonitor::Item;

ItemBuilder::ItemBuilder(const Ice::CommunicatorPtr& ic, const string& machine_id)
		: _ic(ic), _machine_id(machine_id)
{};

IceUtil::ThreadPtr
ItemBuilder::createThread(const Resource& item)
{
	MCE_TRACE("ItemBuilder add "<<item.type);
	if ( item.type == "Loadavg" ) return new Loadavg_Info(_machine_id, item);
	if ( item.type == "CPU" )
	{
		assert(item.parameters.size() == 1);
		return new CPU_Info(_machine_id, item);
	}
	if ( item.type == "DiskFree" )
	{
		assert(item.parameters.size() == 1);
		return new Disk_Info(_machine_id, item);
	}
	if ( item.type == "Mem" )
	{
		assert(item.parameters.size() == 1);
		return new Mem_Info(_machine_id, item);
	}

	if ( item.type == "Shell" )
	{
		assert(item.parameters.size() == 2);
		return new Shell_Info(_machine_id, item);
	}
    assert(false);
};

