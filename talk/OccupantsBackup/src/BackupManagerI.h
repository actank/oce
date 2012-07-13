#ifndef BACKUPMANAGERI_H_
#define BACKUPMANAGERI_H_

#include "OccupantsBackup.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include "Singleton.h"
#include "XmppTools.h"
#include "ServiceI.h"

namespace talk {
namespace occupants {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;

const int ROOM_INFO = 0; 

class RoomInfo : public Ice::Object{
public:
	void put(const OccupantPtr& o);
	void remove(const RoomIdPtr& rid, const JidPtr& jid);
	void removeAll(const RoomIdPtr& rid);
	OccupantSeq get(const RoomIdPtr& rid);
private:
	map<RoomIdPtr, map<Ice::Long,OccupantPtr> > _info;
};

typedef IceUtil::Handle<RoomInfo> RoomInfoPtr;

class RoomInfoFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long hashcode);
};

class SqlObj : public IceUtil::Shared{
public:
	int op; // 0 replace, 1 delete
	OccupantPtr o;
};
typedef IceUtil::Handle<SqlObj> SqlObjPtr;
typedef map<RoomIdPtr, vector<SqlObjPtr> > SqlMap;

class BackupManagerI : public BackupManager, public Singleton<BackupManagerI> {
public:
	virtual void put(const OccupantPtr& o, const Ice::Current& = Ice::Current());
	virtual void puts(const OccupantSeq& oseq, const Ice::Current& = Ice::Current());
	virtual void remove(const RoomIdPtr& rid,const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void removeAll(const RoomIdPtr& rid, const Ice::Current& = Ice::Current());
	virtual OccupantSeq get(const RoomIdPtr& rid,
			const Ice::Current& = Ice::Current());
	
	void swapSqlObj(SqlMap& m);
private:
	void _addUpdateSqlObj(const OccupantPtr& o);
	void _addRemoveSqlObj(const RoomIdPtr& rid, const JidPtr& jid);
	void _addRemoveSqlObj(const RoomIdPtr& rid);
	
	
	SqlMap _saveSeq;
	IceUtil::Mutex _sqlMutex;
	
};

class SaveTimer : public MyUtil::Timer
{
public:
	SaveTimer(int time = 1000): Timer(time){};
	virtual void handle();
};

}
;
}
;
#endif /*BACKUPMANAGERI_H_*/
