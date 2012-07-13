#ifndef __NETWORK_COMMON_H__
#define __NETWORK_COMMON_H__

#include "ConnectionQuery.h"
#include "UserNetwork.h"
#include "DataSet.h"

namespace mop
{
namespace hi
{
namespace svc
{
namespace model
{
using namespace MyUtil;
using namespace com::xiaonei::xce;
//----------------------------------------------------------------------------
class NetworkHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};


//----------------------------------------------------------------------------

class HighSchoolInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};



//----------------------------------------------------------------------------

class UniversityInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};



//----------------------------------------------------------------------------
class WorkplaceInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};


//----------------------------------------------------------------------------
class CollegeInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};


//----------------------------------------------------------------------------
class ElementarySchoolInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};


//----------------------------------------------------------------------------
class JuniorHighSchoolInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};

//----------------------------------------------------------------------------
class RegionInfoHandle : public DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
	virtual void setProperty(const string& key, const string& value);
	virtual Str2StrMap getProperties();
	virtual void setProperties(const Str2StrMap& properties);

	static DataHandlePtr parse(mysqlpp::Row& row);
};


};
};
};
};
#endif

