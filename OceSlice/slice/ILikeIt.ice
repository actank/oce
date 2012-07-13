#ifndef __I_LIKE_IT_ICE__
#define __I_LIKE_IT_ICE__

#include <Util.ice>
#include <ObjectCache.ice>

module xce {
module ilikeit {

class GlobalId {
	int type;
	long id;
};

sequence<GlobalId> GlobalIdSeq;

/* @brief 一组like的关系 */
class Likeship {
	GlobalId gid;
	int uid;
};

sequence<Likeship> LikeshipSeq;

/* @brief like一个gid对象的所有用户ids */
class LikeData {
	MyUtil::IntSeq users;
};

dictionary<long, LikeData> LikeDataMap;

/* @brief 一个用户的信息组 */
class UserDetail {
	int uid;
	string name;
	string img;
};
sequence<UserDetail> UserDetailSeq;

class UserDetail2 {
	int uid;
	string name;
	string img;
	bool isKeepUse;
};
sequence<UserDetail2> UserDetail2Seq;


/* @brief like一个gid对象针对特定用户排序后的结果 */
class LikeInfo {
	int selfCount;
	int friendCount;
	int totalCount;
	bool withFriendList;	/* @brief 用户的好友列表是否正确取到 */
	bool showStrangers;	/* @brief 是否显示陌生人 */
	int detailLimit;		/* @brief 显示详细信息的用户数量限制 */
	// MyUtil::IntSeq uidsSort;	/* @brief 用户排序结果 */
	UserDetailSeq detailList;	/* @brief 喜欢该的自己-好友-陌生人信息列表 */
};
sequence<LikeInfo> LikeInfoSeq;

class LikeInfo2 {
	int selfCount;
	int friendCount;
	int totalCount;
	bool withFriendList;	/* @brief 用户的好友列表是否正确取到 */
	bool showStrangers;	/* @brief 是否显示陌生人 */
	int detailLimit;		/* @brief 显示详细信息的用户数量限制 */
	// MyUtil::IntSeq uidsSort;	/* @brief 用户排序结果 */
	UserDetail2Seq detailList;	/* @brief 喜欢该的自己-好友-陌生人信息列表 */
};
sequence<LikeInfo2> LikeInfo2Seq;

/* @brief 一个gid对象被like的总数和前若干like它的用户id结果 */
class LikeInfoSimple {
	int totalCount;		/* @brief 喜欢该对象的用户总数 */
	MyUtil::IntSeq users;	/* @brief 喜欢该对象的用户列表(前若干个，由取这个对象的方法控制数量) */
};
sequence<LikeInfoSimple> LikeInfoSimpleSeq;

class ILikeItManager {

	void addLike(Likeship ship);

	LikeInfo addLikeAndGet(Likeship ship);

	LikeInfo2 addLikeAndGet2(Likeship ship);

	void addLikeBatch(LikeshipSeq shipSeq);

	void removeLike(Likeship ship);

	LikeInfo removeLikeAndGet(Likeship ship);

	LikeInfo2 removeLikeAndGet2(Likeship ship);

	void removeLikeBatch(LikeshipSeq shipSeq);
};

class ILikeItCache {
	
	/* @brief	取得赞过指定gid的对象的所有用户数量 */
	int getLikeCount(GlobalId gid);

	/* @brief	取得赞过指定gid的对象的所有用户id */
	MyUtil::IntSeq getLikeCache(GlobalId gid);
	
	/* @brief	取得赞过指定gid对象的所有用户id按查看用户排序后的结果 */
	LikeInfo getLikeDetail(GlobalId gid, int uid, int limit, bool showStrangers);
	LikeInfo2 getLikeDetail2(GlobalId gid, int uid, int limit, bool showStrangers);

	/* @brief	批量获取一组gid的LikeInfo */
	LikeInfoSeq getLikeDetailBatch(GlobalIdSeq gids, int uid, int limit, int detailNum);
	LikeInfo2Seq getLikeDetailBatch2(GlobalIdSeq gids, int uid, int limit, int detailNum);

	/* @brief	取得赞过指定gid对象的用户总数和前limit个id(按用户id数值排列) */
	LikeInfoSimple getLikeInfoSimple(GlobalId gid, int limit);

	/* @brief	取得一组对象的LikeInfoSimple信息 */
	LikeInfoSimpleSeq getLikeInfoSimpleBatch(GlobalIdSeq gids, int limit);

	/* @brief	取一个对象是否被一个用户like过.返回
	   @return 	1为like过，0为未like过 	*/
	int isLiked(GlobalId gid, int uid);

	/* @brief	取一组对象是否被一个用户like过 */
	MyUtil::IntSeq isLikedBatch(GlobalIdSeq gids, int uid);	

	/* @brief	设置一组cache信息*/
	void setData(MyUtil::ObjectResult userData);

	/* @brief	load一组gid的用户的结果(如果内存中有则更新) */
	void load(GlobalIdSeq gids);

	/* @brief	增加一组like关系 */
	void addLikeship(Likeship ship);

	/* @brief	删除一系列like关系 */
	void removeLikeship(Likeship ship);

	/* @brief	标志服务是否预加载结束 */
	bool isValid();

	/* @biref	设置服务是否预加载结束 */
	void setValid(bool newState);
};

class ILikeItNotice {

	void addLikeNotice(Likeship ship);

	void removeLikeNotice(Likeship ship);

	void getLikeNotice(GlobalId gid);
};

};
};

#endif
