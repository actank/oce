#ifndef __StateUpdater_h__
#define __StateUpdater_h__

#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "BasicUpdater.h"

namespace mop
{

namespace hi
{

namespace svc
{

namespace model
{

//class StateUpdater : virtual public BasicUpdater<Ice::Int, string>
//{
//public:
//	StateUpdater(
//			string database,
//			string table,
//			string valueColumn,
//			StrSeq& idColumns);
//
//protected:
//    virtual string calc(const string& old, const string& inc);
//    virtual void fixSave(IdSet& ids, const string& value);
//    virtual void trim(IdSetMap& idSets);
//};

class StateUpdater : virtual public BasicUpdater<vector<Ice::Long> , string>
{
public:
	typedef vector<Ice::Long> IdType;

public:
	StateUpdater(
			string database,
			string table,
			string valueColumn,
			StrSeq& idColumns,
			int tabCluster = 0,
			bool useInsertDuplicate = false);

protected:
	virtual string calc(const string& orig, const string& inc);
	virtual void fixSave(IdSet& ids, const string& value);
	virtual void trim(IdSetMap& idSets);

	void save(IdSet& ids, const string& value, const string& table,
			const string& pattern);

private:
	int _tabCluster;
	string _patternPrefix;
	bool _useInsertDuplicate;
};

};
};
};
};

#endif

