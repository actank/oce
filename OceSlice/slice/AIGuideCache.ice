#ifndef __AI_GUIDE_CACHE__
#define __AI_GUIDE_CACHE__

module xce {
module aiguidecache {

	const int FARMGAME = 0;
	const int DOG = 1;
	const int CARPARK = 2;
	const int GODFATHER = 3;
	const int TYCOON = 4;
	const int APPMAX = 5;

	interface AIGuideCacheManager {		
		void set(int userId, int appId, bool status);
		bool get(int userId, int appId);
	};
};
};




















#endif
