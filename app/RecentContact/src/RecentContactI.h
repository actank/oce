#ifndef __RECENTCONTACT_I_H__
#define __RECENTCONTACT_I_H__

#include "RecentContact.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "Date.h"
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>
#include "util/cpp/IntSeqSerialize.h"

namespace xce {
namespace recentcontact {

const static int TASK_LEVEL_BUILD = 1;
const int RECENTCONTACT_SET = 0;
const static string COLUMN_HOST = "owner_id";
const static string COLUMN_GUESTS = "guests";
const static string DB_SOURCE = "recent_contact";
const static int LIMIT_COUNT = 10;
const static int GUESTS_SIZE = 10;

// sort by time
class SortContactInfo {
public:
	static bool OrderByTimeDesc(const ContactInfoPtr& p1, const ContactInfoPtr& p2) {
		return p1->time < p2->time;
	}
};

class GetProps:public MyUtil::Singleton<GetProps>{
public:
	GetProps();
	string getTableName(int ownerId);
private:
	int _tableCount;
        string _tableName;
};

class RecentContactSet :  virtual public Ice::Object { 
public:
	RecentContactSet():_recent_count_limit_time(time(NULL)),_recent_count_limit_count(0){};
	ContactInfoSeq get(ContactType type, Ice::Int begin, Ice::Int limit);
	ContactInfoSeq& getContactInfos();
	void add(ContactInfoSeq::const_iterator it);
	void remove(int ownerId,int guestId);
	bool isEqual(ContactInfoSeq::const_iterator it, ContactInfoSeq::iterator itMem);
	bool isReachRecentCountLimit();
	void updateDb(int ownerId,const ContactInfoSeq& contactInfos);
private:
	IceUtil::Mutex _mutex;
	int _recent_count_limit_time;
	int _recent_count_limit_count;
	ContactInfoSeq _contactInfos;
};

typedef IceUtil::Handle<RecentContactSet> RecentContactSetPtr;

class RecentContactFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int ownerId);
};

class RecentContactSetHandler : public com::xiaonei::xce::ResultHandler {
public:
	RecentContactSetHandler(int ownerId, RecentContactSetPtr& recConSetPtr):_recConSetPtr(recConSetPtr),_ownerId(ownerId){};
	virtual bool handle(mysqlpp::Row& row) const;
	ContactInfoPtr buildContactInfo(mysqlpp::Row& row) const;
private:
	RecentContactSetPtr _recConSetPtr;
	int _ownerId;
};

class RecentContactManagerI : public RecentContactManager ,
	public MyUtil::Singleton<RecentContactManagerI> {
public:
	RecentContactManagerI();
	virtual ~RecentContactManagerI();
	virtual ContactInfoSeq get(int ownerId,const ContactType type, int begin, int limit, const Ice::Current& crt=Ice::Current());
        virtual void add(const ContactInfoSeq& contactInfos, const Ice::Current& crt=Ice::Current());
        virtual void remove(int ownerId, int guestId, const Ice::Current& crt=Ice::Current());
	//string getTableName(int ownerId);
	RecentContactSetPtr getRecentContactSet(int ownerId);
private:
	//int _tableCount;
	//string _tableName;
};

typedef IceUtil::Handle<RecentContactManagerI> RecentContactManagerIPtr;

};
};

#endif
