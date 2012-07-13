#ifndef __ROOMPOOL_H__
#define __ROOMPOOL_H__
#include <map>
#include <set>
#include <string>
#include <Ice/Ice.h>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucTalkRoom.h"
#include "XmppTools.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "Date.h"
#include "QueryRunner.h"
#include "ConnectionQuery.h"


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>



namespace com{
namespace xiaonei{
namespace talk{
namespace muc{

const int ROOMSIZE = 10000;
const string DB_INSTANCE = "minigroup";
const string MUCDOMAIN = "group.talk.xiaonei.com";

using boost::multi_index_container;
using namespace boost::multi_index;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::xce;

typedef set<int> MemberSet;
typedef set<int>::iterator MemberSetIt;
typedef pair<MemberSetIt, bool> InsertRes;

class RoomMemberList : public IceUtil::Shared{
public:
  RoomMemberList(string roomname, int groupid){
    _roomname = roomname;
    _groupid = groupid;
	}
  bool KickMember(int userid);
  bool AddMember(int userid);
  IntSeq GetAllMembersSeq();
	string _roomname;
	int _groupid;
	MemberSet _members;
};
typedef IceUtil::Handle<RoomMemberList> RoomMemberListPtr;


typedef multi_index_container<
  RoomMemberListPtr,
  indexed_by<
		sequenced<>,
		hashed_unique< member<RoomMemberList, int, &RoomMemberList::_groupid> >
	>
> RoomMemberCache;

typedef RoomMemberCache::nth_index<0>::type SequencedIndex;
typedef RoomMemberCache::nth_index<1>::type GroupIdIndex;



class RoomPool : public Ice::Object{
protected:
	//int GetRoomId(const string& roomname);
	string GetDbTableName(int irid);
  void AddRoom(const RoomMemberListPtr& rmlptr);
  RoomMemberListPtr SelectRoomMembers(const MucRoomIdPtr& roomid);
  RoomMemberListPtr GetMemberList(const MucRoomIdPtr& roomid);
public:
  void CreateRoom(const MucRoomIdPtr& roomid);
  void RemoveRoom(const MucRoomIdPtr& roomid);
  void AddRoomMember(int userid, const MucRoomIdPtr& roomid);
  void RemoveRoomMember(int userid, const MucRoomIdPtr& roomid);
  IntSeq GetRoomMember(const MucRoomIdPtr& roomid);
  MucActiveUserSeq GetRoomActiveUserSeq(const MucRoomIdPtr& roomid);
	void PageNotifyOnline(int onlineuser, const MucRoomIdPtr& roomid);
	void PageNotifyOffline(int offlineuser, const MucRoomIdPtr& roomid);
private:
	IceUtil::RWRecMutex _rwmutex;
	IceUtil::Mutex _cachemutex;
	RoomMemberCache _cache;

};
typedef IceUtil::Handle<RoomPool> RoomPoolPtr;


};
};
};
};

#endif
            
