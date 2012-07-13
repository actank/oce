#ifndef _BUDDY_PAGE_COMMON_FANS_ICE_
#define _BUDDY_PAGE_COMMON_FANS_ICE_


#include <Util.ice>
#include <PageCommon.ice>
module xce {
	module buddy {
		struct CommonFansParam {
      int uid;
      int pid;
			int cbegin;
      int climit;
      int ubegin;
      int ulimit;
		};    

  	interface PageCommonFansCacheManager {
			 // Get friend ids of uid that are the fans of pid
			 // if begin < 0 or limit == -1 or limit == 0, return empty sequence
			 // if begin > size, return empty sequence
			 // if limit = -1, return sequence[begin, end)
			 // if begin + limit > size, return sequence[begin, end)
			 // if begin + limit < size, return sequence[begin, begin + limit)
       MyUtil::IntSeq GetCommon(int uid, int pid, int begin, int limit);

			 // Get friend ids of uid that are not the fans of pid 
			 // if begin < 0 or limit == -1 or limit == 0, return empty sequence
			 // if begin > size, return empty sequence
		   // if limit = -1, return sequence[begin, end)
			 // if begin + limit > size, return sequence[begin, end)
			 // if begin + limit < size, return sequence[begin, begin + limit)
       MyUtil::IntSeq GetUnCommon(int uid, int pid, int begin, int limit);

		   // Get Both CommonFans and UnCommonFans sequences
       CommonFans GetCommonAndUnCommon(CommonFansParam param);

			 //Save Data to  Cache
			 bool SetData(PageCommonFans pcf);
       void AddPages(MyUtil::IntSeq pageIdSeq);
  		 MyUtil::Int2IntSeqMap GetFull(int uid);
		};

	};
};

#endif
