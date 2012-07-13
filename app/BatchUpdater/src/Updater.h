#ifndef __Updater_h__
#define __Updater_h__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <set>

namespace mop
{

namespace hi
{

namespace svc
{

namespace model
{
const std::string OP_PLUS=" + ";
const std::string OP_MINUS=" - ";

template <class IdType, class ValueType>
class Updater : virtual public IceUtil::Thread,
    virtual public IceUtil::Monitor<IceUtil::Mutex>
{
public:
	Updater(std::string table, std::string valueColumn,
		std::string idColumn, std::string op = " + ")
		: _table(table), _valueColumn(valueColumn), _idColumn(idColumn), _op(op)
	{
		start().detach();
	};
	
	~Updater(){};
protected:

    typedef std::set<IdType> IdSet;
	typedef std::map<IdType, ValueType> Id2ValueMap;
	typedef std::map<ValueType, IdSet> Value2IdSetMap;
	
    Id2ValueMap _id2ValueMap;    
    Value2IdSetMap _value2SetMap;
    Value2IdSetMap _value2SetMapSwap;

 	void run()
 	{
 		while (true)
 		{
			swapValue2SetMap();
			syncValue2SetMap();
		};
	};

	virtual void updateValue(IdType id, ValueType value) = 0;
    virtual void updateValue(IdSet& ids, ValueType value) = 0;
	virtual void syncValue2SetMap() = 0;
    
    void swapValue2SetMap()
    {
    	while (true) {
			// swap _count2SetMap to _count2SetMapSwap and clear _id2CountMap
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this); 
			if (_id2ValueMap.empty()) {
				try {
		            wait();
		        } catch (...) {
		            // do nothing
		        }
		        continue;
		    }
			_id2ValueMap.clear();
		    _value2SetMap.swap(_value2SetMapSwap);
		    break;
		}
    }
    

	std::string _table;
	std::string _valueColumn;
	std::string _idColumn;
	std::string _op;
};

};
};
};
};

#endif

