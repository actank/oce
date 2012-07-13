
module com
{
module xiaonei
{
module services
{
module FriendInput
{
	sequence<int> IntSeq;
	interface FriendInputManager
	{
		IntSeq getFriendInput(int hostId, string condition, int limit);
		void reLoadFriend(int hostId);
	};
	interface FriendInputData{};
};
};
};
};
