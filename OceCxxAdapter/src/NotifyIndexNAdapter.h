#ifndef _NOTIFY_INDEXN_ADAPTER_H_
#define _NOTIFY_INDEXN_ADAPTER_H_

#include "AdapterI.h"
#include "Notify.h"
#include "NotifyIndexAdapter.h"



namespace xce {
namespace notify {

using namespace MyUtil;

/*
class NotifyBody: public Ice::Object {
	public:
		NotifyBody(NotifyContentPtr notify);
		Ice::Long id;
		Ice::Long source; //用于合并删除
		Ice::Int owner; //回复页面的主人
		Ice::Int from; //谁发的
		//  int to;       //回复给谁
		Ice::Int type; //回复的类型, 最高位在内存中表示是否有压缩，压缩最高位为1
		int bigtype;
		time_t time;
		map<string, string> props; //Key-value data
};
typedef IceUtil::Handle<NotifyBody> NotifyBodyPtr;
typedef map<Ice::Long, NotifyBodyPtr> NotifyBodyDict;
typedef vector<NotifyBodyPtr> NotifyBodySeq;
typedef map<int, NotifyBodySeq> NotifyBodySeqMap;
*/

class NotifyIndexNAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::NotifyChannel, NotifyIndexNAdapter> {
public:
  NotifyIndexNAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);

  }
  NotifyIndexManagerNPrx getManager(Ice::Long id) {
    return locate<NotifyIndexManagerNPrx> (_managers, "M", id, TWO_WAY);
  }
  NotifyIndexManagerNPrx getManagerOneway(Ice::Long id) {
    return locate<NotifyIndexManagerNPrx> (_managersOneway, "M", id, ONE_WAY);
  }
  void addNotify(const NotifyContentPtr& r, const MyUtil::IntSeq& uids);

	/*
  void removeById(Ice::Int uid, Ice::Long notifyId);
  void removeByIds(Ice::Int uid, const MyUtil::LongSeq& nids);
  void removeBySource(Ice::Int uid, Ice::Int type, Ice::Long source);
  void removeBySourceAndSender(Ice::Int uid, Ice::Int type, Ice::Long source,Ice::Int sender);

  NotifyBodySeq getNotifySeq(Ice::Int uid, Ice::Long nid, Ice::Int limit);
  NotifyBodySeq getReplySeq(Ice::Int uid, Ice::Long nid, Ice::Int limit);
  NotifyBodySeq getByType(Ice::Int uid, const MyUtil::IntSeq& type,
      Ice::Long nid, Ice::Int limit);

  NotifyBodySeq getNotifySeq2(Ice::Int uid, Ice::Int begin, Ice::Int limit);
  NotifyBodySeq getReplySeq2(Ice::Int uid, Ice::Int begin, Ice::Int limit);
  NotifyBodySeq getByType2(Ice::Int uid, const MyUtil::IntSeq& type,
      Ice::Int begin, Ice::Int limit);

	*/

/*  NotifyBodySeq getNotifySeqMerge(Ice::Int uid, Ice::Long nid, Ice::Int limit);
  NotifyBodySeq getReplySeqMerge(Ice::Int uid, Ice::Long nid, Ice::Int limit);*/

	/*
  NotifyBodySeq getNotifySeqX(Ice::Int uid,Ice::Int view, Ice::Long nid, Ice::Int limit);
  NotifyBodySeq getReplySeqX(Ice::Int uid, Ice::Int view,Ice::Long nid, Ice::Int limit);
  bool reloadSchema();

  Ice::Int getUnreadCount(Ice::Int uid);
  Ice::Int getUnreadCountByView(Ice::Int uid,int view);
	*/

//==========================================================RMESSAGE================================
	NotifyBodySeq GetNotifyBodyByNotifyIndex(const NotifyIndexSeq& indexs, vector<Ice::Long>& fail_in_db_nids);

	NotifyBodySeq GetRMessageByBigtype(int uid, int begin, int limit, int bigtype);
	NotifyBodySeq GetRMessageBeforLastNid(int userid, int view, Ice::Long nid, int limit, int bigtype);
	NotifyBodySeq GetRMessageMerge(int userid, int view, Ice::Long nid, int limit, int bigtype);
	NotifyBodySeq GetRMessageByType(int userid, const MyUtil::IntSeq& types, int begin, int limit, int bigtype);
	NotifyBodySeqMap GetRMessageByTypeAll(int userid, const MyUtil::IntSeq& types, int begin, int limit);
	int GetUnreadCountByBigtype(int userid, int bigtype);
	MyUtil::Int2IntMap GetUnreadCountByBigtypes(int userid, const MyUtil::IntSeq& types);
	void RemoveRMessageById(int uid, Ice::Long notifyId, int type, int bigtype);
	void RemoveRMessageByIds(int uid, const MyUtil::LongSeq& notifyIds, int type, int bigtype);
	void RemoveRMessageBySource(int uid, int type, Ice::Long source, int bigtype);
	void RemoveRMessageBySourceAndSender(int uid, int type, Ice::Long source, int sender, int bigtype);
	int GetUndealCountByBigtype(int uid, int bigtype);
	int GetUndealCountByType(int uid, int type);
	MyUtil::Int2IntMap GetUndealCountByTypes(int uid, const MyUtil::IntSeq& types);
	void TestGet1(int userid, int begin = 0, int limit = 10, int type = 14, int view = 1, int bigtype = 1);
	//void TestGet1(int userid, int begin, int limit, int type, int view, int bigtype);
//==========================================================RMESSAGE================================

protected:
  void conentSeq2BodySeq(const NotifyContentSeq&, NotifyBodySeq&);

  virtual string name() {
    return "NotifyIndexN";
  }
  virtual string endpoints() {
    return "@NotifyIndexN";
  }
  virtual size_t cluster() {
    return 5;
  }

  vector<NotifyIndexManagerNPrx> _managersUDP;
  vector<NotifyIndexManagerNPrx> _managersOneway;
  vector<NotifyIndexManagerNPrx> _managers;
};

}
}

#endif // _NOTIFY_INDEX_ADAPTER_H_
