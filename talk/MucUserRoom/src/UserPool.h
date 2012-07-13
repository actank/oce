#ifndef __USERPOOL_H__
#define __USERPOOL_H__
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

const int USERSIZE = 10000;

const string DB_INSTANCE = "minigroup";

using boost::multi_index_container;
using namespace boost::multi_index;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::xce;


typedef map<MucRoomIdPtr, MucMemberPtr> RoomMemberMap;

class UserRoomInfo : public Ice::Object{
public:
  UserRoomInfo(int userid){
    _userid = userid;
  }
  bool AddRoomInfo(const MucRoomIdPtr& roomid, const MucMemberPtr& member);
  bool ChangeRoomInfo(const MucRoomIdPtr& roomid, const MucMemberPtr& member);
  bool RemoveRoomInfo(const MucRoomIdPtr& roomid);
  MucMemberPtr GetRoomPermision(const MucRoomIdPtr& roomid);
  MucRoomIdSeq GetUserRooms(const string& domain);
  MucRoomIdSeq GetUserRooms();
	MucRoomMemberSeq GetRoomAndMember();
	int _userid;
	RoomMemberMap _roommember;
};
typedef IceUtil::Handle<UserRoomInfo> UserRoomInfoPtr;


typedef multi_index_container<
  UserRoomInfoPtr,
  indexed_by<
		sequenced<>,
		hashed_unique< member<UserRoomInfo , int, &UserRoomInfo::_userid> >
	>
> UserRoomsCache;

typedef UserRoomsCache::nth_index<0>::type SequencedIndex;
typedef UserRoomsCache::nth_index<1>::type UserIndex;



class UserPool : public Ice::Object{
protected:
  void AddUserInfo(const UserRoomInfoPtr& userinfoptr);
	UserRoomInfoPtr SelectUserInfo(int userid);
	UserRoomInfoPtr GetUserRoomInfo(int userid);
	string GetDbTableName(int userid);
	MucRoomIdPtr MakeRoomId(int roomid, const string& roomname);
	MucMemberPtr MakeMember(int userid, int permision);
public:
	MucRoomIdSeq GetUserRooms(int userid, const string& domain);
	MucRoomIdSeq GetUserRooms(int userid);
	bool AddRoom(int userid, const MucRoomIdPtr& roomid, const MucMemberPtr& member);
	bool RemoveRoom(int userid, const MucRoomIdPtr& roomid);
	MucRoomMemberSeq GetUserRoomAndMember(int userid);
	MucMemberPtr GetMemberPermision(int userid, const MucRoomIdPtr& roomid);
	void SetUserPermision(int targetuser, const MucRoomIdPtr& roomid, Permisions permision);

private:
	IceUtil::RWRecMutex _rwmutex;
	IceUtil::Mutex _cachemutex;
	UserRoomsCache _cache;
};
typedef IceUtil::Handle<UserPool> UserPoolPtr;




};
};
};
};

#endif
