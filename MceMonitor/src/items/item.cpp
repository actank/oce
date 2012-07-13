#include "item.h"
#include "MceMonitor.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

MceMonitor::Item::Item::Item(const string& machine_id, const Resource& item)
	:  _machine_id(machine_id), _item(item)
{
}

void 
MceMonitor::Item::Item::run()
{
}
