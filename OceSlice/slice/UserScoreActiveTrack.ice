#ifndef _USERSCORE_ACTIVE_TRACK
#define _USERSCORE_ACTIVE_TRACK
module com{
	module xiaonei {
	   module wservice {
		module userscore {
		  module activetrack {
			interface ActiveTrackLogic {
				//增加一次登录
                void incrLoginCount(int userId,byte type);
                
                void resetLogined(int userId,bool b);
		     };	
		  };	 
		};
	  };
	};
};
#endif