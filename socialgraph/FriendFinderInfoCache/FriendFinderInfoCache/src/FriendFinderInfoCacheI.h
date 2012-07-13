#ifndef _FRIENDFINDER_INFOCACHEI_H_	
#define _FRIENDFINDER_INFOCACHEI_H_ 

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>
#include "NetworkCollector.h"
#include "FriendFinderInfoCache.h"
#include "MD5Util.h"
#include "FriendFinderInfoWorkerReplicaAdapter.h"
#include "loadpeerthread.h"
#include "OceCxxAdapter/src/SameAreaPeerAdapter.h"

using namespace xce::socialgraph;

namespace xce {
namespace friendfinder {

const int BIRTHDAY_DATA = 0;
const int BIRTHDAY_INDEX = 0;

const int BIRTHDAY_DATA_BACKEND = 7;
const int BIRTHDAY_INDEX_BACKEND = 7;

const int WORKPLACE_DATA = 1;
const int UNIV_DATA = 2;
const int HIGHSCHOOL_DATA = 3;
const int JUNIORSCHOOL_DATA = 4;
const int ELEMENTARYSCHOOL_DATA = 5;

const int NETWORK_DATA = 6;

const int SCHOOL_AREA = 8;  //为中学推人而做

const int rebuildInterval = 7*24*3600;                  //每周重算

const int EACH_ITEM_SIZE = 300;  //高中推人每个阶段最多返回300个id

//*************************************************************************************************

class BirthdayInfoData : virtual public Ice::Object, virtual public IceUtil::Mutex {     
public:
	void set(int birthYear, const std::vector<int>& orderedIds);	
	std::vector<int> getBySimilarYear(int birthYear, const std::vector<int>& friendIds);
	int getUserBirthyear(int userId);

private:
	IceUtil::RWRecMutex _init_flag_mutex;
	std::map< int, std::vector<int> > _birthdayMap;	
};
typedef IceUtil::Handle<BirthdayInfoData> BirthdayInfoDataPtr;

//*************************************************************************************************

class RebuildNetworkTask : virtual public MyUtil::Task {
public:
	RebuildNetworkTask(Ice::Int id): _id(id) { };
	virtual void handle();

private:
	Ice::Int _id;
}; 

//*************************************************************************************************
/*存放进ObjectCache中的对象*/
class SchoolAreaData: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	SchoolAreaData(::Ice::Int hostid, const SchoolArea& data):
		hostid_(hostid), data_(data) {}

	Md5Info GetHighschoolInfo() {
		IceUtil::Mutex::Lock lock(*this);
		return data_.highschool;
	}

	Md5Info GetJuniorschoolInfo() {
		IceUtil::Mutex::Lock lock(*this);
		return data_.juniorschool;
	}

	Md5Info GetCollegeInfo() {
		IceUtil::Mutex::Lock lock(*this);
		return data_.college;
	}

private:
	int hostid_;
	SchoolArea data_;	
};
typedef IceUtil::Handle<SchoolAreaData> SchoolAreaDataPtr;

//*************************************************************************************************

class NetworkData : virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	NetworkData(::Ice::Int ownerId, const Network& data):
		_ownerId(ownerId), _data(data), _timeStamp(time(NULL)) { };

	Network get() { 
		//MCE_INFO("[NetworkData] get");
		IceUtil::Mutex::Lock lock(*this);
		checkTimeOut();
		return _data;	
	};

	InfoSeq getUnivSeq() {
		//MCE_INFO("[NetworkData] getUnivSeq");
		IceUtil::Mutex::Lock lock(*this);
		checkTimeOut();
		return _data.univInfoSeq;
	};

	InfoSeq getHighschoolSeq() {
		//MCE_INFO("[NetworkData] getHighschoolSeq");
		IceUtil::Mutex::Lock lock(*this);
		checkTimeOut();
		return _data.highschoolInfoSeq;
	};

	InfoSeq getJuniorschoolSeq() {
		//MCE_INFO("[NetworkData] getJuniorschoolSeq");
		IceUtil::Mutex::Lock lock(*this);
		checkTimeOut();
		return _data.juniorschoolInfoSeq;
	};

	InfoSeq getElementaryschoolSeq() {
		//MCE_INFO("[NetworkData] getElementaryschoolSeq");
		IceUtil::Mutex::Lock lock(*this);
		checkTimeOut();
		return _data.elementaryschoolInfoSeq;
	};

	void checkTimeOut() {
		if ( time(NULL) - _timeStamp > rebuildInterval ) { //TO-DO
			MCE_INFO("[NetworkData] checkTimeOut ownerId(" << _ownerId << ")");
			MyUtil::TaskManager::instance().execute( new RebuildNetworkTask(_ownerId) );
			_timeStamp = time(NULL);
		}
	};	

private:
	::Ice::Int _ownerId;
	Network _data;
	time_t _timeStamp;	
};
typedef IceUtil::Handle<NetworkData> NetworkDataPtr;

//*************************************************************************************************

class FriendFinderInfoCacheI: virtual public FriendFinderInfoCache,
		virtual public MyUtil::Singleton<FriendFinderInfoCacheI> {
public:
	virtual NetworkInfo getNetworkInfo(::Ice::Int, const Ice::Current&);
	virtual NetworkInfo getNetworkInfoWithHostage(::Ice::Int, const Ice::Current&);
	virtual NetworkInfo getNetworkInfoWithHostageFromDB(::Ice::Int, const Ice::Current&);
	virtual HighSchoolInfo getHighSchoolInfo(::Ice::Int, const Ice::Current&);

	virtual MyUtil::IntSeq getWorkplaceData(const MyUtil::StrSeq&, const Ice::Current&);
	virtual	MyUtil::IntSeq getUnivData(const MyUtil::StrSeq&, const Ice::Current&);
	virtual MyUtil::IntSeq getHighschoolData(const MyUtil::StrSeq&, const Ice::Current&);
	virtual MyUtil::IntSeq getJuniorschoolData(const MyUtil::StrSeq&, const Ice::Current&);
	virtual MyUtil::IntSeq getElementaryschoolData(const MyUtil::StrSeq&, const Ice::Current&);
	virtual int getBirthdayData(::Ice::Int, const Ice::Current&);

	virtual void setWorkplaceDataWithMask(const MyUtil::LongSeq&, const InfoDataSeq&, const Ice::Current&);
    virtual void setUnivDataWithMask(const MyUtil::LongSeq&, const InfoDataSeq&, const Ice::Current&);
    virtual void setHighschoolDataWithMask(const MyUtil::LongSeq&, const InfoDataSeq&, const Ice::Current&);
    virtual void setJuniorschoolDataWithMask(const MyUtil::LongSeq&, const InfoDataSeq&, const Ice::Current&);
    virtual void setElementaryschoolDataWithMask(const MyUtil::LongSeq&, const InfoDataSeq&, const Ice::Current&);

	virtual void setBirthdayData(::Ice::Int, const MyUtil::IntSeq&, const Ice::Current&);

	virtual void rebuild(::Ice::Int, const Ice::Current&);

 	void getUnivItemSeq(int userId, ItemSeq& items);
 	void getUnivItemSeqWithHostage(int userId, ItemSeq& items);
	void getHighschoolItemSeq(int userId, ItemSeq& items);
	void getJuniorschoolItemSeq(int userId, ItemSeq& items);
	void getElementaryschoolItemSeq(int userId, ItemSeq& items);


protected:
	friend class MyUtil::Singleton<FriendFinderInfoCacheI>;

private:
	void Transfer(const std::vector<PItem>& resource, RecommendItemSeq& dest);
	void getPeopleAge(int userId, int& userAge);
	void getPeopleAgeBatch(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& userAgeMap);
	void getPeopleAgeFromDB(int userId, int& userAge);
	void getPeopleAgeBatchFromDB(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& userAgeMap);
	
	void getFriendsWithAge(int userId, MyUtil::IntSeq& resultSeq, const MyUtil::IntSeq& sourceSeq);

	NetworkDataPtr locateNetworkData(::Ice::Int id);

public:
        virtual ::MyUtil::IntSeq getFriendListByInfo(::Ice::Int,::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::IntSeq getInfoMatchFriendList(::Ice::Int,::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::IntSeq getFriendListByGuestInfo(::Ice::Int,::Ice::Int,::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::IntSeq getGuestInfoMatchFriendList(::Ice::Int,::Ice::Int,::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::IntSeq getGuestInfoMatchFriendListBatch(const ::MyUtil::IntSeq&,::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::IntSeq getCommonFriendListByGuestInfo(::Ice::Int,::Ice::Int,::Ice::Int, const Ice::Current&);
private:
        void calc_shares( map<int,int>&, const MyUtil::IntSeq&, const MyUtil::IntSeq& );
	void calc_match( NetworkInfo&, const vector<int>&, map<int,int>& );
        multimap<int,int> cal_friend_byinfo( ::Ice::Int, ::Ice::Int );
	multimap<int,int> cal_friend_byinfo_batch( const ::MyUtil::IntSeq&, ::Ice::Int );
        multimap<int,int> cal_common_friend_byinfo( ::Ice::Int, ::Ice::Int );
};

//*************************************************************************************************

class AbstractFactory: virtual public MyUtil::ServantFactory {
};

//*************************************************************************************************

class NetworkDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

//*************************************************************************************************

class SchoolAreaFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

//*************************************************************************************************

class AutoLoadTask: virtual public MyUtil::Task {
public:
	AutoLoadTask(Ice::Int userId): _userId(userId) {};
	virtual void handle();
private:
	Ice::Int _userId;
};

//*************************************************************************************************

class GetUnivThread : virtual public IceUtil::Thread {
public:
	GetUnivThread(int userId, InfoSeq& item) : _userId(userId), _item(item) {}
	virtual void run();	
	
private:
	int _userId;
	InfoSeq& _item;
};

//*************************************************************************************************

class GetHighschoolThread : virtual public IceUtil::Thread {
public:
	GetHighschoolThread(int userId, InfoSeq& item) : _userId(userId), _item(item) {}
	virtual void run();	
	
private:
	int _userId;
	InfoSeq& _item;
};

//*************************************************************************************************

class GetJuniorschoolThread : virtual public IceUtil::Thread {
public:
	GetJuniorschoolThread(int userId, InfoSeq& item) : _userId(userId), _item(item) {}
	virtual void run();	
	
private:
	int _userId;
	InfoSeq& _item;
};

//*************************************************************************************************

class GetElementaryschoolThread : virtual public IceUtil::Thread {
public:
	GetElementaryschoolThread(int userId, InfoSeq& item) : _userId(userId), _item(item) {}
	virtual void run();	
	
private:
	int _userId;
	InfoSeq& _item;
};

}
}

#endif
