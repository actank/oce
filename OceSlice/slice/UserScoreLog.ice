#ifndef _USERSCORE_LOG_ICE
#define _USERSCORE_LOG_ICE
module xce {
	module userscore {
		interface UserScoreLogManager {
			void addLog(int userId,int type,int scoreAdd);
		};
	};		 
};
#endif
