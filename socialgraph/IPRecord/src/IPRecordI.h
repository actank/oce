#ifndef __IPRECORDI_H__
#define __IPRECORDI_H__

#include <IPRecord.h>
#include "IPRecordI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace iprecord {

const int IPRECORD_LIMIT = 100;

const int IPRECORD_DATA = 0;

const int TASK_LEVEL_UPDATE = 1;

////***************************************************************************

class IPRecordData: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	::MyUtil::IntSeq get(int limit);
	::MyUtil::IntSeq getWithMask(int limit,::MyUtil::IntSeq mask);
	int size();
	void add(::Ice::Long ip,::Ice::Int userId);
	void remove(::Ice::Long ip,::Ice::Int userId);
	::std::vector<int> _users;
//private:
	//IceUtil::Mutex _recordMutex;
        //time_t _lastRecordTime;
	//int _recordCount;
};
typedef IceUtil::Handle<IPRecordData> IPRecordDataPtr;

class IPRecordDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Long ip);
};

class IPRecordDataResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
	public:
		IPRecordDataResultHandler(const IPRecordDataPtr& result):_result(result) {}
	protected:
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		IPRecordDataPtr _result;
};

class IPRecordManagerI: virtual public IPRecordManager,
                virtual public MyUtil::Singleton<IPRecordManagerI>{
public:
	virtual MyUtil::IntSeq getUsers(::Ice::Long,::Ice::Int,
			const Ice::Current&);
	virtual MyUtil::IntSeq getUsersWithMask(::Ice::Long,::Ice::Int,const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual ::Ice::Int getCount(::Ice::Long,
			const Ice::Current&);
	virtual void addUser(::Ice::Long,::Ice::Int,
			const Ice::Current&);
	virtual void removeUser(::Ice::Long,::Ice::Int,
			const Ice::Current&);
	virtual void reload(::Ice::Long,
			const Ice::Current&);
	virtual void updateToDB(::Ice::Long,
			const Ice::Current&);
private:
	IPRecordDataPtr locateIPRecordData(::Ice::Long);
};

class UpdateToDBTask : virtual public MyUtil::Task
{
	public:
		UpdateToDBTask( ::Ice::Long ip, const std::vector<int>& users, ::Ice::Int level=TASK_LEVEL_UPDATE ) :
			Task(level),_ip(ip),_users(users) {
			}
	protected:
		virtual void handle();
	private:
		::Ice::Long _ip;
		std::vector<int> _users;
};

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}

}

#endif
