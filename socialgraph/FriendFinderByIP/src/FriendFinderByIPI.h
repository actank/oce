#ifndef __FriendFinderByIPI_h__
#define __FriendFinderByIPI_h__

#include "FriendFinderByIP.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace friendfinder {

const size_t IP_LIMIT = 10;

const int FRIENDFINDER_BYIP_DATA = 0;

const int TASK_LEVEL_ACCESS = 1;
const int TASK_LEVEL_UPDATE = 2;

////***************************************************************************

struct IPNoteI{
	IPNoteI(long ip,time_t lastRecordTime ) :
		_ip((unsigned int)ip),
		_online_count(1),
		_away_count(0),
		_last_login_time(lastRecordTime){
	}
	unsigned int _ip;
	int _online_count;
	int _away_count;
	time_t _last_login_time;
};
typedef ::std::vector<IPNoteI> IPNoteISeq;

struct greater_freq: public std::greater<IPNoteI> {
	bool operator()(const IPNoteI& a, const IPNoteI& b) const {
		int tmpA = a._online_count-a._away_count*2;
		float scoreA = tmpA>0 ? (100-(float)400/(tmpA+4)) : (float)0;
		int tmpB = b._online_count-b._away_count*2;
		float scoreB = tmpB>0 ? (100-(float)400/(tmpB+4)) : (float)0;
		return scoreA>scoreB;
	}
};

struct greater_lastlogintime: public std::greater<IPNoteI> {
	bool operator()(const IPNoteI& a, const IPNoteI& b) const {
		return a._last_login_time > b._last_login_time;
	}
};

class FriendFinderByIPData: virtual public Ice::Object {
public:
	FriendFinderByIPData() :
		_lastRecordTime(time(0)){
	}
	int getIPCount();
	IPNoteSeq getFreqIP(int limit);
	IPNoteSeq getRecIP(int limit);
	bool access(const MyUtil::LongSeq&,::Ice::Int);
	std::string serialize(std::ostringstream&);
	void addIPNoteI( const IPNoteI& );
	void setLastRecordTime( time_t lastRecordTime );
private:
	IceUtil::RWRecMutex _mutex;
        time_t _lastRecordTime;
	IPNoteISeq _freqips;
};
typedef IceUtil::Handle<FriendFinderByIPData> FriendFinderByIPDataPtr;

////***************************************************************************

class FriendFinderByIPDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

class FriendFinderByIPDataResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
public:
	FriendFinderByIPDataResultHandler(const FriendFinderByIPDataPtr& result):_result(result) {}
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	FriendFinderByIPDataPtr _result;
};

////***************************************************************************

class FriendFinderByIPManagerI: virtual public FriendFinderByIPManager,
		virtual public MyUtil::Singleton<FriendFinderByIPManagerI>{
public:
	virtual IPNoteSeq getFreqIP(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual IPNoteSeq getRecIP(::Ice::Int,::Ice::Int,
			const Ice::Current&);

	virtual void accessIP(::Ice::Int,::Ice::Long,
			const Ice::Current&);
	virtual void accessIPStr(::Ice::Int,const ::std::string&,
			const Ice::Current&);
	virtual void accessIPs(::Ice::Int,const ::MyUtil::LongSeq&,
			const Ice::Current&);
	virtual void accessIPsStr(::Ice::Int,const ::MyUtil::StrSeq&,
			const Ice::Current&);

	virtual void reload(::Ice::Int,
			const Ice::Current&);
private:
	FriendFinderByIPDataPtr locateFriendFinderByIPData(::Ice::Int);
};

////***************************************************************************

class AccessTask: virtual public MyUtil::Task {
public:
        AccessTask(::Ice::Int userId, const MyUtil::LongSeq& ips,::Ice::Int level=TASK_LEVEL_ACCESS):
                MyUtil::Task(level), _userId(userId), _ips(ips) {
        }
        virtual void handle();
private:
	Ice::Int _userId;
	MyUtil::LongSeq _ips;
};

class UpdateToDBTask : virtual public MyUtil::Task
{
public:
	UpdateToDBTask( ::Ice::Int userId, const FriendFinderByIPDataPtr& obj, ::Ice::Int level=TASK_LEVEL_UPDATE ) :
		Task(level),_userId(userId),_obj(obj) {
	}
protected:
	virtual void handle();
private:
	Ice::Int _userId;
	FriendFinderByIPDataPtr _obj;
};

////***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}

}

#endif
