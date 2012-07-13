#ifndef _USER_MONITER_ICE
#define _USER_MONITER_ICE
module com{
	module xiaonei{
		module safecenter{
			module usermonitor{
				interface Monitor{
					void setUserActivity(int id,string ip,string url);
				};
			};
		};
	};
};
#endif
