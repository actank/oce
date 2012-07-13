
#include <Util.ice>

module com {
	module xiaonei {
		module service {
		    exception NotReadyException extends MyUtil::MceException{};
			struct CommonFriend {
				int userId;
				int weight;
				MyUtil::IntSeq shares;
			};
			sequence<CommonFriend> CommonFriendSeq;

			struct ShareFriend {
				int count;
				MyUtil::IntSeq shares;
			};
			dictionary<int, ShareFriend> Int2ShareFriendMap;
			
			interface FriendFinderManager {
				CommonFriendSeq getCommonFriend(int userId, int begin, int limit);
				void removeCommonFriend(int userId, int friendId);
				void reloadCommonFriend(int userId);
				void rebuildCommonFriend(int userId);
				
				MyUtil::IntSeq getSharedFriend(int userId1,int userId2, int begin, int limit);
				int getSharedFriendCount(int userId1,int userId2);
				Int2ShareFriendMap getSharedFriends(int fromId, MyUtil::IntSeq toIds, int limit);
			};
		};
	};
};

