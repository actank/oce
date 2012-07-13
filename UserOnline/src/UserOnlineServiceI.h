#include <IceBox/IceBox.h>
#include "ServiceI.h"
#include "Channel.h"

#include "UserOnline.h"
#include "ActiveUser.h"

#ifndef USER_ONLINE_API
#   define USER_ONLINE_API 
#endif

extern "C"
{
	    USER_ONLINE_API ::IceBox::Service*
			    create(::Ice::CommunicatorPtr communicator);
}


namespace mop
{

namespace hi
{

namespace svc
{
namespace online
{
using namespace MyUtil;
using namespace mop::hi::svc::model;
	
const string MANAGER = "Manager";
const string TABLE_ONLINE = "online";




};
};
};
};

