#ifndef _USERSCORE_EVENT_ICE
#define _USERSCORE_EVENT_ICE
#include <Util.ice>
module com{
	module xiaonei {
	   module wservice {
		module userscore {
		 module event {
			interface EventSender {				
				void send(MyUtil::Str2StrMap eventMap);
		     };
		  };		 
		};
	  };
	};
};
#endif
