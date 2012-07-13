#ifndef __OFFERFRIENDS_LOADER_I_H__
#define __OFFERFRIENDS_LOADER_I_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include <OfferFriends.h>
#include "ServiceI.h"

namespace xce {
namespace offerfriends {

const static int TASK_LEVEL_APPLY = 1;
const static int TASK_LEVEL_ACCEPT = 2;
const static int TASK_LEVEL_DENY = 3;
const static int TASK_LEVEL_IGNORE = 4;
const static int TASK_LEVEL_REPORT = 5;
const static int TASK_LEVEL_REBUILD_NOW = 6;
const static int TASK_LEVEL_REBUILD_LATER = 7;
const static int TASK_LEVEL_RELOAD = 8;
const static int TASK_LEVEL_RELOAD_BATCH = 9;

const static int GUIDE_REBUILD_THRESHOLD = 10;
const static long GUIDE_REBUILD_TIMEOUT = 60;
const static int HOME_REBUILD_THRESHOLD = 70;
const static long HOME_REBUILD_TIMEOUT = 86400;

//**************************************************************************************************************

class OfferFriendsLoaderNI: virtual public OfferFriendsLoaderN, public MyUtil::Singleton<OfferFriendsLoaderNI>{
public:
	friend class MyUtil::Singleton<OfferFriendsLoaderNI>;

	virtual void apply(::Ice::Int, 
										 ::Ice::Int,
										 const Ice::Current&);

  virtual void accept(::Ice::Int, 
											::Ice::Int,
											const Ice::Current&);

  virtual void denyOffer(::Ice::Int, 
												 ::Ice::Int,
												 const Ice::Current&);

  virtual void ignoreOffers(::Ice::Int, 
														const MyUtil::IntSeq&,
														const Ice::Current&);

	void report(const ::xce::offerfriends::GetInfo&,
							const Ice::Current&);

	virtual void rebuild(::Ice::Int, 
											 bool,
											 const Ice::Current&);

	virtual void rebuildBatch(const MyUtil::IntSeq&, 
													  bool,
														const Ice::Current&);

	virtual std::string debug(::Ice::Int,
														const Ice::Current&);

	virtual void reload(::Ice::Int,
											const Ice::Current&);

	virtual void reloadBatch(const MyUtil::IntSeq&,
													 const Ice::Current&);
};

//**************************************************************************************************************

class ApplyTask: public MyUtil::Task{
public:
	ApplyTask(Ice::Int applicant, Ice::Int acceptor, int level=TASK_LEVEL_APPLY):
		Task(level), _applicant(applicant), _acceptor(acceptor){ };
	virtual void handle();

private:
	Ice::Int _applicant;
	Ice::Int _acceptor;
};

//**************************************************************************************************************

class AcceptTask: public MyUtil::Task{
public:
	AcceptTask(Ice::Int acceptor, Ice::Int applicant, int level=TASK_LEVEL_ACCEPT):
		Task(level), _acceptor(acceptor), _applicant(applicant){ };
	virtual void handle();

private:
	Ice::Int _acceptor;
	Ice::Int _applicant;
};

//**************************************************************************************************************

class DenyTask: public MyUtil::Task{
public:
	DenyTask(Ice::Int hostId, Ice::Int guestId, int level=TASK_LEVEL_DENY):
		Task(level), _hostId(hostId), _guestId(guestId) { };
	virtual void handle();

private:
	Ice::Int _hostId;
	Ice::Int _guestId;
};

//**************************************************************************************************************

class IgnoreTask: public MyUtil::Task{
public:
	IgnoreTask(Ice::Int userId, const MyUtil::IntSeq& ids, int level=TASK_LEVEL_IGNORE):
		Task(level), _userId(userId), _ids(ids) { };
	virtual void handle();

private:
	Ice::Int _userId;
	MyUtil::IntSeq _ids;
};

//**************************************************************************************************************

class ReportTask: public MyUtil::Task{
public:
	ReportTask(::xce::offerfriends::GetInfo info, int level=TASK_LEVEL_REPORT):
		Task(level), _info(info) { };
	virtual void handle();

private:
	::xce::offerfriends::GetInfo _info;
};

//**************************************************************************************************************

class RebuildNowTask: public MyUtil::Task{
public:
	RebuildNowTask(Ice::Int userId, int level=TASK_LEVEL_REBUILD_NOW):
		Task(level), _userId(userId) { };
	virtual void handle();

private:
	Ice::Int _userId;
};

//**************************************************************************************************************

class RebuildLaterTask: public MyUtil::Task{
public:
	RebuildLaterTask(Ice::Int userId, int level=TASK_LEVEL_REBUILD_LATER):
		Task(level), _userId(userId) { };
	virtual void handle();

private:
	Ice::Int _userId;
};

//**************************************************************************************************************

class ReloadTask: public MyUtil::Task{
public:
	ReloadTask(Ice::Int userId, int level=TASK_LEVEL_RELOAD):
		Task(level), _userId(userId) { };
	virtual void handle();

private:
	Ice::Int _userId;
};

//**************************************************************************************************************

class ReloadBatchTask: public MyUtil::Task{
public:
	ReloadBatchTask(const MyUtil::IntSeq& userIds, int level=TASK_LEVEL_RELOAD_BATCH):
		Task(level), _userIds(userIds) { };
	virtual void handle();

private:
	MyUtil::IntSeq _userIds;
};

//**************************************************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
	public:
		virtual void configure(const Ice::PropertiesPtr& props);
};

//**************************************************************************************************************

}
}

#endif
