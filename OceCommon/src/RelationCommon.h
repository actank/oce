#ifndef __RELATION_COMMON_h__
#define __RELATION_COMMON_h__

#include "Relation.h"
#include "DataSet.h"
//#include "SqlrWrapper.h"

namespace mop
{
namespace hi
{
namespace svc
{
namespace relation
{
using namespace MyUtil;

class BuddyHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual bool less(const DataHandlePtr& rhs) { 
			return value<BuddyDataPtr>()->buddyId < rhs->value<BuddyDataPtr>()->buddyId;
	}
	virtual bool greater(const DataHandlePtr& rhs) { 
			return value<BuddyDataPtr>()->buddyId > rhs->value<BuddyDataPtr>()->buddyId;
	};
	virtual bool equal(const DataHandlePtr& rhs) { 
			return value<BuddyDataPtr>()->buddyId == rhs->value<BuddyDataPtr>()->buddyId;
	};
	
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	//static DataHandlePtr parse(Connection& conn, int row);
	static DataHandlePtr parse(mysqlpp::Row& row);
};

//---------------------------------------------------------------------------
class GroupHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual bool less(const DataHandlePtr& rhs) { 
			return value<GroupDataPtr>()->groupId < rhs->value<GroupDataPtr>()->groupId;
	}
	virtual bool greater(const DataHandlePtr& rhs) { 
			return value<GroupDataPtr>()->groupId == rhs->value<GroupDataPtr>()->groupId;
	}
	virtual bool equal(const DataHandlePtr& rhs) { 
			return value<GroupDataPtr>()->groupId > rhs->value<GroupDataPtr>()->groupId;
	}
	
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	//static DataHandlePtr parse(Connection& conn, int row);
	static DataHandlePtr parse(mysqlpp::Row& row);
};

//---------------------------------------------------------------------------
class BlockHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual bool less(const DataHandlePtr& rhs) { 
			return value<BlockDataPtr>()->blockeeId < rhs->value<BlockDataPtr>()->blockeeId;
	}
	virtual bool greater(const DataHandlePtr& rhs) { 
			return value<BlockDataPtr>()->blockeeId == rhs->value<BlockDataPtr>()->blockeeId;
	}
	virtual bool equal(const DataHandlePtr& rhs) { 
			return value<BlockDataPtr>()->blockeeId > rhs->value<BlockDataPtr>()->blockeeId;
	}
	
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	//static DataHandlePtr parse(Connection& conn, int row);
	static DataHandlePtr parse(mysqlpp::Row& row);
};

//---------------------------------------------------------------------------
class BlockedHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual bool less(const DataHandlePtr& rhs) { 
			return value<BlockedDataPtr>()->blockerId < rhs->value<BlockedDataPtr>()->blockerId;
	}
	virtual bool greater(const DataHandlePtr& rhs) { 
			return value<BlockedDataPtr>()->blockerId == rhs->value<BlockedDataPtr>()->blockerId;
	}
	virtual bool equal(const DataHandlePtr& rhs) { 
			return value<BlockedDataPtr>()->blockerId > rhs->value<BlockedDataPtr>()->blockerId;
	}
	
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	//static DataHandlePtr parse(Connection& conn, int row);
	static DataHandlePtr parse(mysqlpp::Row&  row);
};

//---------------------------------------------------------------------------
class RequestHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual bool less(const DataHandlePtr& rhs) { 
			return value<RequestDataPtr>()->applicant < rhs->value<RequestDataPtr>()->applicant;
	}
	virtual bool greater(const DataHandlePtr& rhs){ 
			return value<RequestDataPtr>()->applicant < rhs->value<RequestDataPtr>()->applicant;
	}
	virtual bool equal(const DataHandlePtr& rhs){ 
			return value<RequestDataPtr>()->applicant < rhs->value<RequestDataPtr>()->applicant;
	}
	
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	//static DataHandlePtr parse(Connection& conn, int row);
	static DataHandlePtr parse(mysqlpp::Row&  row);
};

}
}
}
}

#endif

