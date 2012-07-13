#ifndef _FRIENDFINDER_INFOCACHE_ICE_ 
#define _FRIENDFINDER_INFOCACHE_ICE_ 
#include <Util.ice>

module xce {
	module friendfinder {
		class BirthData {
			int birthday;
		};

		class InfoData {
			MyUtil::IntSeq idSeq;
		};
		sequence<InfoData> InfoDataSeq;
	
		struct Item {
			MyUtil::StrSeq condition;
			long md5;
			MyUtil::IntSeq idSeq;
		};
		sequence<Item> ItemSeq;

		struct NetworkInfo {
			ItemSeq univInfo;
			ItemSeq highschoolInfo;
			ItemSeq juniorschoolInfo;
			ItemSeq elementaryschoolInfo;
		};	

		struct RecommendItem {
			int uid;
			int gender;	
		};
		sequence<RecommendItem> RecommendItemSeq;

		struct HighSchoolInfo {
			RecommendItemSeq highschoolseq;
			RecommendItemSeq juniorschoolseq;
			RecommendItemSeq collegeseq;
		};
	
		interface FriendFinderInfoCache {
			NetworkInfo getNetworkInfo(int userId);
			NetworkInfo getNetworkInfoWithHostage(int userId);
			NetworkInfo getNetworkInfoWithHostageFromDB(int userId);
			HighSchoolInfo getHighSchoolInfo(int userId);

			MyUtil::IntSeq getWorkplaceData(MyUtil::StrSeq condition);
			MyUtil::IntSeq getUnivData(MyUtil::StrSeq condition);
			MyUtil::IntSeq getHighschoolData(MyUtil::StrSeq condition);
			MyUtil::IntSeq getJuniorschoolData(MyUtil::StrSeq condition);
			MyUtil::IntSeq getElementaryschoolData(MyUtil::StrSeq condition);
			int getBirthdayData(int userId);

			void setWorkplaceDataWithMask(MyUtil::LongSeq md5Seq, InfoDataSeq dataSeq);
			void setUnivDataWithMask(MyUtil::LongSeq md5Seq, InfoDataSeq dataSeq);
			void setHighschoolDataWithMask(MyUtil::LongSeq md5Seq, InfoDataSeq dataSeq);
			void setJuniorschoolDataWithMask(MyUtil::LongSeq md5Seq, InfoDataSeq dataSeq);
			void setElementaryschoolDataWithMask(MyUtil::LongSeq md5Seq, InfoDataSeq dataSeq);

			void setBirthdayData(int birthYear, MyUtil::IntSeq orderedIds);

			void rebuild(int userId);

			MyUtil::IntSeq getFriendListByInfo(int userId,int limit);
			MyUtil::IntSeq getInfoMatchFriendList(int userId,int limit);
			MyUtil::IntSeq getFriendListByGuestInfo(int hostId,int guestId,int limit);
			MyUtil::IntSeq getGuestInfoMatchFriendList(int hostId,int guestId,int limit);
			MyUtil::IntSeq getGuestInfoMatchFriendListBatch(MyUtil::IntSeq hostIds,int guest);
			MyUtil::IntSeq getCommonFriendListByGuestInfo(int userId,int minsharecount,int limit);
		};
	};
};

#endif
