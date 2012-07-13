#ifndef __Updater_h__
#define __Updater_h__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <iterator>
#include <set>
#include "Util.h"
#include "IceLogger.h"
namespace mop {
namespace hi {
namespace svc {
namespace model {
using namespace std;
using namespace MyUtil;

const string OP_PLUS = " + ";
const string OP_MINUS = " - ";

template<class IdType, class ValueType>
class BasicUpdater: virtual public IceUtil::Thread {
protected:
	typedef map<IdType, ValueType> ValueMap;
typedef	typename ValueMap::iterator ValueMapIterator;
	ValueMap _values;

	typedef set<IdType> IdSet;
	typedef map<ValueType, IdSet> IdSetMap;
	IdSetMap _idSets;

	string _database;
	string _table;
	string _valueColumn;
	StrSeq _idColumns;

	IceUtil::Monitor<IceUtil::Mutex> _mutex;

public:
	BasicUpdater(string database,
			string table,
			string valueColumn,
			StrSeq idColumns,
			string op = " + ") :
	_database(database),
	_table(table),
	_valueColumn(valueColumn),
	_idColumns(idColumns)
	{
		start().detach();
	};

	virtual ~BasicUpdater() {};

	void fix(const IdType& id, const ValueType& value)
	{		
		//MCE_DEBUG("Count: " << id << " -> " << value);
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		// check whether id has already existed in the map,
		// if exists, then remove it
		// from orignal position and reassigns it to another position,
		// else, add it to the map
		ValueMapIterator pos = _values.find(id);
		if (pos == _values.end()) {
			_values[id] = value;
			_idSets[value].insert(id);
			//MCE_DEBUG("CountSet: " << value << " -> " << _count2SetMap[value].size());
		} else {
			ValueType origCount = pos->second;
			//MCE_DEBUG("CountOrig: " << id << " -> " << origCount);
			_idSets[origCount].erase(id);
			//MCE_DEBUG("CountSet: " << origCount << " -> " << _count2SetMap[origCount].size());
			if (_idSets[origCount].empty()) {
				_idSets.erase(origCount);
			}
			ValueType newCount = calc(origCount, value);
			_values[id] = newCount;
			_idSets[newCount].insert(id);
			//MCE_DEBUG("CountSet: " << newCount << " -> " << _count2SetMap[newCount].size());
		}
		// notify
		//if (_values.size() == 1) {
		_mutex.notify();
		//}
	};

private:
	virtual ValueType calc(const ValueType& origValue, const ValueType& value) = 0;
	virtual void fixSave(IdSet& ids, const ValueType& value) = 0;
	virtual void trim(IdSetMap& idSets) = 0;

	virtual void run()
	{
		while (true)
		{
			MCE_DEBUG("thread run");
			IdSetMap idSets;
			while (true) {
				// swap _count2SetMap to _count2SetMapSwap and clear _id2CountMap
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				if (_values.empty()) {
					try {
						_mutex.wait();
					} catch (...) {
						// do nothing
					}
					continue;
				}
				_values.clear();
				_idSets.swap(idSets);
				break;
			}
			trim(idSets);
		};
	};
};

};
};
};
};

#endif

