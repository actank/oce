#ifndef __GET_DATA_HANDLER_FACTORY_H__
#define __GET_DATA_HANDLER_FACTORY_H__

#include "ServiceI.h"
#include "../interface/GetDataHandler.h"
#include "../impl/GetDataBuddyByNameHander.h"
#include "../impl/GetDataBuddyByIdHander.h"
#include "../impl/GetDataBuddyByAddTimeHander.h"
#include "../impl/GetDataBuddyByOnlineTimeHander.h"
#include "../impl/GetDataStatusCacheHandler.h"
#include "../impl/GetDataStatusBuddyByIdLoaderHandler.h"


namespace xce {
	namespace generalcheck {
		class GetDataHandlerFactory : public Ice::Object {
			public:
				static GetDataHandlerPtr getHandler(const std::string& identifier) {
					if(identifier == "BuddyByIdCache") {
						return new GetDataBuddyByIdHandler(identifier);
					} else if(identifier == "BuddyByNameCache") {
						return new GetDataBuddyByNameHandler(identifier);
					} else if(identifier == "BuddyByAddTimeCache") {
						return new GetDataBuddyByAddTimeHandler(identifier);
					} else if(identifier == "BuddyByOnlineTimeCache") {
						return new GetDataBuddyByOnlineTimeHandler(identifier);
					} else if(identifier == "StatusCache") {
						return new GetDataStatusCacheHandler(identifier);
					} else if(identifier == "StatusBuddyByIdLoader") {
						return new GetDataStatusBuddyByIdLoaderHandler(identifier);
					}
					return 0;
				}
		};
	}
} 
#endif
