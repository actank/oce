#include <ObjectCache.ice>
#include <FriendFinder.ice>

module xce {
	module friendfinder {
		struct CommonFriendItem {
			int userId;
			int shareCount;
		};
		sequence<CommonFriendItem> CommonFriendItemSeq;

		interface FriendFinderByCommonManager {
			void setCommonFriendSeq(int userId,com::xiaonei::service::CommonFriendSeq cfs);
			CommonFriendItemSeq getCommonFriendItemSeq(int userId, int limit);
			CommonFriendItemSeq getCommonFriendItemSeqById(int userId, int limit);
			int getCount(int userId);
			void removeCommonFriendItem(int userId, int friendId);
			void reload(int userId);

			MyUtil::IntSeq getFriendsByCommon(int guestId,int hostId,int limit);
		};
	};
};

