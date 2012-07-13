#ifndef __USER_COMMON_H__
#define __USER_COMMON_H__

#include "User.h"
#include "DataSet.h"
#include "mysql++/mysql++.h"

namespace mop
{
namespace hi
{
namespace svc
{
namespace model
{
using namespace MyUtil;

class UserHandle : public MyUtil::DataHandle,
        public IceUtil::Mutex
{
public:
        virtual string getProperty(const string& key);
        virtual void setProperty(const string& key, const string& value);
        virtual Str2StrMap getProperties();
        virtual void setProperties(const Str2StrMap& properties);

        static DataHandlePtr parse(mysqlpp::Row& row);
};

class UserBasicHandle : public MyUtil::DataHandle,
        public IceUtil::Mutex
{
public:
        virtual string getProperty(const string& key);
        virtual void setProperty(const string& key, const string& value);
        virtual Str2StrMap getProperties();
        virtual void setProperties(const Str2StrMap& properties);

        static DataHandlePtr parse(mysqlpp::Row& row);
};

class UserDescHandle : public MyUtil::DataHandle,
        public IceUtil::Mutex
{
public:
        virtual string getProperty(const string& key);
        virtual void setProperty(const string& key, const string& value);
        virtual Str2StrMap getProperties();
        virtual void setProperties(const Str2StrMap& properties);

        static DataHandlePtr parse(mysqlpp::Row& row);
};
class ExtraInfoHandle : public MyUtil::DataHandle,
	public IceUtil::Mutex
{
public:
	virtual string getProperty(const string& key);
        virtual void setProperty(const string& key, const string& value);
        virtual Str2StrMap getProperties();
        virtual void setProperties(const Str2StrMap& properties);

        static DataHandlePtr parse(mysqlpp::Row& row);
};


class ValueAddedDataHandle : public MyUtil::DataHandle,
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

