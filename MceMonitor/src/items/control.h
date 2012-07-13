#ifndef MCEMONITOR_ITEM_CONTROL_API
#define MCEMONITOR_ITEM_CONTROL_API

#include "item.h"
#include "MceMonitor.h"

using namespace std;

namespace MceMonitor
{

namespace Item
{

class Control : public Item
{
public:
	Control(const string& machine_id, const vector<IceUtil::ThreadPtr>& threads);
	virtual void run();
private:
	vector<IceUtil::ThreadPtr> _threads;
};
typedef IceUtil::Handle<Control> ControlPtr;

};
};

#endif
