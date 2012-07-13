#ifndef __NOTIFY_I_H__
#define __NOTIFYs_I_H__

#include "Dispatch.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Channel.h"
#include "Date.h"
#include "ServiceAdapter.h"

#include <Ice/Ice.h>
#include <Ice/Exception.h>
#include <Freeze/Freeze.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>
#include "TaskManager.h"
using namespace MyUtil;

namespace mop {
namespace hi {
namespace svc {
namespace dispatch {

const Ice::Int NOTE_SET = 0;
const string NOTE_WRAPPER = "NW";
const string NOTIFIER = "N";
const int ALLTYPE = -1;

const Ice::Int TYPE_NOTE = 0;

typedef deque<HintPtr> HintQueue;

class NoteSet;
typedef IceUtil::Handle<NoteSet> NoteSetPtr;

//---------------------------------------------------------------------------
class ServantFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

//---------------------------------------------------------------------------
class NotifierI : virtual public Notifier, public Singleton<NotifierI> {
public:
	NotifierI();
	virtual ~NotifierI();

	virtual MyUtil::IntSeq notify(const NotePtr& theNote,
			const MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());

	virtual Ice::Int getCount(Ice::Int userId, Ice::Int type, bool unreadOnly,
			const Ice::Current& = Ice::Current());
	virtual NoteSeq getAll(Ice::Int userId, Ice::Int type, bool unreadOnly,
			Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());
	virtual NoteResultSetPtr getResultSet(Ice::Int userId, Ice::Int type,
			bool unreadOnly, Ice::Int begin, Ice::Int limit,
			const Ice::Current& = Ice::Current());
	virtual void setRead(Ice::Int userId, Ice::Int resource, Ice::Int type,
			const Ice::Current& = Ice::Current());
	virtual void setAllRead(Ice::Int userId,
			const Ice::Current& = Ice::Current());

	NoteSetPtr getNoteSet(Ice::Int userId);

	string getDatabase();
	string getDatabaseOld();

	string getNoteTable();
	string getNoteDispatchedTable(int ownerId);

	int getNoteDispatchedTableCount();
	int getNoteMaxTTL();
	int getNoteMaxSize();
	bool isHomotypic(Ice::Int source, Ice::Int target);
private:
	string _noteTable;
	string _database;
	string _databaseOld;
	string _noteDispatchedTable;
	string _noteDispatchedTableOld;
	int _noteDispatchedTableCount;
	int _noteMaxTTL;
	int _noteMaxSize;
	vector<IntSeq> _noteHomotypes;
};

//---------------------------------------------------------------------------
class NoteSet : virtual public Ice::Object, virtual public IceUtil::Mutex {
	friend class ServantFactoryI;
public:
	int getCount(Ice::Int type, bool unreadOnly,
			const Ice::Current& = Ice::Current());
	NoteSeq getAll(Ice::Int type, bool unreadOnly, Ice::Int begin,
			Ice::Int limit, const Ice::Current& = Ice::Current());
	NoteResultSetPtr getResultSet(Ice::Int type, bool unreadOnly,
			Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());
	void setRead(Ice::Int ownerId, Ice::Int resource, Ice::Int type,
			const Ice::Current& = Ice::Current());
	void setAllRead(Ice::Int ownerId, const Ice::Current& = Ice::Current());

	void add(const NotePtr& theNote, const Ice::Current& = Ice::Current());
	void add(const HintQueue& notes);

private:
	bool isHomotypic(const HintPtr& ns, Ice::Int type, Ice::Int resource);
	bool isMatched(const HintPtr& ns, Ice::Int type, bool unreadOnly);
	void saveReads(const HintQueue& notes, Ice::Int userId);
	void evictNotes();

	HintQueue notes;
};

class SaveReadTask : public Task {
public:
	SaveReadTask(const string& table, const string& sql) :
		_table(table), _sql(sql) {
	}
	virtual void handle();
private:
	string _table;
	string _sql;
};

//-------------------------------------------------------------------------
class NotifyCacheAdapter : public AdapterI,
	public Singleton<NotifyCacheAdapter> {
public:
	void getNoteList(HintSeq& hints, NoteSeq& notes);
protected:
	virtual string initializeName() {
		return "ObjectCache";
	}
	virtual string initializeEndpoints() {
		return ServiceI::instance().getCommunicator()->getProperties()
		->getProperty("Service.ObjectCache.Endpoints");
	}
	virtual size_t initializeCluster() {
		return 1;
	}
};

}
;
}
;
}
;
}
;
#endif
