#ifndef __COMPARE_HANDLER_FACTORY_H__
#define __COMPARE_HANDLER_FACTORY_H__

#include "../impl/BuddyDataCompareHandler.h"
#include "../impl/StatusDataCompareHandler.h"

namespace xce {
	namespace generalcheck {
		class CompareHandlerFactory : public Ice::Object {
			public:
				static CompareHandlerPtr getHandler(const string& identifier) {
					if(identifier == "BuddyDataCompare") {
						return new BuddyDataCompareHandler;
					} else if(identifier == "StatusDataCompare") {
						return new StatusDataCompareHandler;
					}	
					return 0;
				}
		};
	}
}
#endif
