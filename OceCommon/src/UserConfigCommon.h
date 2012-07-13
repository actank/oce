#ifndef __USER_CONFIG_COMMON_H__
#define __USER_CONFIG_COMMON_H__

#include "UserConfig.h"
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

using namespace std;
class UserConfigDataI;
typedef IceUtil::Handle<UserConfigDataI> UserConfigDataIPtr;
class UserConfigDataI : public UserConfigData,
        public IceUtil::Mutex
{
public:
        virtual string getProperty(const string& key);
        virtual void setProperty(const string& key, const string& value);
        virtual Str2StrMap getProperties();
        virtual void setProperties(const Str2StrMap& properties);

        void parse(mysqlpp::Row& row);
        void parse(Ice::ObjectPtr& obj);
};


};
};
};
};
#endif

