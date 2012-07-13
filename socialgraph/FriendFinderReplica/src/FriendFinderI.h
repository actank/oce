#ifndef FRIENDFINDERI_H_
#define FRIENDFINDERI_H_
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "CommonFriendFinder.h"
#include "ObjectCacheI.h"
#include "TaskManager.h"

namespace com {
namespace xiaonei {
namespace service {

const static int COMMON_FRIEND_SET = 0;

const static int TASK_LEVEL_REBUILD_LATER = 1;
const static int TASK_LEVEL_REBUILD_NOW = 2;
const static int TASK_LEVEL_REBUILD_CREATE = 3;
const static int TASK_LEVEL_REBUILD_NORMAL = 4;

const static int TASK_LEVEL_REMOVE = 5;
const static int TASK_LEVEL_RECORD = 6;

const static size_t CF_SAVE_THRESHOLD = 100;
const static size_t CF_REBUILD_THRESHOLD = 70;
const static size_t CF_WEBPAGE_THRESHOLD = 6;

const static std::string REBUILD_IMMEDIATELY = "REBUILD_IMMEDIATELY";
const static std::string REMOVE_WITH_BLOCK = "REMOVE_WITH_BLOCK";
const static std::string RANDOM_COMMON_FRIEND = "RANDOM_COMMON_FRIEND";

static int rebuildInterval = 24*3600;

typedef std::vector<CommonFriend> CommonFriendSeq;

class CommonFriendBuilderTask: public MyUtil::Task {
public:
	CommonFriendBuilderTask(Ice::Int id, int level) :
		MyUtil::Task(level), _id(id) {
		}
	virtual void handle();
private:
	Ice::Int _id;
};

//*********************************************************************************

class CommonFriendSet: virtual public Ice::Object,
		virtual public IceUtil::Mutex {
public:
	CommonFriendSet(const CommonFriendSeq& items = CommonFriendSeq()) :
		_items(items), _ready(true), _timestamp(time(NULL)) {
			_items.resize(_items.size() > CF_SAVE_THRESHOLD ? CF_SAVE_THRESHOLD
					: _items.size());
			CommonFriendSeq(_items).swap(_items);
		}
	CommonFriendSeq getItems() {
		if (!_ready) {
			throw NotReadyException();
		}
		IceUtil::Mutex::Lock lock(*this);
		return _items;
	}
	void checkTimeout( int id ) {
		if( time(NULL) - _timestamp > rebuildInterval ) {
			IceUtil::Mutex::Lock lock(*this);
			if ((_items.empty() && (id < 100000000 || id > 300000000)) || 
					(!_items.empty() && _items.size() < CF_WEBPAGE_THRESHOLD) ) {
				MCE_INFO("checkTimeout-> normal user, id=" << id << ", _items.size()=" << _items.size());
				MyUtil::TaskManager::instance().execute(
						new CommonFriendBuilderTask(id, TASK_LEVEL_REBUILD_NORMAL));
			} else {
				MCE_INFO("checkTimeout -> old user, id=" << id);
				MyUtil::TaskManager::instance().execute(
						new CommonFriendBuilderTask(id, TASK_LEVEL_REBUILD_LATER));
			}
			_timestamp=time(NULL);
		}
	}
	CommonFriendSeq getItems(int id, int begin, int limit) {
		if (!_ready) {
			throw NotReadyException();
		}

		checkTimeout( id );
		IceUtil::Mutex::Lock lock(*this);
		size_t realBegin = begin < 0 ? 0 : begin;
		realBegin = realBegin < _items.size() ? realBegin : 0;
		size_t realLimit = limit < 0 ? _items.size() - begin : limit;
		realLimit = realLimit < _items.size() - begin ? realLimit
			: _items.size() - begin;
		CommonFriendSeq result;
		for (CommonFriendSeq::iterator it = _items.begin(); it != _items.end(); ++it) {
			if (realBegin > 0) {
				realBegin--;
				continue;
			}
			result.push_back(*it);
			if (result.size() >= realLimit) {
				break;
			}
		}
		MCE_INFO("CommonFriendSet [ORDERD] " << id << " TotalSize:"
				<< _items.size() << " Begin:" << limit << " RealBegin:"
				<< realBegin << " Limit:" << limit << " RealLimit:"
				<< realLimit << " ResultSize:" << result.size());
		return result;
	}
	CommonFriendSeq getItems(int id, int limit) {
		if (!_ready) {
			throw NotReadyException();
		}
		checkTimeout( id );
		IceUtil::Mutex::Lock lock(*this);
		if ((int) _items.size() <= limit) {
			return _items;
		}
		map<int, int> randomMap;
		vector<int> randomList;
		while ((int) randomMap.size() < limit) {
			int pos = rand() % _items.size();
			if (!randomMap.count(pos)) {
				randomMap.insert(make_pair(pos, 1));
				randomList.push_back(pos);
			}
		}
		CommonFriendSeq result;
		for (vector<int>::iterator it = randomList.begin(); it
				!= randomList.end(); ++it) {
			result.push_back(_items.at(*it));
		}
		MCE_INFO("CommonFriendSet [RANDOM] " << id << " TotalSize:"
				<< _items.size() << " Limit:" << limit << " ResultSize:"
				<< result.size());
		return result;
	}
	size_t size() const {
		IceUtil::Mutex::Lock lock(*this);
		return _items.size();
	}
	void setItems(const CommonFriendSeq& items) {
		IceUtil::Mutex::Lock lock(*this);
		CommonFriendSeq(items).swap(_items);
		_ready = true;
	}
	void removeItem(int id) {
		IceUtil::Mutex::Lock lock(*this);
		for (CommonFriendSeq::iterator it = _items.begin(); it != _items.end(); ++it) {
			if (it->userId == id) {
				_items.erase(it);
				return;
			}
		}
	}
private:
	CommonFriendSeq _items;
	bool _ready;
	time_t _timestamp;
};

typedef IceUtil::Handle<CommonFriendSet> CommonFriendSetPtr;

//*********************************************************************************

class CommonFriendSetFactory: public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long id);
};

//*********************************************************************************

class FriendFinderManagerI: public ::com::xiaonei::service::FriendFinderManager, public MyUtil::Singleton<FriendFinderManagerI> {
protected:
	friend class MyUtil::Singleton<FriendFinderManagerI>;
public:
	virtual ::com::xiaonei::service::CommonFriendSeq getCommonFriend(
			::Ice::Int userId, ::Ice::Int begin, ::Ice::Int limit,
			const ::Ice::Current&);
	virtual void reloadCommonFriend(::Ice::Int userId, const ::Ice::Current&);
	virtual void removeCommonFriend(::Ice::Int userId, ::Ice::Int friendId,
			const ::Ice::Current&);
	virtual void rebuildCommonFriend(::Ice::Int userId, const ::Ice::Current&);

	virtual MyUtil::IntSeq getSharedFriend(::Ice::Int userId1,
			::Ice::Int userId2, ::Ice::Int begin, ::Ice::Int limit,
			const ::Ice::Current&);
	virtual ::Ice::Int getSharedFriendCount(::Ice::Int userId1,
			::Ice::Int userId2, const ::Ice::Current&);
	virtual ::com::xiaonei::service::Int2ShareFriendMap getSharedFriends(
			::Ice::Int fromId, const MyUtil::IntSeq& toIds, ::Ice::Int limit,
			const ::Ice::Current&);
private:
	MyUtil::IntSeq calc_shares(const MyUtil::IntSeq& f1, const MyUtil::IntSeq& f2);
};

//*********************************************************************************

class CommonFriendSetResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	CommonFriendSetResultHandler(CommonFriendSeq& results);
	virtual ~CommonFriendSetResultHandler();
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	CommonFriendSeq& _results;
};

//*********************************************************************************

class FriendStatusResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	FriendStatusResultHandler( CommonFriendSeq& results );
	virtual ~FriendStatusResultHandler();
protected:
	virtual bool handle( mysqlpp::Row& row ) const;
private:
	CommonFriendSeq&	_results;
};

//*********************************************************************************

class FriendFinderServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

// --------- CreateTask ---------------------------------------------------
class CreateTask: public MyUtil::Task{
public:
	CreateTask(Ice::Int id, int level = TASK_LEVEL_REBUILD_CREATE):
		Task(level),
		_id(id)  {
		};
	virtual void handle();
private:
	Ice::Int _id;
};

// --------- CommonFriendRemoveTask ---------------------------------------------------
class CommonFriendRemoveTask: public MyUtil::Task{
public:
	CommonFriendRemoveTask(Ice::Int userId, Ice::Int friendId, bool block):
		Task(TASK_LEVEL_REMOVE),_userId(userId),_friendId(friendId),_block(block)  {
		};
	virtual void handle();
private:
	Ice::Int _userId;
	Ice::Int _friendId;
	bool _block;
};

// --------- CommonFriendRecordTask ---------------------------------------------------
class CommonFriendRecordTask: virtual public MyUtil::Task {
public:
	CommonFriendRecordTask(Ice::Int userId, const CommonFriendSeq& cfs,::Ice::Int level=TASK_LEVEL_RECORD):
		MyUtil::Task(level),_userId(userId), _cfs(cfs) {
		}
	virtual void handle();
private:
	Ice::Int _userId;
	CommonFriendSeq _cfs;
};



}
}
}
#endif /*FRIENDFINDERI_H_*/
