
#include "MceMonitor.h"
#include "control.h"
#include <IceUtil/Thread.h>
#include "../MceMonitorI.h"

using namespace std;
using namespace ::MceMonitor::Item;

Control::Control(const string& machine_id, const vector<IceUtil::ThreadPtr>& threads)
	 : Item(machine_id, Resource()), _threads(threads)
{
    cout<<"control"<<endl;
	_item.interval = 10;
}

void Control::run()
{
	while(true)
	{
        for ( vector<IceUtil::ThreadPtr>::iterator it = _threads.begin();
			it != _threads.end();
			++it )
		{
			cout << "[Thread]" <<  (*it)->isAlive() << endl;
			if (!  (*it)->isAlive() )
			{
				Log m;
				m.resourcegroup = _machine_id;
				m.resource = _item.name;
				m.type = "CONTROL";
				m.value = -1;
				m.time = time(NULL);

				MonitorManagerI::instance().getMonitorManager()->addLog(m);
			}
		}
		sleep(_item.interval);
	}
}
