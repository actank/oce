#ifndef __CLUSTINGCOUNTI_H__
#define __CLUSTINGCOUNTI_H__

#include <Ice/Ice.h>
#include "ServiceI.h"
#include "Singleton.h"
#include "ClustingCount.h"

#ifndef CLUSTING_COUNT_API
#   define CLUSTING_COUNT_API ICE_DECLSPEC_EXPORT
#endif

extern "C"
{
    CLUSTING_COUNT_API ::IceBox::Service*
    create(::Ice::CommunicatorPtr communicator);
}

namespace mop
{

namespace hi
{

namespace svc
{

namespace model
{
using namespace std;
using namespace MyUtil;

const static string CLUSTING_COUNTER = "CC";

//-----------------------------------------------------------------------------
class CLUSTING_COUNT_API ClustingCountServiceI : public MyUtil::ServiceI,
    public MyUtil::Singleton<ClustingCountServiceI>
{
public:
    virtual void initialize();
    
};

//-----------------------------------------------------------------------------
class ClustingCounterI : virtual public ClustingCounter
{
public:		      	
	ClustingCounterI(ServiceI& service);
	virtual ~ClustingCounterI();
	
    virtual Ice::Int getServiceClustingCount(
    			const string& name,
				const Ice::Current&);
private:
	int _userServiceClustingCount;
	int _relationServiceClustingCount;
	int _classServiceClustingCount;
	int _tribeServiceClustingCount;
	int _footprintServiceClustingCount;
	int _blogFootprintServiceClustingCount;
	int _classFootprintServiceClustingCount;
	int _notifyPnlServiceClustingCount;
	int _notifyRplServiceClustingCount;
	int _notifyGplServiceClustingCount;
	int _notifyCplServiceClustingCount;
	int _notifyMnlServiceClustingCount;
	int _notifyNewbieServiceClustingCount;
	
};

};
};
};
};

#endif
