#ifndef __BUDDY_BYAPPLICANT_CACHE_ICE__
#define __BUDDY_BYAPPLICANT_CACHE_ICE__

#include <Util.ice>
#include <ObjectCache.ice>

module xce {
module buddy {

	struct AccepterInfo
	{
		int accepter;
		int time;
	};
	sequence<AccepterInfo> AccepterInfoSeq;

	class BuddyByApplicantData {
		AccepterInfoSeq accepters;
		int totalCount;
	};

	interface BuddyByApplicantCacheManager {
		//for web
		BuddyByApplicantData getAccepters(int id, int begin, int limit); //Asynchronously
		BuddyByApplicantData getAcceptersSync(int id, int begin, int limit); //Synchronously
		//for loader
		void addAccepter(int id, AccepterInfo accepter);
		void removeAccepter(int id, int accepterId);
		void reload(int id, Object data);
		void clear(int id);
		//for preloader
		void setData(MyUtil::ObjectResult userData); 
                bool isValid();
                void setValid(bool newState);

	};

	interface BuddyByApplicantLoader {
		void addAccepter(int id, AccepterInfo accepter);
		void removeAccepter(int id, int accepterId);
		void reload(int id);
		void clear(int id);
	};

};
};

#endif
