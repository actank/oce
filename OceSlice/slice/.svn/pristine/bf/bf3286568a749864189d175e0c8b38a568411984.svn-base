#ifndef __NOTIFY_ICE__
#define __NOTIFY_ICE__

#include <Util.ice>

module xce{
module notify{

class NotifyContent{
	long id;		  //notifyID
	long source;  //用于合并删除
	int owner;		//回复页面的主人
	int from;		  //通知触发者

	int type;		  //通知类型, 最高位在内存中表示是否有压缩，压缩最高位为1, 低16位表示回复提示类型， 高15位为通知类型
	int time;
	int isreply;
	string props;	//Key-value data
};

sequence<NotifyContent> NotifyContentSeq;
//sequence<NotifyContentSeq> NotifyContentSeqs;
dictionary<long,NotifyContent> NotifyContentDict; 

class NotifyContentManager{
  // TODO : 是否需要删除接口?
	NotifyContentDict getNotifyDict(MyUtil::LongSeq NotifyIds); 
	void addNotify(NotifyContent notify);
};


struct NotifyIndex{
	long id;
	long source;
	int from;
	int type;		//通知的类型, 最高位在内存中表示是否有压缩，压缩最高位为1, 低16位表示回复提示类型， 高15位为通知类型
	int isreply;
};
sequence<NotifyIndex> NotifyIndexSeq;
dictionary<int, NotifyIndexSeq> NotifyIndexSeqMap;
class NotifyIndexManagerN{
//============================RMESSAGE======================================================================
	void addNotify(NotifyContent notify, MyUtil::IntSeq uids);
	NotifyIndexSeq GetRMessageByBigtype(int uid, int begin, int limit, int bigtype);
	NotifyIndexSeq GetRMessageBeforLastNid(int uid, int view, long nid, int limit, int bigtype);
	NotifyIndexSeq GetRMessageMerge(int uid, int view, long nid, int limit, int bigtype);
	NotifyIndexSeq GetRMessageByType(int uid, MyUtil::IntSeq types, int begin, int limit, int bigtype);
	NotifyIndexSeqMap GetRMessageByTypeAll(int uid, MyUtil::IntSeq types, int begin, int limit);
	int GetUnreadCountByBigtype(int userid, int bigtype);
	MyUtil::Int2IntMap GetUnreadCountByBigtypes(int userid, MyUtil::IntSeq bigtypes);
	void RemoveRMessageById(int uid, long notifyId, int type, int bigtype);
	void RemoveRMessageByIds(int uid, MyUtil::LongSeq notifyIds, int type, int bigtype);
	void RemoveRMessageBySource(int uid, int type, long source, int bigtype);
	void RemoveRMessageBySourceSeq(int uid, int type, MyUtil::LongSeq source, int bigtype);
	void RemoveRMessageBySourceAndSender(int uid, int type, long source, int sender, int bigtype);
	int GetUndealCountByBigtype(int uid, int bigtype);
	int GetUndealCountByType(int uid, int type);
	MyUtil::Int2IntMap GetUndealCountByTypes(int uid, MyUtil::IntSeq types);
//============================RMESSAGE==============================================================================


};
class NotifyIndexManager{
  // 将notify 加入到uids中各个uid 的 index 列表中
	void addNotify(NotifyContent notify, MyUtil::IntSeq uids);
  // 删除 uid 的 index 列表中相应的元素
	void removeById(int uid, long notifyId);
	void removeByIds(int uid, MyUtil::LongSeq notifyIds);
	void removeBySource(int uid, int type, long source);
	void removeBySourceAndSender(int uid, int type, long source, int sender);
	string dumpList(int uid);
  // 获取指定类型的新留言及回复 from_id 的列表. 参数 begin 忽略
	MyUtil::IntSeq getFromIdSeq(int uid, MyUtil::IntSeq types, int begin, int limit);
  // 获取所有提示列表
	NotifyContentSeq getNotifySeq(int uid, long lastNotifyId, int limit);
  // 获取回复提示列表
	NotifyContentSeq getReplySeq(int uid, long lastNotifyId, int limit);
  // 获取指定类型的notify列表.
	NotifyContentSeq getByType(int uid, MyUtil::IntSeq types, long lastNotifyId, int limit);
  // 获取所有提示列表
	NotifyContentSeq getNotifySeq2(int uid, int begin, int limit);
  // 获取回复提示列表
	NotifyContentSeq getReplySeq2(int uid, int begin, int limit);
  // 获取指定类型的notify列表.
	NotifyContentSeq getByType2(int uid, MyUtil::IntSeq types, int begin, int limit);
  // 获取指定类型的notify列表.有合并
	//NotifyContentSeq getByType2X(int uid, MyUtil::IntSeq types, int begin, int limit,int view );


  // 获取所有提示列表
	//NotifyContentSeq getNotifySeqMerge(int uid, long lastNotifyId, int limit);
  // 获取回复提示列表
	//NotifyContentSeq getReplySeqMerge(int uid, long lastNotifyId, int limit);
	
 // 获取所有提示列表
	NotifyContentSeq getNotifySeqX(int uid, int view,long lastNotifyId, int limit);
  // 获取回复提示列表
	NotifyContentSeq getReplySeqX(int uid,int view, long lastNotifyId, int limit);
	


	bool reloadSchema();
  void AddTester(int uid);

	int getCount(int uid, MyUtil::IntSeq types);

	// 获取pager中显示的未读数目
	int getUnreadCount(int uid);
	int getUnreadCountByView(int uid,int view);
	int getUnreadCountByTypes(int uid,MyUtil::IntSeq types);
	int getUnreadCountByTypesAndMaxReadId(int uid,MyUtil::IntSeq types,int MaxReadId);
};

exception KeyAbsentException extends MyUtil::MceException {};

class NotifyDispatcher{
	void dispatch(MyUtil::Str2StrMap props)
      throws KeyAbsentException;

	void deliverWebpager(MyUtil::Str2StrMap props)
      throws KeyAbsentException;
	bool reloadSchema(); // 重新加载schema/template信息
};

struct GroupIndex{
	long source;
	int stype;	
	int actor;	
};
sequence<GroupIndex> GroupIndexSeq;

class NotifyDispatcherN {
	void dispatchN(long notifyId, bool store, MyUtil::Str2StrMap props);
  void EnableChat(bool enable);

	bool reloadSchema(); // 重新加载schema/template信息
	void QuitChat(int uid,GroupIndexSeq groups);
  void JoinChat(MyUtil::IntSeq uid,GroupIndex idx);
};

class NotifyTarget{
	MyUtil::IntSeq uids;
	MyUtil::IntSeq minigroups;
	MyUtil::IntSeq minigroupMembers;
	MyUtil::IntSeq ugcUids;
	MyUtil::IntSeq ugcMinigroups;
	MyUtil::IntSeq ugcMinigroupMembers;
	MyUtil::IntSeq ugcBlockUids;
};
class NotifyGate{
	bool reloadSchema(); // 重新加载schema/template信息

	void instantNotify(NotifyContent notify, MyUtil::IntSeq uids);
	void deliverWebpager(NotifyContent notify, MyUtil::IntSeq uids);

	MyUtil::StrSeq getNotifySeq(int uid, int view, long lastNotifyId, int limit);
	MyUtil::StrSeq getReplySeq(int uid, int view, long lastNotifyId, int limit);
	MyUtil::StrSeq getByType(int uid, int view, MyUtil::IntSeq types, long lastNotifyId, int limit);

	MyUtil::StrSeq getNotifySeq2(int uid, int view, int begin, int limit);
	MyUtil::StrSeq getReplySeq2(int uid, int view, int begin, int limit);
	MyUtil::StrSeq getByType2(int uid, int view, MyUtil::IntSeq types, int begin, int limit);
	
//	MyUtil::StrSeq getNotifySeqX(int uid, int view, MyUtil::IntSeq types, int begin, int limit);
	MyUtil::StrSeq getNotifySeqX(int uid, int view,  long lastNotifyId, int limit);
	MyUtil::StrSeq getReplySeqX(int uid, int view,  long lastNotifyId, int limit);
	
	void QuitChat(int uid,GroupIndexSeq groups);

////////////////////RMESSAGE////////////////////////////
	void RemoveLogicSource(int uid, int type, long source);
	void RemoveLogicBySourceSeq(int uid, int type, MyUtil::LongSeq sources);
////////////////////RMESSAGE////////////////////////////



};
class FeedGate{};

class NotifyAdmin{};

};
};

#endif
