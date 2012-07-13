#ifndef BUDDYCORESNAPSHOTI_H_
#define BUDDYCORESNAPSHOTI_H_

#include "BuddyCore.h" // Slice
#include "ServiceI.h"
#include "../../BuddyEntry/src/BuddyEntry.h" // Should I copy some code instead ?
#include <IceUtil/IceUtil.h>

namespace com {
namespace xiaonei {
namespace service {
namespace buddy {

const static int CATEGORY_ENTRY = 0;

class BuddyCoreSnapshotI;

class LoadBootstrapThread : public IceUtil::Thread {
public:
	LoadBootstrapThread(BuddyCoreSnapshotI& manager);
	virtual void run();
private:
	BuddyCoreSnapshotI& _manager;
};
typedef IceUtil::Handle<LoadBootstrapThread> LoadBootstrapThreadPtr;

class LoadIncreamentThread : public IceUtil::Thread {
public:
	LoadIncreamentThread(BuddyCoreSnapshotI& manager);
	virtual void run();
private:
	BuddyCoreSnapshotI& _manager;
};
typedef IceUtil::Handle<LoadIncreamentThread> LoadIncreamentThreadPtr;

class EnsureRegisterThread : public IceUtil::Thread {
public:
	EnsureRegisterThread(BuddyCoreSnapshotI& manager);
	virtual void run();
private:
	BuddyCoreSnapshotI& _manager;
};
typedef IceUtil::Handle<EnsureRegisterThread> EnsureRegisterThreadPtr;

class BuddyCoreSnapshotI : virtual public BuddyCoreSnapshot {
public:
	BuddyCoreSnapshotI();
	virtual ::com::xiaonei::service::buddy::SnapshotStatus getStatus(const Ice::Current&);
	virtual void setStatus(::com::xiaonei::service::buddy::SnapshotStatus newStatus,
			const Ice::Current& = Ice::Current());
	virtual void patch(::Ice::Int, const SnapshotEntrySeqMap&, const Ice::Current&);
	virtual SnapshotEntrySeqMap getEntry4CommonFriend(::Ice::Int, const Ice::Current&);
	virtual SnapshotEntrySeqMap getEntry4ShareFriend(const ::MyUtil::IntSeq& ids, const Ice::Current&);
	virtual SnapshotEntrySeqMap getFriends(const ::MyUtil::IntSeq& ids, const Ice::Current&);

	void begin_flag(int id) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_flagsMonitor);
		_flags.push_back(id);
	}
	void end_flag(int id) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_flagsMonitor);
		for (vector<int>::iterator it = _flags.begin(); it!=_flags.end(); ++it) {
			if (id == *it) {
				_flags.erase(it);
				break;
			}
		}
		if (_flags.empty()) {
			setStatus(INITIALIZED);
		}
	}
	void setMyProxy(const BuddyCoreSnapshotPrx& proxy) {
		_myProxy = proxy;
	}

	void boot() {
		//_threadLoadBootstrap = new LoadBootstrapThread(*this);
		//_threadLoadBootstrap->start().detach();
		_threadLoadIncreament = new LoadIncreamentThread(*this);
		_threadLoadIncreament->start().detach();
		_threadEnsureRegister = new EnsureRegisterThread(*this);
		_threadEnsureRegister->start().detach();
	}
private:
	SnapshotStatus _status;
	IceUtil::RWRecMutex _statusMutex;

	vector<int> _flags;
	IceUtil::Monitor<IceUtil::Mutex> _flagsMonitor;

	multimap<int, SnapshotEntrySeqMap> _increaments;
	IceUtil::Mutex _increamentsMutex;

	BuddyCoreSnapshotPrx _myProxy;

	friend class LoadBootstrapThread;
	void loadBootstrap();
	LoadBootstrapThreadPtr _threadLoadBootstrap;

	friend class LoadIncreamentThread;
	bool loadIncreament();
	LoadIncreamentThreadPtr _threadLoadIncreament;

	friend class EnsureRegisterThread;
	void ensureRegister();
	EnsureRegisterThreadPtr _threadEnsureRegister;
};

class BuddyCoreSnapshotServiceSchemaI : public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

const static int LOAD_TASK_LEVEL = 0;

class LoadBootstrapTask : public MyUtil::Task {
public:
	LoadBootstrapTask(int minUserId, int maxUserId, BuddyCoreSnapshotI& flagManager) :
		MyUtil::Task(LOAD_TASK_LEVEL), _flagManager(flagManager), _minUserId(minUserId), _maxUserId(maxUserId) {
		_flagManager.begin_flag(_minUserId);
	}
	~LoadBootstrapTask() {
		_flagManager.end_flag(_minUserId);
	}
	virtual void handle();
private:
	BuddyCoreSnapshotI& _flagManager;
	int _minUserId;
	int _maxUserId;
};

class BootstrapEntryHandler : public com::xiaonei::xce::ResultHandler {
public:
	BootstrapEntryHandler(EntryListMap& result, const std::string& keyField, const std::string& valueField,
			const uint32_t desc);
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	EntryListMap& _result;
	std::string _keyField;
	std::string _valueField;
	uint32_t _desc;
};

class BootstrapUserHandler : public com::xiaonei::xce::ResultHandler {
public:
	BootstrapUserHandler(const std::string& field, int& maxId);
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	std::string _field;
	int& _maxId;
};

class LoadIncreamentTask : public MyUtil::Task {
public:
	LoadIncreamentTask(const MyUtil::IntSeq& ids) :
		MyUtil::Task(LOAD_TASK_LEVEL), _ids(ids) {
	}
	virtual void handle();
private:
	MyUtil::IntSeq _ids;
};

}
}
}
}
#endif /*BUDDYCORESNAPSHOTI_H_*/
